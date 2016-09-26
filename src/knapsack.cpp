#include "knapsack.h"
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;
namespace knapsack{
    template<> void knapsack<kitem>::run(){
        int* value = new int[maxCost + 1]();
        int* table = new int[maxCost + 1]();
        int temp = 0;

        for(int i = 0; i < items.size(); ++i){
            for(int j = maxCost; j - items[i].cost >= 0; --j){
                if((temp = max(value[j], value[j - items[i].cost] + items[i].value) != value[j])){
                    table[j] = i;
                    value[j] = temp;
                }
            }
        }

        resultValue = value[maxCost - 1];

        resultCost = maxCost - 1;
        while(resultCost >= 1 && value[resultCost - 1] == resultValue)
            --resultCost;

        int current = maxCost;
        int p;
        resultItems.clear();
        while(current >= 0){
            p = table[current];
            resultItems.push_back(items[p]);
            current -= items[p].cost;
        }

        delete[] value;
        delete[] table;
    }
}
