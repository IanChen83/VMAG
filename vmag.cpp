#include "range.h"
#include "level.h"
#include "cost.h"
#include "vmag.h"
#include <cassert>
#include <numeric>
#include <stack>
#include <iostream>

using namespace std;

/*
 * 'Initialization Phase' sub-algorithm
 * */
namespace vmag {
    int vmag::_next_one_distance(const int level, const int view) {
        assert(user[level][view] == 1);
        For(view + 1, MCS_VIEW, i) {
            if (user[level][i] != 0)
                return i - view;
        }
        return MCS_VIEW;
    }

    void vmag::InitializationPhase() {
        // 1. Vertical Propagation
        ForEachView(i) {
            ForEachLevel(j) {
                if (user[j][i] != 0) {
                    For(j + 1, MCS_LEVEL, k)user[k][i] |= 1;
                    break;
                }
            }
        }

        // 2.Horizontal Propagation
        int temp = 0;
        ForEachLevel(i) {
            ForEachView(j) {
                if (user[i][j] == 1) {
                    if ((temp = _next_one_distance(i, j)) <= R) {
                        For(1, temp, k) {
                            user[i][k + j] |= 1;
                        }
                        j += temp - 1;
                    }
                }
            }
        }

        // 3. Mark Ranges
        bool s;
        int begin = 0, end = 0;
        ForEachLevel(i) {
            s = false;
            ForEachView(j) {
                if (!s) {
                    if (user[i][j] == 1) {
                        s = true;
                        begin = j;
                    }
                } else {
                    if (user[i][j] == 0) {
                        s = false;
                        end = j;
                        LT[i].addRange(begin, end - 1);
                    }
                }
            }
            if (s) {
                LT[i].addRange(begin, MCS_VIEW - 1);
                s = false;
            }
        }
        // Virtual level
        LT[MCS_LEVEL].addRange(0, MCS_VIEW - 1);

        //4. Cost initialization
        ForEachView(i) {
            ForEachLevel(l) {
                cost[l][i][i].cost = RBT[l][i];
            }
        }

        ForEachView(i) {
            cost[MCS_LEVEL][i][i].cost = 0;
        }
    }

/*
 * Return QoE factor, assuming 0 <= k < j < MCS_VIEW
 * */
    int vmag::QOE(const int k, const int j) {
        if (j < k + 2)
            return 0;
        return ALPHA * (j - k) * accumulate(&userView[k + 1], &userView[j], 0);
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
    void vmag::NAggregation(const int l, const Range &lower, const Range &upper, int start = -1) {
        int temp, m;
        int prev = -1;

        For(lower.first, lower.second + 1, i) {
            For(i, upper.second + 1, j) {
                temp = m = INT_MAX;
                For(max(j - R, i), j, k) {
                    if (k < start) continue;
                    assert(cost[l][i][k].cost != INT_MAX);
                    if ((temp = min(cost[l][i][k].cost + RBT[l][j]/* + QOE(k, j)*/, m)) != m) {
                        m = temp;
                        prev = k;
                    }
                }
                if (m < cost[l][i][j].cost) {
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
    void vmag::VAggregation(const int l, const Range &range1, const Range &begin, const Range &end) {
        assert(l > 0);
        int temp, m;
        int prev = -1;


        For(range1.first, range1.second + 1, h) {
            For(end.first, end.second + 1, j) {
                temp = m = INT_MAX;
                For(max(h, begin.first), begin.second + 1, i) {
                    assert(cost[l - 1][i][j].cost != INT_MAX);
                    assert(cost[l][h][i].cost != INT_MAX);
                    if ((temp = min(cost[l - 1][i][j].cost + cost[l][h][i].cost - RBT[l][i], m))
                        != m) {
                        m = temp;
                        prev = i;
                    }
                }
                if (m < cost[l][h][j].cost) {
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
        For(range1.first, range1.second + 1, h) {
            cost[l][h][begin.second].cost = cost[l][h][begin.second].cost - cost[l][begin.second][begin.second].cost
                                            + cost[l - 1][begin.second][begin.second].cost;
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
    void vmag::VHAggregation(const int l, const Range &begin, const Range &mid, const Range &end) {
        assert(l > 0);
        int temp, m;
        int prev = -1;

        For(begin.first, begin.second + 1, h) {
            For(end.first, end.second + 1, j) {
                temp = m = INT_MAX;
                For(mid.first, mid.second + 1, o) {
                    assert(cost[l][h][o].cost != INT_MAX);
                    assert(cost[l - 1][o][j].cost != INT_MAX);
                    if ((temp = min(cost[l][h][cost[l][h][o].prev].cost + cost[l - 1][o][j].cost, m)) != m) {
                        // Using the cost from its prev is equivalent to reducing the cost of itself
                        m = temp;
                        prev = o;
                    }
                }
                if (m < cost[l][h][j].cost) {
                    cost[l][h][j].cost = m;
                    cost[l][h][j].prev = prev;
                    cost[l][h][j].method = 'C'; // combined
                }
            }
        }
    }

    void vmag::Finalize() {
        resultViewLevel = getCostTrace(cost, MCS_LEVEL, 0, MCS_VIEW - 1);
    }

/*
 * Reset the vmag structure so be re-used.
 * */
    void vmag::reset() {
        // RI
        For(0, MCS_LEVEL + 1, i)LT[i].reset();

        // cost
        For(0, MCS_LEVEL + 1, i) {
            ForEachView(j) {
                ForEachView(k) {
                    cost[i][j][k].cost = INT_MAX;
                    cost[i][j][k].method = '\0';
                    cost[i][j][k].prev = -1;
                }
            }
        }

        resultViewLevel.clear();
    }

/*
 * VMAG Algorithm
 * */
    void vmag::VMAG() {

        InitializationPhase();

        for (auto &it: LT[0]) {
            auto x = it.expandDup(R - 1);
            NAggregation(0, Range(x.first, it.first), Range(it.second, x.second));
        }

        For(1, MCS_LEVEL + 1, i) {
            for (auto &it: LT[i]) {
                auto x = it.expandDup(R - 1);
                auto covered = LT[i - 1].getCoverRanges(it);

                // With start number, we can safely call NA without referencing cost[l][h][h:start]
                int start = -1;

                if (covered.size() == 0) {
                    // No VA, and VHA is NOT possible
                    NAggregation(i, Range(x.first, it.first), Range(it.second, x.second), start);
                    continue;
                } else {

                    auto temp = NULL_RANGE;
                    for (auto &cit: covered) {
                        auto cx = cit.expandDup(R - 1);
                        NAggregation(i, Range(x.first, it.first), Range(cx.first, cit.first), start);
                        VAggregation(i, Range(x.first, it.first), Range(cx.first, cit.first),
                                     Range(cit.second, cx.second));
                        if (temp != NULL_RANGE) {
                            auto over = getOverlapped(temp.expandDup(R - 1), cit.expandDup(R - 1));
                            if (over != NULL_RANGE) {
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

    std::map<int, int> vmag::getCostTrace(Cost &ct, int level, int from, int to) {
        typedef std::tuple<int, int, int> LFT;
        const int L = 0, F = 1, T = 2;

        std::map<int, int> ret;
        std::stack<LFT> s;
        CostBlock *c;
        int _l, _f, _t;

        s.push(std::make_tuple(level, from, to));
        while (!s.empty()) {
            std::tie(_l, _f, _t) = s.top();
            c = &ct[_l][_f][_t];

            if (_f == _t) {
                if (ret.count(_t) == 0) {
                    ret[_t] = _l;
                }
                s.pop();
            } else if (c->method == 'N') {
                std::get<T>(s.top()) = c->prev;
                if (ret.count(_t) == 0) {
                    ret[_t] = _l;
                }
                continue;
            } else if (c->method == 'V') {
                std::get<T>(s.top()) = c->prev;
                s.push(std::make_tuple(_l - 1, c->prev, _t));
            } else if (c->method == 'C') {
                std::get<T>(s.top()) = c->prev;
                s.push(std::make_tuple(_l - 1, c->prev, _t));
            }
        }
        if (ret.count(_t) == 0) {
            ret[_t] = _l;
        }

        return ret;
    }


    void vmag::printCostTrace(Cost &ct, int level, int from, int to) {
        auto trace = getCostTrace(ct, level, from, to);
        for (auto t: trace) {
            if (t.second == MCS_LEVEL) continue;
            cout << '(' << t.first << ',' << t.second << ')' << ' ';
        }
        cout << endl;
    }
}
