#include <cassert>
#include "range.h"
#include "vmag.h"

namespace vmag {
    Range::Range(int a, int b) : std::pair<int, int>(a, b) {}

    void Range::expand(int r) {
        assert(r >= 0);
        first = (first - r > 0) ? first - r : 0;
        second = second + r < MCS_VIEW ? second + r : MCS_VIEW - 1;
    }

    Range Range::expandDup(int r) {
        auto y = Range(first, second);
        y.expand(r);
        return y;
    }

    bool isOverlapped(const Range &a, const Range &b) {
        return !((a.second < b.first) || (b.second < a.first));
    }

    bool isCover(const Range &a, const Range &b) {
        return (a.first <= b.first && a.second >= b.second);
    }

    Range getOverlapped(const Range &a, const Range &b) {
        if (a.first > b.first) return getOverlapped(b, a);
        if (b.first > a.second) return NULL_RANGE;
        return Range(b.first, (a.second > b.second) ? b.second : a.second);
    }

    std::ostream& operator<<(std::ostream& os,const Range& range){
        os << '[' << range.first << ':' << range.second << ']';
        return os;
    }
}
