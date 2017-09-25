//
// Created by fellajim on 9/22/17.
//

#ifndef MC_PRICER_HEDGINGPORTFOLIO_H
#define MC_PRICER_HEDGINGPORTFOLIO_H
class HedgingPortfolio {
    double H;
    MonteCarlo* monteCarlo;

    double hedgingPAndL(PnlVect* result, PnlMat* path);
};
#endif //MC_PRICER_HEDGINGPORTFOLIO_H
