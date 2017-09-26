#include "../src/BlackScholesModel.hpp"
#include "../src/parser.hpp"
#include <string>
#include <iostream>

using namespace std;

int main() {

    double T, r, strike, rho;
    PnlVect *spot, *sigma, *divid;
    PnlMat *pnlMat;
    string type;
    int size, nbtt;
    size_t n_samples;

    Param *P = new Parser("../data/basket.dat");

    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    P->extract("correlation", rho);
    if (P->extract("dividend rate", divid, size) == false) {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("sample number", n_samples);
    PnlRng *rng = pnl_rng_create(PNL_RNG_KNUTH);
    pnl_rng_sseed(rng, time(NULL));
    PnlVect *trend = pnl_vect_create_from_scalar(size, r);
    BlackScholesModel blackScholesModel(size, r, rho, sigma, spot, trend);
    PnlMat *path = blackScholesModel.simul_market(300, T, rng);

    if (path->m != 300 && path->n != size) {
        cout << "Erreur sur la taille de la matrice" << endl;
        exit(1);
    }

    pnl_rng_free(&rng);
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    pnl_vect_free(&trend);
    delete P;

    exit(0);

}
