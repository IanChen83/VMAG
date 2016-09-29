#ifndef VMAGC_KNAPSACK_H
#define VMAGC_KNAPSACK_H

#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>

void print(int** value, int i, int j){
    if(value[i][j] != 0)
        std::cout << i << '\t' << value[i][j] << std::endl;
}

void stub(){
    return;
}

namespace knapsack{
    template <class T>
        struct kitem{
            int cost;
            int value;
            T data;

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

            void run(){
                int size = items.size();
                int* value = new int[maxCost + 1]();
                int* record = new int[maxCost + 1]();
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
                                if(x.value == items[i].value){
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

                delete[] value;

            };

            void reset(){
                maxCost = resultValue = resultCost = 0;
                resultItems.clear();
                items.clear();
            }
        };
}

#endif
