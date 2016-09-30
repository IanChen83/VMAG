#include "vmag_config.h"
#include "level.h"
#include <algorithm>
#include <cassert>
#include <ostream>
#include <iostream>

namespace vmag {
    void Level::addRange(const int lower, const int upper) {
        assert(lower >= 0 && lower < MCS_VIEW);
        assert(upper >= 0 && upper < MCS_VIEW);
        assert(lower <= upper);
        assert(!isInRange(lower) && !isInRange(upper));
        level.emplace_back(lower, upper);
        sort();
    }

    void Level::addRange(const Range &range) {
        addRange(range.first, range.second);
    }

    void Level::deleteRange(const int lower, const int upper) {
        level.erase(
                std::remove_if(level.begin(), level.end(),
                               [lower, upper](const Range &x) {
                                   return (x.first == lower && x.second == upper);
                               })
        );
    }

    bool Level::isInRange(const int view) {
        for (auto &r: level)
            if (view >= r.first && view <= r.second)
                return true;

        return false;
    }

    void Level::sort() {
        std::sort(level.begin(), level.end(),
                  [](Range &a, Range &b) { return a.first < b.first; });
    }

    Level Level::filter(RangeFilter rf) {
        Level ret;
        for (auto &it: level) {
            if (rf(it)) {
                ret.addRange(it);
            }
        }
        return ret;
    }

    Range Level::filterOne(RangeFilter rf) {
        for (auto &it: level) {
            if (rf(it)) {
                return it;
            }
        }

        return NULL_RANGE;
    }

    Level Level::getCoverRanges(const Range &range) {
        return filter([range](Range &r) { return isCover(range, r); });
    }

    Level Level::getCoverRanges(const int a, const int b) {
        return getCoverRanges(Range(a, b));
    }

    void Level::reset() {
        level.clear();
    }

    size_t Level::size() {
        return level.size();
    }

    RangeIter Level::begin() {
        return level.begin();
    }

    RangeIter Level::end() {
        return level.end();
    }

    Level Level::get_served(std::vector<int> views){
        std::sort(views.begin(), views.end());
        Level ret;
        Range x = NULL_RANGE;
        for(int v : views){
            if(x == NULL_RANGE){
                x.first = v;
                x.second = v;
            }else{
                if(v < x.first && x.first - v <= R){
                    x.first = v;
                }else if(v > x.second && v - x.second <= R){
                    x.second = v;
                }else{
                    ret.addRange(x);
                    x.first = v;
                    x.second = v;
                }
            }
        }
        if(x != NULL_RANGE){
            ret.addRange(x);
        }
        return ret;
    }

    std::ostream& operator<<(std::ostream& os,const Level& level){
        for(auto& x : level.level){
            os << x;
        }
        return os;
    }
}
