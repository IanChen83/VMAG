#ifndef VMAG_UTIL_H
#define VMAG_UTIL_H

#include "vmag.h"

namespace vmag{
    void printRBT(ResourceBlockTable&);
    void printUser(UserTable&);
    void printRange(LevelTable&);
    void printCost(Cost&);

    bool parseRBT(const std::string, ResourceBlockTable&);
    bool parseUser(const std::string, UserTable&, UserViewTable&);
}

namespace vmagc{
    bool parseRBT(const std::string);
    bool parseUser(const std::string);
}

#endif //VMAG_UTIL_H
