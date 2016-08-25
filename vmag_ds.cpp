#include "vmag_ds.hpp"

bool isOverlapped(const Range& a, const Range& b){
    return !((a.second < b.first) || (b.second < a.first));
}

bool isCovered(const Range& a, const Range& b){
    return (a.first <= b.first && a.second >= b.second);
}

void expandRange(Range& x, int r){
    assert(r >= 0);
    x.first = (x.first - r > 0) ? x.first - r : 0;
    x.second = (x.second + r < MCS_VIEW) ? x.second + r : MCS_VIEW - 1;
}

Range expandRangeDup(const Range& x, int r){
    auto y = x;
    expandRange(y, r);
    return y;
}

