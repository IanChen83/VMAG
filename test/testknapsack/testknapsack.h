#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "knapsack.h"

class KnapsackTest : public ::testing::Test {
    public:
        knapsack::kitem<int> item_gen(int, int, int);
        KnapsackTest();
        knapsack::knapsack<int> k;
};
