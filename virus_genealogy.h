#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>

template <class TVirus>
class VirusGenealogy
{
private:
    TVirus::id_type _id;

    std::map<TVirus::id_type, std::vector<TVirus::id_type>> descendants;
    std::map<TVirus::id_type, std::vector<TVirus::id_type>> ascendants;

public:
    VirusGenealogy(const TVirus::id_type& stem_id)
    {
        //TODO
    }

    TVirus::id_type get_stem_id() const
    {
        return _id;
    }
};

#endif //VIRUS_GENEALOGY_H
