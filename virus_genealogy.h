#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <map>
#include <memory>
#include <algorithm>

class VirusNotFound : std::exception {
    public:
        virtual const char* what() const { return "VirusNotFound"; }
};

class VirusAlreadyCreated : std::exception {
    public:
        virtual const char* what() const { return "VirusAlreadyCreated"; }
};

class TriedToRemoveStemVirus : std::exception {
    public:
        virtual const char* what() const { return "TriedToRemoveStemVirus"; }
};

template<class Virus>
class VirusGenealogy {
private:
    typedef typename Virus::id_type ID;

    /* Uzywamy tutaj weak_ptr, bo weak_ptr daje 100% gwarancje no-throw dla
     * konstruktorow i operatorow przypisania.
     */
    struct Node;
    typedef std::vector<std::weak_ptr<Node>> NodeVector;
    struct Node {
        Virus virus;
        NodeVector descendants;
        NodeVector ascendants;

        Node(const ID& id)
                : virus(id), descendants(), ascendants() { }
    };

    /* Mapa niestety ZAWSZE moze rzucic, bo ID::operator< zawsze moze rzucic
     * PATRZ forum
     */
    std::map<ID, std::shared_ptr<Node>> nodes;

    ID _stem_id;

public:
    VirusGenealogy(const VirusGenealogy&) = delete;
    VirusGenealogy& operator=(const VirusGenealogy&) = delete;

    VirusGenealogy(const ID& stem_id) {
        _stem_id = stem_id;
        nodes.insert(std::make_pair(stem_id, std::make_shared<Node>(stem_id)));
    }

    // No-throw, chyba, ze konstruktor id rzuci
    ID get_stem_id() const {
        return _stem_id;
    }

    // Silna odpornosc
    std::vector<ID> get_children(const ID& id) const {
        try {
            Node& current = *nodes.at(id);
            std::vector<ID> children;
            for (auto& x : current.descendants) {
                children.push_back(x.lock()->virus.get_id());
            }
            return children;
        }
        catch (const std::out_of_range& oor) {
            throw VirusNotFound();
        }
        return std::vector<ID>(); // żeby się kompilator odczepił
    }

    // Silna odpornosc
    std::vector<ID> get_parents(const ID& id) const {
        try {
            Node& current = *nodes.at(id);
            std::vector<ID> parents;
            for (auto& x : current.ascendants) {
                parents.push_back(x.lock()->virus.get_id());
            }
            return parents;
        }
        catch (const std::out_of_range& oor) {
            throw VirusNotFound();
        }
        return std::vector<ID>(); // żeby się kompilator odczepił
    }

    // Silna odpornosc
    Virus& operator [](const ID& id) const {
        try {
            return nodes.at(id)->virus;
        }
        catch (const std::out_of_range& oor) {
            throw VirusNotFound();
        }
    }

    // Silna odpornosc
    void create(const ID& id, const ID& parent_id) {
        if (nodes.count(id) > 0) throw VirusAlreadyCreated();
        if (nodes.count(parent_id) == 0) throw VirusNotFound();

        auto node = std::make_shared<Node>(id);
        auto parent = nodes[parent_id];
        node->ascendants.push_back(std::weak_ptr<Node>(parent));
        parent->descendants.push_back(std::weak_ptr<Node>(node));

        // Rollback w razie wyjatku
        try {
            nodes.insert(std::make_pair(id, std::move(node)));
        }
        catch (...) {
            parent->descendants.pop_back();
            throw;
        }
    }

    // Silna odpornosc
    void create(const ID& id, const std::vector<ID>& parent_ids) {
        if (nodes.count(id) > 0) throw VirusAlreadyCreated();
        for (auto& parent_id : parent_ids)
            if (nodes.count(parent_id) == 0) throw VirusNotFound();

        auto node = std::make_shared<Node>(id);
        for (auto& parent_id : parent_ids)
            node->ascendants.push_back(std::weak_ptr<Node>(nodes[parent_id]));

        /* Robimy pomocniczy wektor wskaznikow, tak, by moc bez zagladania do wyjatkogennej
         * mapy wstawiac i wyrzucac krawedzie.
         */
        std::vector<std::shared_ptr<Node>> parents;
        typename std::vector<std::shared_ptr<Node>>::iterator it;

        for (auto& parent_id : parent_ids) {
            parents.push_back(nodes[parent_id]);
        }
        
        /* Zrobiwszy wektor uzywamy go do rollbacku w razie wyjatku. */
        try {
            for (it = parents.begin(); it < parents.end(); it++) {
                (*it)->descendants.push_back(std::weak_ptr<Node>(node));
            }
            nodes.insert(std::make_pair(id, std::move(node)));
        }
        catch (...) {
            it--;
            while (it >= parents.begin()) {
                (*it)->descendants.pop_back();
            }
            throw;
        }
    }

    // Silna odpornosc
    void connect(const ID& child_id, const ID& parent_id) {
        std::shared_ptr<Node> child, parent;
        try {
            child = nodes.at(child_id);
            parent = nodes.at(parent_id);
        }
        catch (const std::out_of_range& oor) {
            throw VirusNotFound();
        }

        /* Jak widac, uzycie std::vector do przechowywania sasiadow w grafie
         * skutkuje liniowym czasem sprawdzania, czy krawedz istnieje.
         * Niestety, nie jest latwo tego uniknac. Oto dlaczego: std::set::erase nie ma
         * gwarancji no-throw w ogolnym przypadku. A zatem aby zaimplementowac poprawnie rollback,
         * trzeba by robic kopie kontenera. To zas rowniez prowadzi do liniowej zlozonosci.
         * Byc moze daloby sie zrobic poprawne rozwiazanie z setem, gdyby przechowywac iteratory.
         * Zdecydowalismy sie pozostac przy prostszym rozwiazaniu w nadziei, ze logarytmiczny czas
         * znajdowania wirusow w mapie wystarczy.
         */
        auto it = child->ascendants.begin();
        while (it != child->ascendants.end() && it->lock() != parent)
            it++;
        if (it == child->ascendants.end()) {
            child->ascendants.push_back(std::weak_ptr<Node>(parent));
            try {
                parent->descendants.push_back(std::weak_ptr<Node>(child));
            }
            catch (...) {
                child->ascendants.pop_back();
                throw;
            }
        }
    }

    void remove(const ID& id) {
        //TODO
    }

    void propagate_removal(Node* n) {
        for (auto& n : n->descendants) {

        }
    }

    // Silna odpornosc
    // Mapa zawsze moze rzucic
    bool exists(const ID& id) {
        return nodes.count(id) > 0;
    }
};

#endif //VIRUS_GENEALOGY_H
