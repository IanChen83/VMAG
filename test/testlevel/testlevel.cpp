#include "testlevel.h"
#include <vector>
#include <iostream>

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

TEST_F(LevelTest, GetServedTest){
    std::vector<int> views;

    views.push_back(1);
    views.push_back(4);

    level = Level::get_served(views);

    ASSERT_EQ(level.size(), 1);
    EXPECT_EQ(*(level.begin()), Range(1, 4));

    views.push_back(8);
    views.push_back(10);
    views.push_back(11);
    views.push_back(12);

    level = Level::get_served(views);

    ASSERT_EQ(level.size(), 2);
    EXPECT_EQ(*(level.begin() + 1), Range(8, 12));
}

TEST_F(LevelTest, ResetTest){
    vmag::Level level2 = vmag::Level();

    level.addRange(1, 4);
    level.addRange(8, 12);
    level.reset();

    EXPECT_TRUE(level.level == level2.level);
}
