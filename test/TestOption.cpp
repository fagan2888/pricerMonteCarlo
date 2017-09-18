#include "../src/OptionBasket.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionPerformance.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

    bool  success = true;

    double maturity = 4;
    int nbTimeSteps = 4;
    int size = 1;
    double strike = 3;
    PnlVect *payoffCoeff = pnl_vect_create_from_scalar(size, 1.0 / size);
    PnlMat *path = pnl_mat_create_from_scalar(nbTimeSteps, size, 4);

    /// Test sur OptionBasket
    OptionBasket optionBasket(maturity, nbTimeSteps, size, payoffCoeff, strike);
    if (optionBasket.payoff(path) != 1) {
        cout << "Erreur sur le calcul du payoff de la Basketoption" << endl;
        success = false;
        return 1;
    }

    /// Test sur OptionAsian
    OptionAsian optionAsian(maturity, nbTimeSteps, size, payoffCoeff, strike);

    /// Test sur OptionPerformance
    OptionPerformance optionPerformance(maturity, nbTimeSteps, size, payoffCoeff);

    if (success) {
        cout << "SUCCESS" << endl;
    } else {
        cout << "FAIL" << endl;
    }

    return 0;
}