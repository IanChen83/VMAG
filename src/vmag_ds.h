#ifndef VMAG_DS_HEADER
#define VMAG_DS_HEADER

#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <climits>
#include <cassert>
#include <functional>

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

#define MCS_LEVEL   3
#define MCS_VIEW    16
#define R           3
#define ALPHA       1

typedef int ResourceBlockTable[MCS_LEVEL + 1][MCS_VIEW];
typedef int UserTable[MCS_LEVEL][MCS_VIEW];
typedef int UserViewTable[MCS_VIEW];

struct CostBlock{
    CostBlock(){
        cost = INT_MAX;
        prev = -1;
        method = '\0';
    }
    int         cost;
    int         prev;
    char        method;
};

typedef CostBlock CostTable[MCS_VIEW][MCS_VIEW];

#define For(begin, end, it) for(auto (it) = (begin); (it) != (end); ++(it))
#define ForRev(end, begin, it) for(auto (it) = (end); (it) != (begin), --(it))

#define ForEachLevel(i) For(0, MCS_LEVEL, (i))
#define ForEachView(i) For(0, MCS_VIEW, (i))


typedef std::pair<int, int>                 Range;
typedef std::vector<Range>::iterator        RangeIter;
typedef std::vector<Range>::const_iterator  ConstRangeIter;
typedef std::pair<RangeIter, RangeIter>     RangeIterPair;

typedef std::function<bool (Range&)>        RangeFilter;
typedef std::function<Range (Range&)>      RangeTransform;

#define NULL_RANGE (Range(-1, -1))

#define ForAllRange(v, it) for(auto& (it) : (v))

/*
 * Return true if a is overlapped with b
 * */
bool isOverlapped(const Range&, const Range&);

/*
 * Return true if a covers b
 * */
bool isCover(const Range&, const Range&);

/*
 * Expand the range, boundary-safe
 * */
void expandRange(Range&, int);
Range expandRangeDup(const Range&, int);

/*
 * Get Overlapped range of a and b
 * */
Range getOverlapped(const Range& a, const Range& b);

/*
 * Return trace of costblock
 * */
std::map< int, int> getCostTrace(CostTable[], int, int, int);

class RangeIndicator{
    public:
        /*
         * Add a Range into RangeIndicator
         * Note that there should be any overlapped range in RangeIndicator
         *
         * */
        void addRange(const int level, const int lower, const int upper){
            assert(level >= 0 && level <= MCS_LEVEL);
            assert(lower >= 0 && lower < MCS_VIEW);
            assert(upper >= 0 && upper < MCS_VIEW);
            assert(lower < upper);
            assert(!isInRange(level, lower) && !isInRange(level, upper));

            views[level].push_back(Range(lower, upper));
            sort(level);
        }

        /*
         * Delete a Range from RangeIndicator
         * There won't be any warning if nothing is erased
         *
         * */
        void deleteRange(const int level, const int lower, const int upper){
            views[level].erase(
                    std::remove_if(views[level].begin(), views[level].end(),
                        [lower, upper](const Range& x){
                        return (x.first == lower && x.second == upper);
                        })
                    );
        }

        /*
         * Specify a view position, return true if it falls into any range
         * */
        bool isInRange(const int& level, const int& view){
            for(auto& it: views[level]){
                if(view >= it.first && view <= it.second)
                    return true;
            }
            return false;
        }

        void sort(const int level){
            std::sort(views[level].begin(), views[level].end(),
                    [](Range& a, Range& b){ return a.first < b.first; });
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

        std::vector<Range>& get(const int level){
            return views[level];
        }
        std::vector<Range> get(const int level, RangeFilter rf){
            std::vector<Range> ret;
            for(auto& it: views[level]){
                if(rf(it)){
                    ret.push_back(it);
                }
            }
            return ret;
        }
        std::vector<Range> get(RangeFilter rf, std::vector<Range>& context){
            std::vector<Range> ret;
            for(auto& it: context){
                if(rf(it)){
                    ret.push_back(it);
                }
            }
            return ret;
        }
        Range getOne(const int level, RangeFilter rf){
            for(auto& it: views[level]){
                if(rf(it)){
                    return it;
                }
            }

            return NULL_RANGE;
        }

        Range getOne(RangeFilter rf, std::vector<Range>& context){
            for(auto& it: context){
                if(rf(it)){
                    return it;
                }
            }

            return NULL_RANGE;
        }

        /*
         * Return a vector of ranges which are the first and the end of elements that is
         * covered in their level
         * */
        std::vector<Range> getCoverRanges(const int level, const int lower, const int upper){
            return getCoverRanges(level, Range(lower, upper));
        }
        std::vector<Range> getCoverRanges(const int level, const Range& range){
            return get(level, [range](Range& r){ return isCover(range, r); });
        }

    private:
        std::vector<Range> views[MCS_LEVEL + 1];

};
#endif  // VMAG_DS_HEADER
