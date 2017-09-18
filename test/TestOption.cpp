#include "../src/OptionBasket.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

    double maturity = 4;
    int nbTimeSteps = 1;
    int size = 1;
    double strike = 5;
    PnlVect *payoffCoeff = pnl_vect_create_from_scalar(size, 2);
    PnlMat *path = pnl_mat_create_from_scalar(maturity / nbTimeSteps, size, 4);

    /// Test sur OptionBasket
    OptionBasket optionBasket(maturity, nbTimeSteps, size, payoffCoeff, strike);
    if (optionBasket.payoff(path) != 3) {
        cout << "Erreur sur le calcul du payoff de la Basketoption" << endl;
        return 1;
    }

    return 0;
}