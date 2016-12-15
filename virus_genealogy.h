#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <map>
#include <memory>
#include <algorithm>

class VirusNotFound : std::exception { };

class VirusAlreadyCreated : std::exception { };

class TriedToRemoveStemVirus : std::exception { };

template <class Virus>
class VirusGenealogy
{
private:
    typedef typename Virus::id_type ID;
/*
    struct ptr_comparator {
        bool operator() (const std::shared_ptr<ID>& l, const std::shared_ptr<ID>& r) {
            return *l < *r;
        }
    };

    template <typename Enumerable, typename Function>
    Enumerable select(Enumerable enumerable, Function handler) {
        std::transform(enumerable.begin(), enumerable.end(), enumerable.begin(), handler);
        return enumerable;
    }*/

    typedef std::vector<std::weak_ptr<ID>/*, ptr_comparator*/> IDSet;

    struct Node {
        const std::shared_ptr<ID> id_ptr;
        Virus virus;
        IDSet descendants;
        IDSet ascendants;
        Node(const ID& id)
            : id_ptr(std::make_shared<ID>(id))
            , virus(id)
            , descendants()
            , ascendants()
        {}
    };

    std::map<ID, std::unique_ptr<Node>> nodes;

    ID _stem_id;

public:
    VirusGenealogy(const ID& stem_id)
    {
        _stem_id = stem_id;
        nodes.insert(std::make_pair(stem_id, std::make_unique<Node>(stem_id)));
    }

    ID get_stem_id() const
    {
        return _stem_id;
    }

    std::vector<ID> get_children(const ID& id) const
    {
        try {
            Node& current = *nodes.at(id);
            std::vector<ID> children;
            for (auto& x : current.descendants) {
                children.push_back(*(x.lock()));
            }
            return children;
        }
        catch (const std::out_of_range& oor) {
            //throw VirusNotFound();
        }
        return std::vector<ID>(); // żeby się kompilator odczepił
    }

    std::vector<ID> get_parents(const ID& id) const
    {
        try {
            Node& current = *nodes.at(id);
            std::vector<ID> parents;
            for (auto& x : current.ascendants) {
                parents.push_back(*(x.lock()));
            }
            return parents;
        }
        catch (const std::out_of_range& oor) {
            //throw VirusNotFound();
        }
        return std::vector<ID>(); // żeby się kompilator odczepił
    }

    Virus& operator[](const ID& id) const
    {
        try {
            return nodes.at(id)->virus;
        }
        catch (std::out_of_range e) {
            throw VirusNotFound();
        }
    }

    void create(const ID& id, ID& parent_id)
    {
        if(nodes.count(id) > 0) throw VirusAlreadyCreated();
        if(nodes.count(parent_id) == 0) throw VirusNotFound();

        auto node = std::make_unique<Node>(id);
        node->ascendants.push_back(std::weak_ptr<ID>(nodes[parent_id]->id_ptr));
        nodes.insert(std::make_pair(id, std::move(node)));
    }

    void create(const ID& id, const std::vector<ID>& parent_ids)
    {
        if(nodes.count(id) > 0) throw VirusAlreadyCreated();
        for(auto parent_id : parent_ids)
            if(nodes.count(parent_id) == 0) throw VirusNotFound();

        auto node = std::make_unique<Node>(id);
        for(auto& parent_id : parent_ids)
            node->ascendants.push_back(std::weak_ptr<ID>(nodes[parent_id]->id_ptr));
        nodes.insert(std::make_pair(id, std::move(node)));
    }

    void connect(const ID& child_id, const ID& parent_id)
    {
        try {
            Node& child = *nodes.at(child_id);
            Node& parent = *nodes.at(parent_id);
            std::weak_ptr<ID> parent_id_ptr = parent.id_ptr;
            if(std::find(child.ascendants.begin(), child.ascendants.end(), parent_id_ptr) != child.ascendants.end()) {
                child.ascendants.push_back(parent_id_ptr);
                try {
                    parent.descendants.push_back(std::weak_ptr<ID>(child.id_ptr));
                }
                catch (...) {
                    child.ascendants.pop_back();
                    throw;
                }
            }
        }
        catch (const std::out_of_range& oor) {
            //throw
        }
    }

    void remove(const ID& id)
    {
        //TODO
    }

    bool exists(const ID& id)
    {
        //TODO
    }
};

#endif //VIRUS_GENEALOGY_H
