#include "../src/OptionBasket.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionPerformance.hpp"
#include "../src/MonteCarlo.hpp"

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"

#include <iostream>

using namespace std;

int main() {

    bool success = true;

    double maturity = 4;
    int nbTimeSteps = 4;
    int size = 1;
    double strike = 3;
    PnlVect *payoffCoeff = pnl_vect_create_from_scalar(size, 1.0 / size);
    PnlMat *path = pnl_mat_create_from_scalar(nbTimeSteps + 1, size, 4);

    /// Test sur OptionBasket
    OptionBasket optionBasket(maturity, nbTimeSteps, size, payoffCoeff, strike);
    if (optionBasket.payoff(path) != 1) {
        cout << "Erreur sur le calcul du payoff de la Basketoption" << endl;
        success = false;
    }

    /// Test sur OptionAsian
    OptionAsian *optionAsian = new OptionAsian(maturity, nbTimeSteps, size, payoffCoeff, strike);
    PnlRng *rng = pnl_rng_create(PNL_RNG_KNUTH);
    pnl_rng_sseed(rng, 0);
    BlackScholesModel *bsModel = new BlackScholesModel(1, 1.01, 1.0, pnl_vect_create_from_scalar(1, 0.4),
                                                       pnl_vect_create_from_scalar(1, 10.0));
    MonteCarlo monteCarlo(bsModel, optionAsian, rng, 10.0, 50000);
    double prix, ic;
    monteCarlo.price(prix, ic);

    /// Test sur OptionPerformance
    OptionPerformance optionPerformance(maturity, nbTimeSteps, size, payoffCoeff);

    pnl_rng_free(&rng);
    pnl_vect_free(&payoffCoeff);
    pnl_mat_free(&path);

    if (success) {
        exit(0);
    } else {
        exit(1);
    }
}
