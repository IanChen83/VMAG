#include "vmag.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <sstream>
#include <climits>
#include <numeric>
#include <iomanip>


using namespace std;

/******************************************************************************
 *                               Print Method                                 *
 *****************************************************************************/
void printRBT(ResourceBlockTable& rbt){
    ForEachLevel(i){
        ForEachView(j){
            cout << rbt[i][j] << ' ';
        }
        cout << endl;
    }
}

void printUser(UserTable u){
    ForEachLevel(i){
        ForEachView(j){
            cout << (u[i][j]) << ' ';
        }
        cout << endl;
    }
}

void printRange(RangeIndicator& ri){
    ForEachLevel(i){
        cout << i << " : ";
        For(ri.begin(i), ri.end(i), it){
            printf("[%d %d] ", it->first, it->second);
        }
        cout << endl;
    }
}

void printCost(CostTable ct[]){
    ForEachLevel(l){
        ForEachView(i){
            ForEachView(j){

                if(ct[l][i][j].cost == INT_MAX)
                    cout << setw(5) << "MAX";
                else
                    cout << setw(5) << ct[l][i][j].cost;
            }
            cout << endl;
        }

        cout << "---------------------------------------------------------------------\n";
    }

    cout << setw(0);
}

ostream& operator<<(ostream& os,const Range& range){
    os << '[' << range.first << ':' << range.second << ']';
    return os;
}
/******************************************************************************
 *                               Parse Method                                 *
 *****************************************************************************/
/*
 * (*.rbt) file that fits our setting should have MCS_LEVEL * MCS_VIEW lines,
 * while each line contains one number
 * */
bool parseRBT(const string rbtPath){
    ifstream is(rbtPath);
    if(!is.is_open()){
        printf("File IO error: %s\n", rbtPath.c_str());
        return false;
    }

    int level = 0, view = 0, count = 0;
    string line;
    while(getline(is, line)){
        RBT[level][view++] = stoi(line);
        ++count;
        if(view == MCS_VIEW){
            view = 0;
            ++level;
        }
        if(level == MCS_LEVEL){
            // printf("Warning: File not ends but RBT all filled\n");
            break;
        }
    }

    if(count != MCS_LEVEL * MCS_VIEW){
        printf("Warning: Expect %d lines, only %d lines read\n",
                MCS_LEVEL * MCS_VIEW, count);
    }

    is.close();
    return true;
}

/*
 * (*.user) file should have variant number of lines, while the first line is the number
 * of users, and all of the following line contain 2 numbers delimeted by one white-space
 * character
 * [view] [level]
 * */
bool parseUser(const string rangePath){
    ifstream is(rangePath);
    if(!is.is_open()){
        printf("File IO error: %s\n", rangePath.c_str());
        return false;
    }
    string line;
    int level, view;
    getline(is, line);  // Drop the first line
    while(getline(is, line)){
        stringstream ss;
        ss << line;
        ss >> level >> view;
        if(ss.fail()){
            printf("Error: Parsing range line failed: %s\n", line.c_str());
        }else{
            assert(0 <= level && level < MCS_LEVEL);
            assert(0 <= view && view < MCS_VIEW);
            user[level][view] = 1;
            ++userView[view];
        }
    }

    is.close();
    return true;
}
/******************************************************************************
 *                              Algorithm Method                              *
 *****************************************************************************/

/*
 * 'Initialization Phase' sub-algorithm
 * */

int _next_one_distance(const int level, const int view){
    assert(user[level][view] == 1);
    For(view + 1, MCS_VIEW, i){
        if(user[level][i] != 0)
            return i - view;
    }
    return MCS_VIEW;
}

void InitializationPhase(){
    // 1. Vertical Propagation
    ForEachView(i){
        ForEachLevel(j){
            if(user[j][i] != 0){
                For(j+1, MCS_LEVEL, k)
                    user[k][i] |= 1;
                break;
            }
        }
    }

    // 2.Horizontal Propagation
    int temp = 0;
    ForEachLevel(i){
        ForEachView(j){
            if(user[i][j] == 1){
                if((temp = _next_one_distance(i, j)) <= R){
                    For(1, temp, k){
                        user[i][k + j] |= 1;
                    }
                    j += temp - 1;
                }
            }
        }
    }

    // 3. Mark Ranges
    bool s = false;
    int begin = 0, end = 0;
    ForEachLevel(i){
        s = false;
        ForEachView(j){
            if(!s){
                if(user[i][j] == 1){
                    s = true;
                    begin = j;
                }
            }else{
                if(user[i][j] == 0){
                    s = false;
                    end = j;
                    RI.addRange(i, begin, end - 1);
                }
            }
        }
        if(s){
            RI.addRange(i, begin, MCS_VIEW - 1);
            s = false;
        }
    }

    //4. CostTable initialization
    ForEachLevel(l){
        ForEachView(i){
            cost[l][i][i].cost = RBT[l][i];
        }
    }
}

/*
 * Return QoE factor, assuming 0 <= k < j < MCS_VIEW
 * */
int QOE(const int k, const int j){
    if(j < k+2)
        return 0;
    return ALPHA * (j - k) * accumulate(&userView[k+1], &userView[j], 0);
}

/*
 * 'No Aggregation' sub-algorithm (formula 1)
 *
 *                      h  i               j  t
 *                      ***|||||||||||||||||***
 *
 * Calling NAggregation(level, [h:i], [j:t]) needs cost[level][x][x] pre-defined for
 * x in range [h:i]
 *
 * */
void NAggregation(const int l, const Range& lower, const Range& upper){
    cout << "Run NoAggregation(" << l << ", " << lower << ", " << upper << ")\n";
    int temp, m;
    CostBlock* prev = NULL;

    For(lower.first, lower.second + 1, i){
        For(i, upper.second + 1, j){
            temp = m = INT_MAX;
            For(max(j - R, i), j, k){
                assert(cost[l][i][k].cost != INT_MAX);
                if((temp = min(cost[l][i][k].cost + RBT[l][j] + QOE(k, j), m)) != m){
                    m = temp;
                    prev = &(cost[l][i][k]);
                }
            }
            if(m < cost[l][i][j].cost){
                cost[l][i][j].cost = m;
                cost[l][i][j].prev = prev;
            }
        }
    }
}

/*
 * 'Vertical Aggregation' sub-algorithm (formula 2)
 *
 *                      m  u       v  n
 *                      ***|||||||||***
 *                  h  i                  j  t
 *                  ***||||||||||||||||||||***
 *
 * 'Vertical Aggregation' is specified by 3 ranges, [h:i], [m:u], and [v:n]
 * Calling VAggregation(level, [h:i], [m:u], [v:n]) requires cost[level-1][m:u][v:n]
 * and cost[level][h:i][m:u] pre-calculated
 *
 * Note that calling NAggregation(level, [h:i], [v:t]) would be necessary (explained in
 * formula 3)
 *
 * */
void VAggregation(const int l, const Range& range1, const Range& begin, const Range& end){
    cout << "Run VAggregation(" << l << ", " << range1 << ", " << begin << ", " << end << ")\n";
    assert(l > 0);
    int temp, m;
    CostBlock* prev = NULL;

    For(range1.first, range1.second + 1, h){
        For(end.first, end.second + 1, j){
            temp = m = INT_MAX;
            For(begin.first, begin.second + 1, i){
                assert(cost[l-1][i][j].cost != INT_MAX);
                assert(cost[l][h][i].cost != INT_MAX);
                if((temp = min(cost[l-1][i][j].cost + cost[l][h][i].cost - RBT[l][i], m))
                        != m){
                    m = temp;
                    prev = &(cost[l][h][i]);
                }
            }
            if(m < cost[l][h][j].cost){
                cost[l][h][j].cost = m;
                cost[l][h][j].prev = prev;
            }
        }
    }
}

/*
 * 'Vertical and Horizontal Aggregation' sub-algorithm (formula 4)
 *
 *                                   f  ab  g
 *                                   ***||***
 *                      m  u       v  n
 *                      ***|||||||||***
 *                  h  i                  j  t
 *                  ***||||||||||||||||||||***
 *
 * 'Vertical and Horizontal Aggregation' is specified by 3 ranges, [h:i], [f:n], and [b:g]
 * Calling VHAggregation(level, [h:i], [f:n], [b:g]) requires cost[l-1][m:u][f:n] and
 * cost[l-1][f:a][b:g] pre-calculated
 *
 * Note that calling VHA may not produce optimal result, so that calling VA and NA is still
 * needed (explained is formula 4)
 * */
void VHAggregation(const int l,const Range& begin, const Range& mid, const Range& end){
    cout << "Run VAggregation(" << l << ", " << begin << ", " << mid << ", " << end << ")\n";
    assert(l > 0);
    int temp, m;
    CostBlock* prev = NULL;

    For(begin.first, begin.second + 1, h){
        For(end.first, end.second + 1, j){
            temp = m = INT_MAX;
            For(mid.first, mid.second + 1, o){
                assert(cost[l][h][o].cost != INT_MAX);
                assert(cost[l-1][o][j].cost != INT_MAX);
                if((temp = min(cost[l][h][o].cost + cost[l-1][o][j].cost - RBT[l][o], m)) != m){
                    m = temp;
                    prev = &(cost[l][h][o]);
                }
            }
            if(m < cost[l][h][j].cost){
                cost[l][h][j].cost = m;
                cost[l][h][j].prev = prev;
            }
        }
    }
}


/*
 * VMAG Algorithm
 * */
void VMAG(){

    InitializationPhase();

    ForAllRange(RI.get(0), it){
        auto x = expandRangeDup(*it, R);
        NAggregation(0, Range(x.first, it->first), Range(it->second, x.second));
    }

    For(1, MCS_LEVEL, i){
        ForAllRange(RI.get(i), it){
            auto x = expandRangeDup(*it, R);

            auto covered = RI.getCoveredRanges(i - 1, *it);

            if(RI.isNullRangeIter(covered)){
                NAggregation(i, Range(x.first, it->first), Range(it->second, x.second));
                continue;
            }

            auto start = Range(x.first, it->first);

            For(covered.first, covered.second, cit){
                auto cx = expandRangeDup(*cit, R);
                NAggregation(i, start, Range(cx.first, cit->first));
                VAggregation(i, start, Range(cx.first, cit->first), Range(cit->second, cx.second));
                start.first = cit->second;
                start.second = cx.second;
            }

            NAggregation(i, start, Range(it->second, x.second));

            for(auto cit = covered.first; cit != covered.second; ++cit){
                if(isOverlapped(*cit, *(cit+1))){
                    // Call VHAggregation
                }
            }
        }
    }

}

