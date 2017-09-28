#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/OptionBasket.hpp"
#include "../src/parser.hpp"
#include <string>
#include <iostream>

using namespace std;

int main() {

    /// Déclaration des variables
    double T, r, strike, mcPrice, corr;
    PnlVect *spot, *sigma, *divid, *payoffCoeff;
    string type;
    int size, nbTimeSteps;
    size_t n_samples;

    /// Parser
    Param *P = new Parser("../data/asian.dat");
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
    n_samples = 100;
    P->extract("timestep number", nbTimeSteps);

    /// Test
    OptionBasket *opt = new OptionBasket(T, nbTimeSteps, size, payoffCoeff, strike);
    PnlRng *rng = pnl_rng_create(PNL_RNG_KNUTH);
    pnl_rng_sseed(rng, time(NULL));
    PnlVect *trend = pnl_vect_create_from_zero(size);
    BlackScholesModel *bsmodel = new BlackScholesModel(size, r, corr, sigma, spot, trend);
    MonteCarlo *monteCarlo = new MonteCarlo(bsmodel, opt, rng, 0.1, (int) n_samples);
    PnlMat *path = pnl_mat_create(nbTimeSteps + 1, size);
    bsmodel->asset(path, T, nbTimeSteps, rng);
    cout << "P&L : " << monteCarlo->hedgingPAndL(path, (int) n_samples) << endl;

    /// Libération de la mémoire
    delete P;
    delete opt;
    pnl_rng_free(&rng);
    delete bsmodel;
    delete monteCarlo;

    exit(0);
}
