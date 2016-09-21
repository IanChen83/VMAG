#include "../vmagc.h"
#include <iostream>

using namespace std;
using namespace vmagc;

int main(){
    vmagc::vmagc a;

    cout << a.get_pattern(1,4,1,4,2, 2) << endl;

    return 0;
}
