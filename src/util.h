#ifndef VMAG_UTIL_H
#define VMAG_UTIL_H

#include "vmag.h"
#include "vmagc.h"

namespace vmag{
    void printRBT(ResourceBlockTable&);
    void printUser(UserTable&);
    void printRange(LevelTable&);
    void printCost(Cost&);

    bool parseRBT(const std::string, ResourceBlockTable&);
    bool parseUser(const std::string, UserTable&, UserViewTable&);
}

namespace vmagc{
    bool parseRBT(const std::string, vmagc& vm);
    bool parseUser(const std::string, vmagc& vm);
}

#endif //VMAG_UTIL_H
