//
// Created by fellajim on 9/22/17.
//
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "MonteCarlo.hpp"


#ifndef MC_PRICER_HEDGINGPORTFOLIO_H
#define MC_PRICER_HEDGINGPORTFOLIO_H
class HedgingPortfolio {
public:
    int H;
    MonteCarlo* monteCarlo;

    HedgingPortfolio(int H, MonteCarlo* mCarlo);
    double hedgingPAndL(PnlVect* result, PnlMat* path);
};
#endif //MC_PRICER_HEDGINGPORTFOLIO_H
