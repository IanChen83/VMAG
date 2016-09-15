#ifndef VMAG_VMAG_H
#define VMAG_VMAG_H

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

#include "vmag_config.h"
#include "cost.h"
#include "range.h"
#include "level.h"
#include <map>

#define For(begin, end, it) for(auto (it) = (begin); (it) != (end); ++(it))
#define ForRev(end, begin, it) for(auto (it) = (end); (it) != (begin), --(it))

#define ForEachLevel(i) For(0, MCS_LEVEL, (i))
#define ForEachView(i) For(0, MCS_VIEW, (i))

/**
 * Using VMAG
 *
 * 1. Load RBT table, user table, and userView table
 * 2. call VMAG function
 * 3. result (represent in (level, view) pairs) will be stored in resultLevelView
 * 4. optionally call reset to reset intermediate data structure, but RBT table, user
 *    table and userView table won't be reset.
 */
namespace vmag {
    typedef int UserTable[MCS_LEVEL][MCS_VIEW];
    typedef int UserViewTable[MCS_VIEW];
    typedef CostBlock Cost[MCS_LEVEL + 1][MCS_VIEW][MCS_VIEW];
    typedef Level LevelTable[MCS_LEVEL + 1];
    typedef int ResourceBlockTable[MCS_LEVEL + 1][MCS_VIEW];
    typedef std::map<int, int> LVPair;

    class vmag {
    private:
        int _next_one_distance(const int, const int);

        int QOE(const int, const int);

    public:
        void InitializationPhase();

        void NAggregation(const int, const Range &, const Range &, int);

        void VAggregation(const int, const Range &, const Range &, const Range &);

        void VHAggregation(const int, const Range &, const Range &, const Range &);

        void Finalize();

        void reset();

        void VMAG();

        LVPair getCostTrace(Cost&, int, int, int);
        void printCostTrace(Cost&, int, int, int);


        Cost cost;
        LevelTable LT;
        UserTable user = {0};
        UserViewTable userView = {0};
        ResourceBlockTable RBT = {0};
        LVPair resultLevelView;
    };
}

#endif //VMAG_VMAG_H
