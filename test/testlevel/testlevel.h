#include "gtest/gtest.h"
#include "range.h"
#include "level.h"

class LevelTest : public ::testing::Test {
    public:
        LevelTest();
        vmag::Level level;
};
