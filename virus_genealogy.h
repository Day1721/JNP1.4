#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <map>
#include <memory>

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
*/
    typedef std::vector<std::shared_ptr<ID>/*, ptr_comparator*/> IDSet;

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
            return std::vector(nodes.at(id)->descendants);
		}
        catch (const std::out_of_range& oor) {
            //throw VirusNotFound();
        }
        return std::vector<ID>(); // żeby się kompilator odczepił
    }

    std::vector<ID> get_parents(const ID& id) const
    {
		try {
            return std::vector(nodes.at(id)->ascendants);
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

    void create(const ID& id, const ID& parent_id)
    {
        //TODO
    }

    void create(const ID& id, const std::vector<ID>& parent_ids)
    {
        //TODO
    }

    void connect(const ID& child_id, const ID& parent_id)
    {
        // TODO
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
