#include "testvmagc.h"
#include <vector>
#include <sstream>
#include <string>

VmagcTest::VmagcTest(){}

TEST_F(VmagcTest, InitTest){
    EXPECT_EQ(vm.result.size(), 0);
}

TEST_F(VmagcTest, GetPatternTest){
    // Boundary checking
    int x = vm.get_pattern(-1, 10, 6, 9, -1, 8);
    EXPECT_GE(x, 1);
    EXPECT_LE(x, 18);
}

TEST_F(VmagcTest, VmagcTest){
    vmagc::vmagc vm;

    vmagc::parseUser("/home/patrickchen/Documents/VMAG/test/testvmagc/user.txt" ,vm);
    vmagc::parseRBT("/home/patrickchen/Documents/VMAG/test/testvmagc/slot.txt" ,vm);

    vm.VMAGC();
}
