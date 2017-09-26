#include "../src/HedgingPortfolio.hpp"
#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/OptionBasket.hpp"
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

    P->extract("mc price", mcPrice);
    P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    P->extract("correlation", corr);
    P->extract("payoff coefficients", payoffCoeff, size);
    if (P->extract("dividend rate", divid, size) == false) {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("sample number", n_samples);
    P->extract("timestep number", nbTimeSteps);
    OptionBasket *opt = new OptionBasket(T, nbTimeSteps, size, payoffCoeff, strike);
    PnlRng *rng = pnl_rng_create(PNL_RNG_KNUTH);
    pnlMat = pnl_mat_create_from_zero(nbtt + 1, size);
    PnlVect *trend = pnl_vect_create_from_zero(size);
    BlackScholesModel blackScholesModel(size, r, rho, sigma, spot, trend);
    MonteCarlo monteCarlo(blackScholesModel, opt, rng, 0.1, n_samples);
    HedgingPortfolio hedgingPortfolio(n_samples, monteCarlo);
    PnlVect *results;
    //PnlMat *path = blackScholesModel.asset()
    //cout << hedgingPortfolio.hedgingPAndL()
}