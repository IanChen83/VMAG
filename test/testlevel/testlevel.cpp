#include "testlevel.h"

using namespace vmag;

LevelTest::LevelTest(){}

TEST_F(LevelTest, AddRangeTest){
    ASSERT_EQ(level.level.size(), 0) << "Initial level size should be zero";
    level.addRange(1, 2);

    ASSERT_EQ(level.level.size(), 1) << "Range(1, 2) is not added to level";

    level.addRange(Range(3, 4));
    ASSERT_EQ(level.level.size(), 2) << "addRange(const Range &) is wrong";
}

TEST_F(LevelTest, CopyCostructorTest){
    level.addRange(1, 2);
    auto x(level);
    ASSERT_TRUE(x.isInRange(1));
}
