#include "../src/BlackScholesModel.hpp"
#include "../src/parser.hpp"
#include "../src/OptionBasket.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionPerformance.hpp"
#include "../src/MonteCarlo.hpp"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

    double T, r, strike, rho;
    PnlVect *spot, *sigma, *divid;
    PnlMat *pnlMat;
    string type;
    int size, nbtt;
    size_t n_samples;
    PnlVect *payoffCoeff = pnl_vect_create_from_scalar(size, 1.0 / size);

    Param *P = new Parser("../data/basket.dat");

    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    if (P->extract("dividend rate", divid, size) == false) {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("sample number", n_samples);
    nbtt = 11;
    PnlRng *rng = pnl_rng_create(PNL_RNG_KNUTH);
    pnlMat = pnl_mat_create_from_zero(nbtt + 1, size);
    BlackScholesModel blackScholesModel(size, r, rho, sigma, spot);
    blackScholesModel.asset(pnlMat, T, nbtt, rng);
    //pnl_mat_print(pnlMat);

    /// Test de BlackScholesModel avec un historique
    pnlMat = pnl_mat_create_from_zero(nbtt + 1, size);
    int t = 2;
    PnlMat *past = pnl_mat_create_from_scalar(t, size, 4);
    //blackScholesModel.asset(pnlMat,t,T,nbtt,rng,past);
    //pnl_mat_print(past);
    cout << "" << endl;
    //pnl_mat_print(pnlMat);


    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    delete P;

    exit(0);

}