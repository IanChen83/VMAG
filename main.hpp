#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <vector>
#include <array>
#include <algorithm>
#include <utility>
#include <climits>
#include <cassert>

/********************************************************************
 *                 Data structure of VMAG algorithm                 *
 ********************************************************************
  1. Resource Block Table
     A 2D array denoted RBT(level, view) that storing the cost to
     transmit each view in which
        level   : The number of MCS level, 0 denotes the least costy
                  one.
        view    : Slot number

  2. Range Indicator
     Each range store the information of consecutive views with the
     maximum length.

  3. Cost Table
     For back tracking purpose, Cost Table store cost for transmitting
     view i to view j as well as previous selected view.
     Cost[i][j]

  4. User Table
     Storing the number of user in each view.

 *******************************************************************/

#define MCS_LEVEL   15
#define MCS_VIEW    16
#define R           3
#define ALPHA       1

typedef int ResourceBlockTable[MCS_LEVEL][MCS_VIEW];
typedef int UserTable[MCS_LEVEL][MCS_VIEW];
typedef int UserViewTable[MCS_VIEW];

struct CostBlock{
    CostBlock(){
        cost = INT_MAX;
        prev = this;
    }
    int         cost;
    CostBlock*  prev;
};

typedef CostBlock CostTable[MCS_VIEW][MCS_VIEW];

#define For(begin, end, it) for(auto (it) = (begin); (it) != (end); ++(it))
#define ForRev(end, begin, it) for(auto (it) = (end); (it) != (begin), --(it))

#define ForEachLevel(i) For(0, MCS_LEVEL, (i))
#define ForEachView(i) For(0, MCS_VIEW, (i))

void initializeCostTable(CostTable& ct, const ResourceBlockTable& rbt,  const int level){
    ForEachView(i){
        ct[i][i].cost = rbt[level][i];
    }
}

typedef std::pair<int, int>                 Range;
typedef std::vector<Range>::iterator        RangeIter;
typedef std::vector<Range>::const_iterator  ConstRangeIter;
typedef std::pair<RangeIter, RangeIter>     RangeIterPair;

typedef std::pair<int, int>                 LVPair;

#define ForAllRange(v, it) for(auto (it) = (v).begin(); (it) != (v).end(); ++(it))

class RangeIndicator{
    public:
        /*
         * Add a Range into RangeIndicator
         * Note that there should be any overlapped range in RangeIndicator
         *
         * */
        void addRange(const int level, const int lower, const int upper){
            assert(level >= 0 && level < MCS_LEVEL);
            assert(lower >= 0 && lower < MCS_VIEW);
            assert(upper >= 0 && upper < MCS_VIEW);
            assert(lower <= upper);
            assert(!isInRange(level, lower) && !isInRange(level, upper));

            views[level].push_back(std::make_pair(lower, upper));
            sort(level);
        }

        /*
         * Delete a Range from RangeIndicator
         * There won't be any warning if nothing is erased
         *
         * */
        void deleteRange(const int level, const int lower, const int upper){
            views[level].erase(
                    std::remove_if(views[level].begin(), views[level].end(),[lower, upper](const Range& x){
                        return (x.first == lower && x.second == upper);
                        })
                    );
        }

        /*
         * Specify a view position, return true if it falls into any range
         * */
        bool isInRange(const int& level, const int& view){
            ForAllRange(views[level], it){
                if(view >= it->first && view <= it->second)
                    return true;
            }
            return false;
        }

        void sort(const int level){
            std::sort(views[level].begin(), views[level].end(), [](Range& a, Range& b){ return a.first < b.first; });
        }

        void sortAll(){
            for(int i = 0; i < MCS_LEVEL; ++i)
                sort(i);
        }

        /*
         * Return iterator by specifying level number
         * */
        RangeIter begin(const int level){ return views[level].begin(); }
        RangeIter end(const int level){ return views[level].end(); }

        std::vector<Range>& get(const int level){ return views[level]; }

        /*
         * Return a pair of range iterator which are the first and the end of elements
         * that is covered in their level
         * Return end(), which can be accessed by calling getIteratorEnd(), if nothing is covered
         * */
        RangeIterPair getCoveredRanges(const int level, const int lower, const int upper){
            bool s = true;
            RangeIter a = views[level].end(), b = a;
            ForAllRange(views[level], it){
                if(s){
                    if(lower < it->first && it->second < upper){
                        a = it;
                        b = a;
                        s = false;
                    }
                }else{
                    if(lower < it->first && it->second < upper){
                        b = it;
                    }else{
                        break;
                    }
                }
            }
            return std::make_pair(a, b);
        }
        RangeIterPair getCoveredRanges(const int level, const Range& range){
            return getCoveredRanges(level, range.first, range.second);
        }

    private:
        std::vector<Range> views[MCS_LEVEL];

};

/*
 * Return true if a is overlapped with b
 * */
bool isOverlapped(const Range& a, const Range& b){
    return !((a.second < b.first) || (b.second < a.first));
}

/*
 * Return true if a covers b
 * */
bool isCovered(const Range& a, const Range& b){
    return (a.first <= b.first && a.second >= b.second);
}

/*
 * Expand the range, boundary-safe
 * */
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

#endif  // MAIN_HEADER
