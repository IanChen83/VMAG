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
CostTable cost;
UserTable user = {0};
UserViewTable userView = {0};

#define ForEachLevel(i) for(int (i) = 0; (i) < MCS_LEVEL; ++ (i))
#define ForEachView(i) for(int (i) = 0; (i) < MCS_VIEW; ++ (i))
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
    for(int i = 0; i < MCS_LEVEL; ++i){
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
 * Return QoE factor, assuming 0 <= k < j < MCS_VIEW
 * */
int QOE(const int k, const int j){
    if(j < k+2)
        return 0;
    return ALPHA * (j - k) * accumulate(&userView[k+1], &userView[j-1], 0);
}

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
    For(0, MCS_VIEW, i){
        For(0, MCS_LEVEL, j){
            if(user[j][i] != 0){
                For(j, MCS_LEVEL, k)
                    user[k][i] |= 1;
                break;
            }
        }
    }

    // 2.Horizontal Propagation
    int temp = 0;
    For(0, MCS_LEVEL, i){
        For(0, MCS_VIEW, j){
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
}
/*
 * 'No Aggregation' sub-algorithm
 * */
void NAggregation(const Range& range, const int l, const int j){    // range, level, view
    // TODO: API change
    int m = INT_MAX, temp;
    CostBlock* block;
    for(int k = (j - R >= range.first) ? (j - R): range.first; k < j; ++k){
        if((temp = min(m, RBT[l][j] + cost[l][k].cost + QOE(k, j) )) != m){
            m = temp;
            block = &(cost[l][k]);
        }
    }
    cost[l][j].cost = m;
    cost[l][j].prev = block;
}

/*
 * 'Vertical Aggregation' sub-algorithm
 * */
void VAggregation(const Range& range1, const Range& range2, const int level, const int view){
}

/*
 * 'Vertical and Horizontal Aggregation' sub-algorithm
 * */
void VHAggregation(const RangeIter& begin, const RangeIter& end, const int level, const int view){
}


/*
 * VMAG Algorithm
 * */
void VMAG(){

    InitializationPhase();

    For(0, MCS_VIEW, i){
        auto begin = RI.begin(i), end = RI.end(i);
        For(begin, end, it){
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
