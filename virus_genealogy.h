#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <map>

template <class Virus>
class VirusGenealogy
{
private:
    typedef Virus::id_type ID;
    typedef std::vector<Virus::id_type> IDVector;

    ID _stem_id;

    std::map<ID, IDVector> descendants; // TODO: zamienić na smart pointery
    std::map<ID, IDVector> ascendants; // TODO: zamienić na smart pointery
    std::map<ID, Virus> viruses;

public:
    VirusGenealogy(const ID& stem_id)
    {
        _stem_id = stem_id;
        viruses.emplace(stem_id, stem_id);
        //TODO
    }

    ID get_stem_id() const
    {
        return _stem_id;
        //TODO
    }

    IDVector get_children(const ID& id) const
    {
        try {
            return descendants.at(id);
        }
        catch (const std::out_of_range& oor) {
            throw VirusNotFound(); 
        }
    }

    IDVector get_parents(const ID& id) const
    {
        try {
            return ascendants.at(id);
        }
        catch (const std::out_of_range& oor) {
            throw VirusNotFound(); 
        }
    }

    Virus& operator[](const ID& id) const
    {
        try {
            return viruses.at(id);
        }
        catch (const std::out_of_range& oor) {
            throw VirusNotFound(); 
        }
    }

    void create(const ID& id, const ID& parent_id)
    {
        //TODO
    }

    void create(const ID& id, const IDVector& parent_ids)
    {
        //TODO
    }

    void connect(const ID& child_id, const ID& parent_id)
    {
        try {
            IDVector& parent_desc = descendants.at(parent_id);
            IDVector& child_asc = ascendants.at(child_id);
            parent_desc.push_back(child_id);
            try {
                child_asc.push_back(parent_id);
            }
            catch (...) {
                parent_desc.pop_back();
                throw;
            }
        }
        catch (const std::out_of_range& oor) {
            throw VirusNotFound();
        }
    }

    void remove(const ID& id)
    {
        //TODO
    }
};

#endif //VIRUS_GENEALOGY_H
