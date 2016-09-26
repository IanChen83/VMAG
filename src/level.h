#ifndef VMAG_LEVEL_H
#define VMAG_LEVEL_H

#include "range.h"

namespace vmag {
    typedef std::vector<Range>::iterator RangeIter;
    typedef std::vector<Range>::const_iterator ConstRangeIter;
    typedef std::pair<RangeIter, RangeIter> RangeIterPair;

    typedef std::function<bool(Range &)> RangeFilter;
    typedef std::function<Range(Range &)> RangeTransform;

    struct Level {
        Level() = default;

        Level(const Level &) = default;

        void addRange(const int, const int);

        void addRange(const Range &);

        void deleteRange(const int, const int);

        bool isInRange(const int);

        void sort();

        Level filter(RangeFilter);

        Range filterOne(RangeFilter);

        Level getCoverRanges(const int, const int);

        Level getCoverRanges(const Range &range);

        void reset();

        size_t size();

        RangeIter begin();

        RangeIter end();

        std::vector<Range> level;

        static Level get_served(std::vector<int>);
    };
}

#endif //VMAG_LEVEL_H
