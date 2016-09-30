#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>
#include "util.h"

using namespace std;

namespace vmag{
    void printRBT(ResourceBlockTable& rbt){
        ForEachLevel(i){
            ForEachView(j){
                cout << rbt[i][j] << ' ';
            }
            cout << endl;
        }
    }

    void printUser(UserTable& u){
        cout << "--------------------------------------------------------------" << endl;
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

    void printRange(LevelTable & lt){
        ForEachLevel(i){
            cout << i << " : ";
            for(auto& it: lt[i]){
                printf("[%d %d] ", it.first, it.second);
            }
            cout << endl;
        }
    }

    void printCost(Cost& ct){
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



    /*
 * (*.rbt) file that fits our setting should have MCS_LEVEL * MCS_VIEW lines,
 * while each line contains one number
 * */
    bool parseRBT(const string rbtPath, ResourceBlockTable& RBT){
        ifstream is(rbtPath);
        if(!is.is_open()){
            printf("File IO error: %s\n", rbtPath);
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
    bool parseUser(const string rangePath, UserTable& user, UserViewTable& userView){
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
}

namespace vmagc{
    bool parseUser(const string userPath, vmagc& vm){
        assert(vm.carriers.size() == CARRIER_NUM);
        ifstream is(userPath);
        if(!is.is_open()){
            printf("File IO error: %s\n", userPath.c_str());
            return false;
        }
        string line;
        int level[CARRIER_NUM], view;
        getline(is, line);  // Drop the first line
        while(getline(is, line)){
            stringstream ss;
            ss << line;
            ss >> view;
            for(int i = 0; i < CARRIER_NUM; ++i)
                ss >> level[i];
            if(ss.fail()){
                printf("Error: Parsing range line failed: %s\n", line.c_str());
            }else{
                assert(0 <= view && view < MCS_VIEW);
                for(int i = 0; i < CARRIER_NUM; ++i){
                    if(level[i] == -1) continue;

                    assert(0 <= level[i] && level[i] < MCS_LEVEL);
                    vm.carriers[i].user[level[i]][view] = 1;
                    ++vm.carriers[i].userView[view];
                }
            }
        }

        is.close();
        return true;
    }

    bool parseRBT(const string rbtPath, vmagc& vm){
        ifstream is(rbtPath);
        if(!is.is_open()){
            printf("File IO error: %s\n", rbtPath.c_str());
            return false;
        }
        int level = 0, view = 0, count = 0, num = 0;
        string line;
        while(getline(is, line)){
            num = stoi(line);
            for(auto& c : vm.carriers){
                c.RBT[level][view] = num;
            }
            ++view;
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
        for(auto& c : vm.carriers){
            ForEachView(i){
                c.RBT[MCS_LEVEL][i] = 0;
            }
        }
        return true;
    }
}

