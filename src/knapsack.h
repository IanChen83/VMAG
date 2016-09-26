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
                int* table = new int[maxCost + 1];
                for(int i = 0; i < maxCost + 1; ++i){
                    table[i] = -1;
                }

                int temp = 0;

                for(int idx = 0; idx < items.size(); ++idx){
                    auto i = items[idx];
                    for(int j = maxCost; j - i.cost >= 0; --j){
                        temp = std::max(value[j], value[j - i.cost] + i.value);
                        if(temp != value[j]){
                            table[j] = idx;
                            value[j] = temp;
                        }
                    }
                }

                resultValue = value[maxCost];

                resultCost = maxCost;
                while(resultCost >= 1 && value[resultCost - 1] == resultValue)
                    --resultCost;

                resultItems.clear();

                int c = maxCost;
                auto p = table[c];
                while(p != -1){
                    auto x = items[p];
                    resultItems.push_back(x);
                    c -= x.cost;
                    p = table[c];
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
