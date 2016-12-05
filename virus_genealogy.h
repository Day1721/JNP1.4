#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <map>

template <class TVirus>
class VirusGenealogy
{
private:
    TVirus::id_type _stem_id;

    std::map<TVirus::id_type, std::vector<TVirus::id_type>> descendants;
    std::map<TVirus::id_type, std::vector<TVirus::id_type>> ascendants;
    std::map<TVirus::id_type, TVirus> viruses;

public:
    VirusGenealogy(const TVirus::id_type& stem_id)
    {
        _stem_id = stem_id;
        //TODO
    }

    TVirus::id_type get_stem_id() const
    {
        return _stem_id;
        //TODO
    }

    std::vector<TVirus::id_type> get_children(const TVirus::id_type& id) const
    {
        return descendants[id];
        //TODO
    }

    std::vector<TVirus::id_type> get_parents(const TVirus::id_type& id) const
    {
        return ascendants[id];
        //TODO
    }

    TVirus& operator[](const TVirus::id_type& id) const
    {
        return viruses[id];
    }

    void create(const TVirus::id_type& id, const TVirus::id_type& parent_id)
    {
        //TODO
    }

    void create(const TVirus::id_type& id, const std::vector<TVirus::id_type>& parent_ids)
    {
        //TODO
    }

    void connect(const TVirus::id_type& child_id, const TVirus::id_type& parent_id)
    {
        //TODO
    }

    void remove(const TVirus::id_type& id)
    {
        //TODO
    }
};

#endif //VIRUS_GENEALOGY_H
