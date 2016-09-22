#include "util.h"
#include <iostream>
#include <cstdlib>
#include <climits>

using namespace std;

void userGenerator(vmag::UserTable &user, vmag::UserViewTable &userView, int s, int r) {
    srand(r);

    int level, view;
    for (int i = 0; i < s; ++i) {
        view = rand() % MCS_VIEW;
        level = rand() % MCS_LEVEL;
        user[level][view] = 1;
        userView[view] += 1;
    }

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <dir>\n", argv[0]);
        return 1;
    }

    char* resolved_path = new char[PATH_MAX];
    realpath(argv[1], resolved_path);

    for(int i = 0; i < 100; ++i){
        vmag::vmag main;
        vmag::parseRBT(string(resolved_path) + "/slot.txt", main.RBT);
        vmag::parseUser(string(resolved_path) + "/user.txt", main.user, main.userView);

        main.VMAG();
        cout << main.cost[MCS_LEVEL][0][MCS_VIEW - 1].cost << endl;
        main.reset();
    }
}
