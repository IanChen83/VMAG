#include "knapsack.h"
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;
namespace knapsack{
    template<> void knapsack<>::run(){
        int* value = new int[maxCost]();
        int** table = new int*[maxCost]();
        int temp = 0;

        for(auto& i: items){
            for(int j = maxCost; j - i.cost >= 0; --j){
                if((temp = max(value[j], value[j - i.cost] + i.value) != value[j])){
                    table[j] = &(value[j - i.cost]);
                    value[j] = temp;
                }
            }
        }

        resultValue = value[maxCost - 1];

        resultCost = maxCost - 1;
        while(resultCost >= 1 && value[resultCost - 1] == resultValue)
            --resultCost;

        int* p = table[maxCost - 1];
        resultItems.clear();
        while(p != NULL){
            resultItems.push_back(items[p - (int*)*p]);
        }

        delete[] value;
        delete[] table;
    }
}
