#include "util.h"
#include "vmagc.h"
#include <iostream>
#include <cstdlib>
#include <climits>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <dir>\n", argv[0]);
        return 1;
    }

    char* resolved_path = new char[PATH_MAX];
    realpath(argv[1], resolved_path);

    vmagc::vmagc vm;

    vmagc::parseUser(string(resolved_path) + "/user.txt" ,vm);
    vmagc::parseRBT(string(resolved_path) + "/slot.txt" ,vm);

    vm.VMAGC();
}
