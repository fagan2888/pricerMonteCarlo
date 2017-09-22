#include <iostream>
#include <string>
#include "../src/parser.hpp"
#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionBasket.hpp"
#include "../src/OptionPerformance.hpp"

using namespace std;

int main(int argc, char **argv)
{
   double T, r, strike, corr;
    PnlVect *spot, *sigma, *divid, *payoffCoeff;
    string type;
    int size, nbTimeSteps;
    size_t n_samples; /* ATTENTION : type long donné en entrée ! */

    char *infile = argv[1];
    Param *P = new Parser(infile);

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

    P->print();

    Option *opt;

    if (type.compare("asian") == 0)
        opt = new OptionAsian(T, nbTimeSteps, size, payoffCoeff, strike);
    if (type.compare("basket") == 0)
        opt = new OptionBasket(T, nbTimeSteps, size, payoffCoeff, strike);
    if (type.compare("performance") == 0)
        opt = new OptionPerformance(T, nbTimeSteps, size, payoffCoeff);

    BlackScholesModel *bsmod = new BlackScholesModel(size, r, corr, sigma, spot);

    /* Pas de temps */
    double fdStep = 0.1;
    /* Générateur aléatoire */
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));

    MonteCarlo monteCarlo(bsmod, opt, rng, fdStep, n_samples);

    double prix;
    double ic;
    monteCarlo.price(prix, ic);
    cout << prix << " | " << ic << endl;

    int nbtt = 10;
    PnlMat *pnlMat = pnl_mat_create_from_scalar(nbtt + 1, size, 10);
    bsmod->asset(pnlMat, T, nbtt, rng);
    PnlVect *deltas = pnl_vect_create_from_scalar(size, 1.0 / size);
    monteCarlo.delta(pnlMat, 0, deltas);
    pnl_vect_print(deltas);

    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    pnl_vect_free(&payoffCoeff);
    delete P;

    exit(0);
}
