#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "MonteCarlo.hpp"


#ifndef MC_PRICER_HEDGINGPORTFOLIO_H
#define MC_PRICER_HEDGINGPORTFOLIO_H

class HedgingPortfolio {
public:
    int H;
    MonteCarlo *monteCarlo;

    /**
    * Constructeur de la classe
    *
    * @param[in] nombre de pas
    * @param[in] modele Monte Carlo
    */
    HedgingPortfolio(int H, MonteCarlo *mCarlo);

    /**
    * Destructeur de classe
    */
    ~HedgingPortfolio();

    double hedgingPAndL(PnlVect *result, PnlMat *path);
};

#endif //MC_PRICER_HEDGINGPORTFOLIO_H
