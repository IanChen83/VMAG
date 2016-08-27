#ifndef VMAG_HEADER
#define VMAG_HEADER

#include "vmag_ds.hpp"
#include <string>

extern ResourceBlockTable RBT;
extern RangeIndicator RI;
extern CostTable cost[MCS_LEVEL + 1];
extern UserTable user;
extern UserViewTable userView;

/******************************************************************************
 *                               Print Method                                 *
 *****************************************************************************/

void printRBT(ResourceBlockTable&);
void printUser(UserTable);
void printRange(RangeIndicator&);
void printCost(CostTable[]);
void printCostTrace(CostTable[], int, int, int);

/******************************************************************************
 *                               Parse Method                                 *
 *****************************************************************************/

bool parseRBT(const std::string);
bool parseUser(const std::string);

/******************************************************************************
 *                              Algorithm Method                              *
 *****************************************************************************/

void InitializationPhase();
void NAggregation(const int, const Range&, const Range&, int);
void VAggregation(const int, const Range&, const Range&, const Range&);
void VHAggregation(const int, const Range&, const Range&);
void Finalize();

void VMAG();

#endif
