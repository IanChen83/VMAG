#include "vmag.hpp"
#include <cstdlib>
#include <iostream>

ResourceBlockTable RBT = {0};
RangeIndicator RI;
CostTable cost[MCS_LEVEL + 1];
UserTable user = {0};
UserViewTable userView = {0};

using namespace std;

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Usage: %s <dir>\n", argv[0]);
        return 1;
    }

    char resolved_path[PATH_MAX];
    realpath(argv[1], resolved_path);
    parseRBT(string(resolved_path) + "/slot.txt");
    parseUser(string(resolved_path) + "/user.txt");

    VMAG();

    printCost(cost);
    printUser(user);

    //TODO: Output result

    return 0;
}

