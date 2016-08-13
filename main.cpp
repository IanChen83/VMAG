#include "main.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <sstream>
#include <climits>
#include <numeric>

using namespace std;

ResourceBlockTable RBT = {0};
RangeIndicator RI;
CostTable cost[MCS_LEVEL];
UserTable user = {0};
UserViewTable userView = {0};

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
            cout << u[i][j] << ' ';
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

    int level = MCS_LEVEL - 1, view = 0, count = 0;
    string line;
    while(getline(is, line)){
        RBT[level][view++] = stoi(line);
        ++count;
        if(view == MCS_VIEW){
            view = 0;
            --level;
        }
        if(level < 0){
            // printf("Warning: File not ends but RBT all filled\n");
            break;
        }
    }

    if(count != MCS_LEVEL * MCS_VIEW){
        printf("Warning: Expect %d lines, only %d lines read\n", MCS_LEVEL * MCS_VIEW, count);
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
                For(j, MCS_LEVEL, k)
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
}

/*
 * Return QoE factor, assuming 0 <= k < j < MCS_VIEW
 * */
int QOE(const int k, const int j){
    if(j < k+2)
        return 0;
    return ALPHA * (j - k) * accumulate(&userView[k+1], &userView[j-1], 0);
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
    int temp = INT_MAX, m = temp;
    CostBlock* prev = NULL;

    For(lower.first, lower.second + 1, i){
        For(i, upper.second + 1, j){
            temp = INT_MAX;
            For(max(j - R, upper.first), j, k){
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
 * Calling VAggregation(level, [h:i], [m:u], [v:n]) needs cost[level-1][m:u][v:n]
 * pre-defined and cost[level][h:i][m:u] pre-defined
 *
 * Note that calling NAggregation(level, [h:i], [v:t]) would be necessary (explained in
 * formula 3)
 *
 * */
void VAggregation(const int l, const Range& range1, const Range& begin, const Range& end){
    int temp = INT_MAX, m = temp;
    CostBlock* prev = NULL;

    For(range1.first, range1.second + 1, h){
        For(end.first, end.second + 1, j){
            temp = INT_MAX;
            For(begin.first, begin.second + 1, i){
                if((temp = min(cost[l-1][i][j].cost + cost[l][h][i].cost - rbt[l][i],m)) != m){
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
 * */
void VHAggregation(const int level,const Range& range, const Range& range1, const Range& range2){
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
            NAggregation(0, Range(x.first, it->first), Range(it->second, x.second));

            auto covered = RI.getCoveredRanges(i - 1, *it);
            VAggregation(i, *it, covered.first, covered.second);

            for(auto cit = covered.first; cit != covered.second; ++cit){
                if(isOverlapped(*cit, *(cit+1))){
                    VHAggregation(i, *it, *cit, *(cit+1));
                }
            }
        }
    }

}
/******************************************************************************
 *                                    Main                                    *
 *****************************************************************************/
int main(int argc, char* argv[]){
    if(argc != 4){
        printf("Usage: %s <rbt> <range> <output>\n", argv[0]);
        return 1;
    }

    parseRBT(argv[1]);
    parseUser(argv[2]);

    VMAG();

    //TODO: Output result

    return 0;
}
