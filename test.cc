#include "virus_genealogy.h"
#include <iostream>

class Virus {
public:
    typedef typename std::string id_type;
    Virus(id_type const &_id) : id(_id) {
    }
    id_type get_id() const {
        return id;
    }
private:
    id_type id;
};

int main(void) {
    VirusGenealogy<Virus> k(std::string("kek"));
    std::cout << k.get_stem_id() << std::endl;

    std::vector<std::string> parents = k.get_parents("kek");
    std::cout << parents.empty() << std::endl;
    return 0;
}
