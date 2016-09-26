#include "vmag_ds.h"
#include <stack>
#include <tuple>
bool isOverlapped(const Range& a, const Range& b){
    return !((a.second < b.first) || (b.second < a.first));
}

bool isCover(const Range& a, const Range& b){
    return (a.first <= b.first && a.second >= b.second);
}

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

Range getOverlapped(const Range& a, const Range& b){
    if(a.first > b.first) return getOverlapped(b, a);
    if(b.first > a.second) return NULL_RANGE;
    return Range(b.first, (a.second > b.second)? b.second : a.second);
}

std::map<int, int> getCostTrace(CostTable ct[], int level, int from, int to){
    typedef std::tuple<int, int, int> LFT;
    const int L=0, F=1, T=2;

    std::map<int, int> ret;
    std::stack<LFT> s;
    CostBlock* c;
    int _l, _f, _t;

    s.push(std::make_tuple(level, from ,to));
    while(!s.empty()){
        std::tie(_l, _f, _t) = s.top();
        c = &ct[_l][_f][_t];

        if(_f == _t){
            if(ret.count(_t) == 0){
                ret[_t] = _l;
            }
            s.pop();
        }else if(c->method == 'N'){
            std::get<T>(s.top()) = c->prev;
            if(ret.count(_t) == 0){
                ret[_t] = _l;
            }
            continue;
        }else if(c->method == 'V'){
            std::get<T>(s.top()) = c->prev;
            s.push(std::make_tuple(_l-1, c->prev, _t));
        }else if(c->method == 'C'){
            std::get<T>(s.top()) = c->prev;
            s.push(std::make_tuple(_l-1, c->prev, _t));
        }
    }
    if(ret.count(_t) == 0){
        ret[_t] = _l;
    }


    return ret;
}
