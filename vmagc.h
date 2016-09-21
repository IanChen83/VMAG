#include "knapsack.h"
#include "vmag.h"
#include <vector>

namespace vmagc{
    /********************************************************************
    * There're four params that are important when deciding view's value
    *
    * b ***|***|***|***
    *      bl  ^   br
    *          c
    *
    * a *****|***|*****
    *        al  ar
    *
    *******************************************************************/
    typedef std::pair<int, int> VLPair;
    struct kitem : knapsack::kitem{
        std::pair<int, int> view;
        kitem(int _c, int _v, std::pair<int, int> _view){
            cost = _c;
            value = _v;
            view = _view;
        }
    };

    class vmagc{
        public:
            std::vector<vmag::vmag> carriers;

            kitem set_kitem(vmag::vmag&, vmag::vmag&, VLPair);

            int one_round();

            int get_pattern(int, int, int, int, int, int);
            /**
             * VMAGC entry point.
             */
            void VMAGC();
            void reset();
            /**
             * Dispatch views from carrier a to carrier b using 0/1 knapsack algorithm.
             * Assume that both carriers have run VMAG algorithm.
             */
            std::vector<kitem> dispatch(vmag::vmag&, vmag::vmag&);

            std::map<int, vmag::VLPair > result;
        private:
            const int PATTERN_NONE = 0;
            const int PATTERN_UNDER = 1;
            const int PATTERN_INNER = 2;
            const int PATTERN_CODE[3][3][2] =
            {
                {{1, 2}, {3, 4}, {5, 6}},
                {{7, 8}, {9, 10}, {11, 12}},
                {{13, 14}, {15, 16}, {17, 18}}};
    };
}
