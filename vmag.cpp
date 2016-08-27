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
    cout << setw(2) << 'x';
    ForEachView(i){
        cout << setw(3) << i;
    }
    cout << endl;
    ForEachLevel(i){
        cout << setw(2) << i;
        ForEachView(j){
            cout <<setw(3) << (u[i][j]);
        }
        cout << endl;
    }
}

void printRange(RangeIndicator& ri){
    ForEachLevel(i){
        cout << i << " : ";
        for(auto& it: ri.get(i)){
            printf("[%d %d] ", it.first, it.second);
        }
        cout << endl;
    }
}

void printCost(CostTable ct[]){
    ForEachLevel(l){
        cout << "-------------------- LEVEL " << l << "--------------------" << endl;
        cout << setw(2) << 'x';
        ForEachView(i){
            cout << setw(5) << i;
        }
        cout << endl;
        ForEachView(i){
            cout << setw(2) << i;
            ForEachView(j){
                if(ct[l][i][j].cost == INT_MAX)
                    cout << setw(5) << "MAX";
                else
                    cout << setw(5) << ct[l][i][j].cost;
            }
            cout << endl;
        }

        cout << "-----------------------------------------------" << endl;
    }

    cout << "-------------------- VIRTUAL --------------------" << endl;
    cout << setw(2) << 'x';
    ForEachView(i){
        cout << setw(5) << i;
    }
    cout << endl;
    ForEachView(i){
        cout << setw(2) << i;
        ForEachView(j){
            if(ct[MCS_LEVEL][i][j].cost == INT_MAX)
                cout << setw(5) << "MAX";
            else
                cout << setw(5) << ct[MCS_LEVEL][i][j].cost;
        }
        cout << endl;
    }

    cout << "-----------------------------------------------" << endl;


    cout << setw(0);
}

void printCostTrace(CostTable ct[], int level, int from, int to){
    auto trace = getCostTrace(ct, level, from, to);
    for(auto t: trace){
        if(t.second == MCS_LEVEL) continue;
        cout << '(' << t.first << ',' << t.second << ')' << ' ';
    }
    cout << endl;
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

    ForEachView(i){
        RBT[MCS_LEVEL][i] = 0;
    }
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

unsigned getLevelNumber(CostBlock* cb){
    return (cb-(CostBlock*)cost)/MCS_VIEW;
}

unsigned getViewNumber(CostBlock* cb){

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
    // Virtual level
    RI.addRange(MCS_LEVEL, 0, MCS_VIEW - 1);

    //4. CostTable initialization
    ForEachView(i){
        int top = -1;
        ForEachLevel(l){
            if(top == -1){
                cost[l][i][i].cost = RBT[l][i];
                if(user[l][i] == 1){
// TODO                    top = RBT[l][i];
                }
            }else{
                cost[l][i][i].cost = top;
            }
        }
    }
    ForEachView(i){
        cost[MCS_LEVEL][i][i].cost = 0;
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
void NAggregation(const int l, const Range& lower, const Range& upper, int start = -1){
    int temp, m;
    int prev = -1;

    For(lower.first, lower.second + 1, i){
        For(i, upper.second + 1, j){
            temp = m = INT_MAX;
            For(max(j - R, i), j, k){
                if(k < start) continue;
                assert(cost[l][i][k].cost != INT_MAX);
                if((temp = min(cost[l][i][k].cost + RBT[l][j]/* + QOE(k, j)*/, m)) != m){
                    m = temp;
                    prev = k;
                }
            }
            if(m < cost[l][i][j].cost){
                cost[l][i][j].cost = m;
                cost[l][i][j].prev = prev;
                cost[l][i][j].method = 'N';
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
    assert(l > 0);
    int temp, m;
    int prev = -1;


    For(range1.first, range1.second + 1, h){
        For(end.first, end.second + 1, j){
            temp = m = INT_MAX;
            For(max(h, begin.first), begin.second + 1, i){
                assert(cost[l-1][i][j].cost != INT_MAX);
                assert(cost[l][h][i].cost != INT_MAX);
                if((temp = min(cost[l-1][i][j].cost + cost[l][h][i].cost - RBT[l][i], m))
                        != m){
                    m = temp;
                    prev = i;
                }
            }
            if(m < cost[l][h][j].cost){
                cost[l][h][j].cost = m;
                cost[l][h][j].prev = prev;
                cost[l][h][j].method = 'V';
            }
        }
    }
    /*
     * Before return, cost[l][h:i][u] should be update to use cost[l-1][h:i][u]
     * We make an assumption that every path from [h:i] to u only use one view in [m:u] in l-1 level,
     * because it can be transmitted in l level and reduce resource blocks
     * */
    For(range1.first, range1.second + 1, h){
        cost[l][h][begin.second].cost = cost[l][h][begin.second].cost - cost[l][begin.second][begin.second].cost
            + cost[l-1][begin.second][begin.second].cost;
    }

}

/*
 * 'Vertical and Horizontal Aggregation' sub-algorithm (formula 4)
 *
 *                                   f  a  b  g
 *                                   ***||||***
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
    assert(l > 0);
    int temp, m;
    int prev = -1;

    For(begin.first, begin.second + 1, h){
        For(end.first, end.second + 1, j){
            temp = m = INT_MAX;
            For(mid.first, mid.second + 1, o){
                assert(cost[l][h][o].cost != INT_MAX);
                assert(cost[l-1][o][j].cost != INT_MAX);
                if((temp = min(cost[l][h][cost[l][h][o].prev].cost + cost[l-1][o][j].cost, m)) != m){
                    // Using the cost from its prev is equivalent to reducing the cost of itself
                    m = temp;
                    prev = o;
                }
            }
            if(m < cost[l][h][j].cost){
                cost[l][h][j].cost = m;
                cost[l][h][j].prev = prev;
                cost[l][h][j].method = 'C'; // combined
            }
        }
    }
}

void Finalize(){
    printCostTrace(cost, MCS_LEVEL, 0, MCS_VIEW - 1);
}

/*
 * VMAG Algorithm
 * */
void VMAG(){

    InitializationPhase();

    for(auto& it: RI.get(0)){
        auto x = expandRangeDup(it, R-1);
        NAggregation(0, Range(x.first, it.first), Range(it.second, x.second));
    }

    For(1, MCS_LEVEL + 1, i){
        for(auto& it: RI.get(i)){
            auto x = expandRangeDup(it, R-1);
            auto covered = RI.getCoverRanges(i - 1, it);

            // With start number, we can safely call NA without referencing cost[l][h][h:start]
            int start = -1;

            if(covered.size() == 0){
                // No VA, and VHA is NOT possible
                NAggregation(i, Range(x.first, it.first), Range(it.second, x.second), start);
                continue;
            }else{

                auto temp = NULL_RANGE;
                for(auto& cit: covered){
                    auto cx = expandRangeDup(cit, R-1);
                    NAggregation(i, Range(x.first, it.first), Range(cx.first, cit.first), start);
                    VAggregation(i, Range(x.first, it.first), Range(cx.first, cit.first), Range(cit.second, cx.second));
                    if(temp != NULL_RANGE){
                        auto over = getOverlapped(expandRangeDup(temp, R-1), expandRangeDup(cit, R-1));
                        if(over != NULL_RANGE){
                            VHAggregation(i, Range(x.first, it.first), over, Range(cit.second, cx.second));
                        }
                    }
                    start = cit.second;
                    temp = cit;
                }

                NAggregation(i, Range(x.first, it.first), Range(it.second, x.second), start);
            }

        }
    }

    Finalize();

}

