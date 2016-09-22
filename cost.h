#ifndef VMAG_COST_H
#define VMAG_COST_H

#include <climits>
#include <map>

namespace vmag {
    struct CostBlock {
        CostBlock() {
            cost = INT_MAX;
            prev = -1;
            method = '\0';
        }

        int cost;
        int prev;
        char method;
    };

}
#endif //VMAG_COST_H
