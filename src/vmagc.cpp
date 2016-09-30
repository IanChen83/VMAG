#include "level.h"
#include "util.h"
#include "vmag.h"
#include "vmagc.h"
#include "knapsack.h"
#include <cassert>
#include <iostream>
#include <tuple>
#include <algorithm>

namespace vmagc{
    vmagc::vmagc(){
        for(int i = 0; i < CARRIER_NUM; ++i){
            carriers[i] = vmag::vmag();
        }
    }
    int vmagc::get_pattern(int al, int ar, int bl, int br, int c, int loc){
        int lpattern, rpattern, y;
        y = (c == -1) ? PATTERN_NONE : PATTERN_UNDER;

        lpattern = (al != -1 && loc - al <= R) ? PATTERN_INNER : PATTERN_NONE;
        if(bl != -1){
            if(al == bl){
                lpattern = PATTERN_UNDER;
            }else if(al < bl){
                lpattern = PATTERN_NONE;
            }
        }
        rpattern = (ar != -1 && ar - loc <= R) ? PATTERN_INNER : PATTERN_NONE;
        if(br != -1){
            if(ar == br){
                rpattern = PATTERN_UNDER;
            }else if(ar > br){
                lpattern = PATTERN_NONE;
            }
        }

        return PATTERN_CODE[lpattern][rpattern][y];
    }

    VLPair_kitem vmagc::set_kitem(vmag::vmag& a, vmag::vmag& b, VLPair i){
        const VLPair NULL_VL = std::pair<int, int>(-1, -1);
        const VLPair MAX_VL = std::pair<int, int>(MCS_VIEW, MCS_VIEW);

        auto al = NULL_VL;
        auto ar = MAX_VL;
        auto bl = NULL_VL;
        auto br = MAX_VL;
        auto y = NULL_VL;

        for(auto& x : a.resultViewLevel){
            if(x.second < i.second && x.second > al.second) al = x;
            if(x.second > i.second && x.second < ar.second) ar = x;
            if(x.second == i.second) y = x;
        }

        for(auto& x : b.resultViewLevel){
            if(x.second < i.second && x.second > bl.second) bl = x;
            if(x.second > i.second && x.second < br.second) br = x;
        }

        int code = get_pattern(al.second, ar.second, bl.second, br.second, y.second, i.second);

        int cost = b.RBT[i.second][i.first];
        int value = 0;
        switch(code){
            case 1:     // None, None, None
                value = 0;
                break;
            case 2:     // None, None, Under
                value = a.RBT[y.second][y.first] - cost;
                break;
            case 3:     // None, Under, None
                value = 0;
                break;
            case 4:     // None, Under, Under
                value = a.RBT[y.second][y.first] - cost;
                break;
            case 5:     // None, Inner, None
                value = a.RBT[al.second][ al.first] / 2 - cost;
                break;
            case 6:     // None, Inner, Under
                value = a.RBT[y.second][y.first] + a.RBT[al.second][ al.first] / 2 - cost;
                break;
            case 7:     // Under, None, None
                value = 0;
                break;
            case 8:     // Under, None, Under
                value = a.RBT[y.second][y.first] - cost;
                break;
            case 9:     // Under, Under, None
                value = 0;
                break;
            case 10:    // Under, Under, Under
                value = a.RBT[y.second][y.first] - cost;
                break;
            case 11:    // Under, Inner, None
                value =  + a.RBT[al.second][ al.first] / 2 - cost;
                break;
            case 12:    // Under, Inner, Under
                value = a.RBT[y.second][y.first] + a.RBT[al.second][ al.first] / 2 - cost;
                break;
            case 13:    // Inner, None, None
                value = a.RBT[ar.second][ar.first] / 2 - cost;
                break;
            case 14:    // Inner, None, Under
                value = a.RBT[y.second][y.first] + a.RBT[ar.second][ ar.first] / 2 - cost;
                break;
            case 15:    // Inner, Under, None
                value =  + a.RBT[ar.second][ ar.first] / 2 - cost;
                break;
            case 16:    // Inner, Under, Under
                value = a.RBT[y.second][y.first] + a.RBT[ar.second][ ar.first] / 2 - cost;
                break;
            case 17:    // Inner, Inner, None
                value = a.RBT[ar.second][ar.first] + a.RBT[al.second][al.first] - cost;
                break;
            case 18:    // Inner, Inner, Under
                value = a.RBT[y.second][y.first] - cost;
                break;
            default:
                assert(false);
        }
        std::cout <<"(" << i.second << "," << i.first << ") Cost: " << cost << ", Value: " << value << ", code: " << code << std::endl;
        return VLPair_kitem(cost, value, i);
    }

    std::vector<VLPair_kitem> vmagc::dispatch(vmag::vmag& a, vmag::vmag& b){
        knapsack::knapsack<VLPair> k;
        k.maxCost = VMAGC_LIMIT;

        // Identify the dispatch value of each view
        for(auto& i : b.resultViewLevel){
            auto x = set_kitem(a, b, i);
            bool isDup = false;
            for(auto& c : result){
                if(c.second.find(i.first) != c.second.end()){
                    isDup = true;
                    break;
                }
            }
            if(!isDup)
                k.items.push_back(x);
        }

        k.run();

        return k.resultItems;
    }

    int vmagc::one_round(){
        std::vector< std::pair<int, int> > carrier_rank;

        for(int i = 0; i < CARRIER_NUM; ++i){
            if(result.find(i) == result.end()){
                carriers[i].VMAG();
                std::cout << i << ' ' << carriers[i].getCost() << std::endl;
                carrier_rank.push_back(std::make_pair(i, carriers[i].getCost()));
            }
        }

        std::sort(carrier_rank.begin(), carrier_rank.end(),
                [](const VLPair& a, const VLPair& b){
                return a.second < b.second;
                });

        // Decide what views to dispatch
        const int transfer_from = carrier_rank[0].first;
        const int transfer_to = carrier_rank[carrier_rank.size() - 1].first;
        std::vector<VLPair_kitem> transfer = dispatch(carriers[transfer_from], carriers[transfer_to]);

        std::cout << "Transfer from " << transfer_from << " to " << transfer_to << ":";
        for(auto& x : transfer){
            std::cout << '(' << x.data.second << ',' << x.data.first << ')';
        }
        std::cout << std::endl;

        // Calculate served ranges
        std::vector<int> transfer_view;
        vmag::VLMap one_round_result;
        for(auto& v : transfer){
            transfer_view.push_back(v.data.first);
            one_round_result[v.data.first] = v.data.second;
        }

        // result views should be added to served ranges
        for(auto& c : result){
            for(auto& v : c.second){
                if(std::find(transfer_view.begin(), transfer_view.end(), v.first) == transfer_view.end()){
                    transfer_view.push_back(v.first);
                }
            }
        }

        std::cout << "Transfer views: ";
        for(auto& x : transfer_view){
            std::cout << x << ' ';
        }
        std::cout << std:: endl;

        vmag::Level served = vmag::Level::get_served(transfer_view);
        std::cout << "Served ranges: " << served << std::endl;

        result[transfer_to] = one_round_result;

        // Clear users in served ranges
        for(int c = 0; c < CARRIER_NUM; ++c){
            if(result.find(c) != result.end()) continue;
            for(auto& x : served){
                for(int i = x.first; i <= x.second; ++i){
                    for(int j = 0; j < MCS_LEVEL; ++j){
                        carriers[c].user[j][i] = 0;
                    }
                    carriers[c].userView[i] = 0;
                }
            }

            std::cout << "===== Carrier " << c << " =====" << std::endl;
            vmag::printUser(carriers[c].user);

            carriers[c].VMAG();
        }

        return transfer_to;
    }

    void vmagc::VMAGC(){
        result.clear();
        for(; result.size() != CARRIER_NUM - 1;){
            int ret = one_round();
            if(ret == -1){
                // Cannot be achieved;
                std::cout << "Cannot be achieved" << std::endl;
                return;
            }
        }
        for(int i = 0; i < CARRIER_NUM; ++i){
            if(result.find(i) == result.end()){
                result[i] = carriers[i].resultViewLevel;
            }
        }

        for(auto& x : result){
            for(auto& y : x.second){
                if (y.second == MCS_LEVEL) continue;
                std::cout << x.first << '(' << y.second << ',' << y.first << ')' << ' ';
            }
            std::cout << std::endl;
        }
    }
}
