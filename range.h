#ifndef VMAG_RANGE_H
#define VMAG_RANGE_H

#include <utility>
#include <vector>
#include <functional>

namespace vmag {
    struct Range : std::pair<int, int> {
        Range(int, int);

        /**
         * @brief Expand the range, boundary-safe
         *
         * @param r    length to expand
         */
        void expand(int);

        Range expandDup(int);
    };

    /**
     * @brief Return true if a is overlapped with b
     *
     * @param a     Range a
     * @param b     Range b
     */
    bool isOverlapped(const Range &, const Range &);

    /**
     * @brief Return true if a covers b
     *
     * @param a     Range a
     * @param b     Range b
     **/
    bool isCover(const Range &, const Range &);


    /**
     * @brief Get Overlapped range of a and b
     *
     * @param a     Range a
     * @param b     Range b
     */
    Range getOverlapped(const Range &, const Range &);
}

#define NULL_RANGE Range(-1, -1)

#endif //VMAG_RANGE_H
