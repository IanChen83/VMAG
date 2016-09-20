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
        kitem(int _c, int _v){
            cost = _c;
            value = _v;
        }
    };

    class vmagc{
        public:
            std::vector<vmag::vmag> carriers;

            kitem set_kitem(vmag::vmag&, vmag::vmag&, VLPair);

            int get_pattern(int, int, int, int, int);
            /**
             * VMAGC entry point.
             */
            void VMAGC();
            void reset();
            /**
             * Dispatch views from carrier a to carrier b using 0/1 knapsack algorithm.
             * Assume that both carriers have run VMAG algorithm.
             */
            void dispatch(vmag::vmag&, vmag::vmag&);
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
