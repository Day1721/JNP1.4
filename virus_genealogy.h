#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <map>
#include <memory>
#include <algorithm>

class VirusNotFound : std::exception { };

class VirusAlreadyCreated : std::exception { };

class TriedToRemoveStemVirus : std::exception { };

template<class Virus>
class VirusGenealogy {
private:
    typedef typename Virus::id_type ID;

    struct Node;
    typedef std::vector<std::weak_ptr<Node>> NodeVector;

    struct Node {
        Virus virus;
        NodeVector descendants;
        NodeVector ascendants;

        Node(const ID& id)
                : virus(id), descendants(), ascendants() { }
    };

    std::map<ID, std::shared_ptr<Node>> nodes;

    ID _stem_id;

public:
    VirusGenealogy(const ID& stem_id) {
        _stem_id = stem_id;
        nodes.insert(std::make_pair(stem_id, std::make_shared<Node>(stem_id)));
    }

    ID get_stem_id() const {
        return _stem_id;
    }

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
            //throw VirusNotFound();
        }
        return std::vector<ID>(); // żeby się kompilator odczepił
    }

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
            //throw VirusNotFound();
        }
        return std::vector<ID>(); // żeby się kompilator odczepił
    }

    Virus& operator [](const ID& id) const {
        try {
            return nodes.at(id)->virus;
        }
        catch (std::out_of_range e) {
            throw VirusNotFound();
        }
    }

    void create(const ID& id, const ID& parent_id) {
        if (nodes.count(id) > 0) throw VirusAlreadyCreated();
        if (nodes.count(parent_id) == 0) throw VirusNotFound();

        auto node = std::make_shared<Node>(id);
        auto parent = nodes[parent_id];
        node->ascendants.push_back(std::weak_ptr<Node>(parent));
        parent->descendants.push_back(std::weak_ptr<Node>(node));

        try {
            nodes.insert(std::make_pair(id, std::move(node)));
        }
        catch (...) {
            parent->descendants.pop_back();
        }
    }

    void create(const ID& id, const std::vector<ID>& parent_ids) {
        if (nodes.count(id) > 0) throw VirusAlreadyCreated();
        for (auto& parent_id : parent_ids)
            if (nodes.count(parent_id) == 0) throw VirusNotFound();

        auto node = std::make_shared<Node>(id);
        for (auto& parent_id : parent_ids)
            node->ascendants.push_back(std::weak_ptr<Node>(nodes[parent_id]));

        std::vector<std::shared_ptr<Node>> parents;
        typename std::vector<std::shared_ptr<Node>>::iterator it;

        for (auto& parent_id : parent_ids) {
            parents.push_back(nodes[parent_id]);
        }
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

    void connect(const ID& child_id, const ID& parent_id) {
        std::shared_ptr<Node> child, parent;
        try {
            child = nodes.at(child_id);
            parent = nodes.at(parent_id);
        }
        catch (const std::out_of_range& oor) {
            //throw
        }

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
        using iterators = std::vector<std::vector<std::weak_ptr<Node>>::iterator>;
        if(id == _stem_id) throw TriedToRemoveStemVirus();
        if(nodes.count(id) == 0) throw VirusNotFound();
        std::shared_ptr<Node> node = nodes[id];

        NodeVector::iterator ascendants_iterator, descendants_iterator;
        bool flag = false;

        std::vector<NodeVector::const_iterator> descendants, ascendants;/*
        for(ascendants_iterator = node->ascendants.begin(); ascendants_iterator < node->ascendants.end(); ascendants_iterator++) {


        }*/

        for (ascendants_iterator = node->ascendants.begin(); ascendants_iterator < node->ascendants.end(); ascendants_iterator++) {
            (*ascendants_iterator)->descendants.erase((*ascendants_iterator)->descendants.find(nodes[id]));
        }
        flag = true;
        for (descendants_iterator = node->descendants.begin(); descendants_iterator < node->descendants.end(); descendants_iterator++) {
            (*descendants_iterator)->ascendants.erase((*descendants_iterator)->ascendants.find(nodes[id]));
        }

        try {
            for (ascendants_iterator = node->ascendants.begin(); ascendants_iterator < node->ascendants.end(); ascendants_iterator++) {
                (*ascendants_iterator)->descendants.erase((*ascendants_iterator)->descendants.find(nodes[id]));
            }
            flag = true;
            for (descendants_iterator = node->descendants.begin(); descendants_iterator < node->descendants.end(); descendants_iterator++) {
                (*descendants_iterator)->ascendants.erase((*descendants_iterator)->ascendants.find(nodes[id]));
            }
            //TODO recursive remove of descendants
            nodes.erase(id);
        }
        catch (...) {
            if(flag) {
                //TODO
            }
            //TODO
        }
    }

    bool exists(const ID& id) {
        return nodes.count(id) > 0;
    }

private:
    NodeVector::const_iterator find(const typename std::vector<std::weak_ptr<Node>> vector, std::weak_ptr<Node> ptr) {
        for(auto iterator = vector.begin(); iterator < vector.end(); iterator++) {
            if(ptr->id == iterator->virus.get_id())
                return iterator;
        }
        return vector.end();
    }
};

#endif //VIRUS_GENEALOGY_H
