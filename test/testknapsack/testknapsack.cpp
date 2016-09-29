#include "testknapsack.h"
#include <vector>
#include <sstream>
#include <string>

knapsack::kitem<int> KnapsackTest::item_gen(int cost, int value, int id = 0){
    knapsack::kitem<int> ret;
    ret.data = id;
    ret.cost = cost;
    ret.value = value;
    return ret;
}

KnapsackTest::KnapsackTest(){}

TEST_F(KnapsackTest, InitTest){
    ASSERT_EQ(k.items.size(), 0);
}

TEST_F(KnapsackTest, NoResultTest){
    k.items.push_back(item_gen(150, 300));
    k.items.push_back(item_gen(400, 900));
    k.items.push_back(item_gen(400, 850));
    k.items.push_back(item_gen(400, 800));

    k.maxCost = 100;

    k.run();

    EXPECT_EQ(k.resultItems.size(), 0);
    EXPECT_EQ(k.resultCost, 0);
    EXPECT_EQ(k.resultValue, 0);
}

TEST_F(KnapsackTest, KnapsackTest_1){
    k.items.push_back(item_gen(150, 300));
    k.items.push_back(item_gen(400, 900));
    k.items.push_back(item_gen(400, 850));
    k.items.push_back(item_gen(400, 800));

    k.maxCost = 1000;

    k.run();

    EXPECT_EQ(k.resultItems.size(), 3);
    EXPECT_EQ(k.resultCost, 950);
    EXPECT_EQ(k.resultValue, 2050);
}

TEST_F(KnapsackTest, KnapsackTest_2){
    k.items.push_back(item_gen(400, 850, 2));
    k.items.push_back(item_gen(150, 300, 8));
    k.items.push_back(item_gen(400, 900, 4));
    k.items.push_back(item_gen(400, 800, 1));
    k.maxCost = 1000;

    k.run();

    int idmask = 0;
    std::stringstream ss;

    ss << "Result items: ";

    for(auto& x : k.resultItems){
        ss << '(' << x.data << ',' << x.cost << ',' << x.value << ')';
        idmask |= x.data;
    }

    std::string error_ss = ss.str();

    EXPECT_EQ(k.resultItems.size(), 3);
    EXPECT_EQ(k.resultCost, 950);
    EXPECT_EQ(k.resultValue, 2050);
    EXPECT_EQ(idmask, 14) << error_ss;
}

