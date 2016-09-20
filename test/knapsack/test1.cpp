#include "../../knapsack.h"
#include <iostream>

using namespace std;

knapsack::kitem item_gen(int cost, int value){
    knapsack::kitem ret;
    ret.cost = cost;
    ret.value = value;
    return ret;
}

int main(){

    knapsack::knapsack<knapsack::kitem> k;

    k.items.push_back(item_gen(400, 850));
    k.items.push_back(item_gen(150, 300));
    k.items.push_back(item_gen(400, 850));
    k.items.push_back(item_gen(400, 850));

    k.maxCost = 1000;

    k.run();

    return 0;
}
