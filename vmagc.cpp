#include "vmag.h"
#include "vmagc.h"
#include "knapsack.h"
#include <cassert>
#include <tuple>

namespace vmagc{
    int vmagc::get_pattern(int al, int ar, int bl, int br, int c){
        int lpattern, rpattern, y;
        y = (c == -1) ? PATTERN_UNDER : PATTERN_NONE;

        lpattern = (al != -1 && c - al <= R) ? PATTERN_INNER : PATTERN_NONE;
        if(bl != -1){
            if(al == bl){
                lpattern = PATTERN_UNDER;
            }else if(al < bl){
                lpattern = PATTERN_NONE;
            }
        }
        rpattern = (ar != -1 && ar - c <= R) ? PATTERN_INNER : PATTERN_NONE;
        if(br != -1){
            if(ar == br){
                rpattern = PATTERN_UNDER;
            }else if(ar > br){
                lpattern = PATTERN_NONE;
            }
        }

        return PATTERN_CODE[lpattern][rpattern][y];
    }

    kitem vmagc::set_kitem(vmag::vmag& a, vmag::vmag& b, VLPair i){
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

        int code = get_pattern(al.second, ar.second, bl.second, br.second, y.second);

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
        return kitem(cost, value);
    }

    void vmagc::dispatch(vmag::vmag& a, vmag::vmag& b){
        vmag::UserViewTable uv = {0};

        knapsack::knapsack<kitem> k;
        k.maxCost = VMAGC_LIMIT;
        // Identify the dispatch value of each view
        for(auto& i : b.resultViewLevel){
            auto item = set_kitem(a, b, i);
            k.items.push_back(item);
        }

        k.run();
    }
}
