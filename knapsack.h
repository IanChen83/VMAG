#ifndef VMAGC_KNAPSACK_H
#define VMAGC_KNAPSACK_H

#include <vector>
#include <cassert>

namespace knapsack{
    struct kitem{
        int cost;
        int value;
    };

    template <typename item>
        struct knapsack{
            static_assert(std::is_base_of<kitem, item>::value, "item must be derived from kitem");

            int maxCost;
            std::vector<item> items;

            int resultValue;
            int resultCost;
            std::vector<item> resultItems;

            void run(){
                int* value = new int[maxCost + 1]();
                int** table = new int*[maxCost + 1]();
                int temp = 0;

                for(auto& i: items){
                    for(int j = maxCost; j - i.cost >= 0; --j){
                        if((temp = max(value[j], value[j - i.cost] + i.value) != value[j])){
                            table[j] = &(value[j - i.cost]);
                            value[j] = temp;
                        }
                    }
                }

                resultValue = value[maxCost];

                resultCost = maxCost;
                while(resultCost >= 1 && value[resultCost - 1] == resultValue)
                    --resultCost;

                int* p = table[maxCost];
                int* ptemp;
                resultItems.clear();
                while(p != NULL){
                    ptemp = (int*)*p;
                    resultItems.push_back(items[p - ptemp]);
                    p = ptemp;
                }

                delete[] value;
                delete[] table;
            }

            void reset(){
                maxCost = resultValue = resultCost = 0;
                resultItems.clear();
                items.clear();
            }
        };
}

#endif
