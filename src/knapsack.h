#ifndef VMAGC_KNAPSACK_H
#define VMAGC_KNAPSACK_H

#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>

namespace knapsack{
    template <class T>
        struct kitem{
            int cost;
            int value;
            T data;

            kitem(){}
            kitem(int _c, int _v, T _d): cost(_c), value(_v), data(_d){}

            bool operator< (const kitem& rhs){ return this->value < rhs.value; }
            bool operator> (const kitem& rhs){ return rhs < *this; }
            bool operator<=(const kitem& rhs){ return !(*this > rhs); }
            bool operator>=(const kitem& rhs){ return !(*this < rhs); }
        };


    template <class T>
        struct knapsack{
            int maxCost;
            std::vector< kitem<T> > items;

            int resultValue;
            int resultCost;
            std::vector< kitem<T> > resultItems;

            void normalize(){
                int minValue = INT_MAX;
                for(auto& x : items){
                    minValue = std::min(minValue, x.value);
                    if(x.cost == 0){
                        std::cout << "Normalize cost (0) to 1" << std::endl;
                        x.cost = 1;
                    }
                }
                if(minValue <= 0){
                    for(auto& x : items){
                        std::cout << "Normalize value (" << x.value << ") to ";
                        x.value -= minValue;
                        ++x.value;
                        std::cout << x.value << std::endl;
                    }
                }
            }

            void run(){
                std::cout << "=================== Knapsack Start ===================" << std::endl;
                int size = items.size();
                int* value = new int[maxCost + 1];
                int* record = new int[maxCost + 1];

                normalize();

                for(int i = 0; i <= maxCost; ++i){
                    value[i] = 0;
                    record[i] = 0;
                }

                std::sort(items.begin(), items.end());
                std::reverse(items.begin(), items.end());

                int temp;
                for(int i = 0; i < size; ++i){
                    for(int j = maxCost; j - items[i].cost >= 0; --j){
                        if((temp = std::max(value[j], value[j - items[i].cost] + items[i].value)) != value[j]){
                            value[j] = temp;
                            record[j] = i + 1;
                        }
                    }
                }


                resultValue = value[maxCost];
                resultCost = maxCost;
                while(resultCost > 0 && value[resultCost] == value[resultCost - 1])
                    --resultCost;

                int cost = resultCost;
                while(record[cost] != 0){
                    for(int i = 0; i < size; ++i){
                        if(items[i].cost == items[record[cost] - 1].cost){
                            bool isDup = false;
                            for(auto& x : resultItems){
                                if(x.value == items[i].value && x.data == items[i].data){
                                    isDup = true;
                                    break;
                                }
                            }
                            if(!isDup){
                                resultItems.push_back(items[i]);
                                break;
                            }
                        }
                    }
                    cost -= items[record[cost]-1].cost;
                }

                std::cout << "Result value: " << resultValue << std::endl;
                std::cout << "Result cost: " << resultCost << std::endl;
                std::cout << "Result Item(cost, value, data): ";
                for(auto& x : resultItems){
                    std::cout << "(" << x.cost << "," << x.value << ") ";
                }
                std::cout << std::endl;
                std::cout << "==================== Knapsack End ====================" << std::endl;
            };

            void reset(){
                maxCost = resultValue = resultCost = 0;
                resultItems.clear();
                items.clear();
            }
        };
}

#endif
