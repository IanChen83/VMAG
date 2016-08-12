#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <vector>
#include <array>
#include <algorithm>
#include <utility>
#include <cassert>

/********************************************************************
 *                 Data structure of VMAG algorithm                 *
 ********************************************************************
  1. Resource Block Table
     A 2D array denoted RBT(level, view) that storing the cost to
     transmit each view in which
        level   : The number of MCS level, 0 denotes the most costy
                  one.
        view    : Slot number

  2. Range Indicator
     Each range store the information of consecutive views with the
     maximum length.

 *******************************************************************/

#define MCS_LEVEL   16
#define MCS_VIEW    16

typedef std::pair<int, int>                 Range;
typedef std::vector<Range>::iterator        RangeIter;
typedef std::vector<Range>::const_iterator  ConstRangeIter;
typedef std::pair<RangeIter, RangeIter>     RangeIterPair;

#define ForAllRange(v, it) for(auto (it) = (v).begin(); (it) != (v).end(); ++(it))
#define ForAllLevel(a, it) for(auto (it) = (a).begin(); (it) != (a).end(); ++(it))
#define ForEachRange(begin, end, it) for(auto (it) = (begin); (it) != (end); ++(it))
#define ForEachLevel(begin, end, it) for(auto (it) = (begin); (it) != (end); ++(it))

struct ResourceBlockTable: std::array<int, MCS_LEVEL * MCS_VIEW>{
    int& operator()(const int a, const int b){
        return at(a * b);
    }
};

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
        RangeIter getIteratorBegin(const int level){ return views[level].begin(); }
        RangeIter getIteratorEnd(const int level){ return views[level]. end();}

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

ResourceBlockTable RBT;
RangeIndicator RI;


#endif  // MAIN_HEADER
