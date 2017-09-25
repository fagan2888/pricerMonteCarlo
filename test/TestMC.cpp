#include <iostream>
#include <string>
#include "../src/parser.hpp"
#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionBasket.hpp"
#include "../src/OptionPerformance.hpp"

using namespace std;

int main(int argc, char **argv) {

    if (argc < 2) {
        cout << "No input file" << endl;
    }

    if (argc > 2) {
        cout << "Multiple input files --> one input file" << endl;
        exit(1);
    }

    double T, r, strike, corr, mcPrice;
    PnlVect *spot, *sigma, *divid, *payoffCoeff;
    string type;
    int size, nbTimeSteps;
    size_t n_samples;

    char *infile = argv[1];
    Param *P = new Parser(infile);

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
	//n_samples = 5;
    P->extract("timestep number", nbTimeSteps);

    /* Création de l'option en fonction du type */
    Option *opt;

    if (type.compare("asian") == 0)
        opt = new OptionAsian(T, nbTimeSteps, size, payoffCoeff, strike);
    if (type.compare("basket") == 0)
        opt = new OptionBasket(T, nbTimeSteps, size, payoffCoeff, strike);
    if (type.compare("performance") == 0)
        opt = new OptionPerformance(T, nbTimeSteps, size, payoffCoeff);
	if (type.compare("asian") != 0 && type.compare("basket") != 0 && type.compare("performance") != 0) {
		cout << "Bad option type !" << endl;
        exit(1);
    }

    PnlVect *trend = pnl_vect_create_from_zero(size);
    BlackScholesModel *bsmod = new BlackScholesModel(size, r, corr, sigma, spot, trend);

    /* Pas de temps */
    double fdStep = 0.1;
    /* Générateur aléatoire */
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));

    MonteCarlo monteCarlo(bsmod, opt, rng, fdStep, (int) n_samples);

    double prix;
    double ic;
    //monteCarlo.price(prix, ic);
    PnlVect* deltas=pnl_vect_create_from_zero(size);
    PnlMat* history = pnl_mat_create_from_scalar(1, size, 100.0);
    monteCarlo.delta(history,0,deltas);
    pnl_vect_print(deltas);
    //monteCarlo.price(prix,ic);
    monteCarlo.price(history,0,prix,ic);

    cout << "Prix attendu : " << mcPrice << endl;
    cout << "Prix et ic obtenu : " << prix << " | " << ic << endl;

    /*int nbtt = 10;
    PnlMat *pnlMat = pnl_mat_create_from_scalar(nbtt + 1, size, 10);
    bsmod->asset(pnlMat, T, nbtt, rng);
    PnlVect *deltas = pnl_vect_create_from_scalar(size, 1.0 / size);
    monteCarlo.delta(pnlMat, 0, deltas);
    pnl_vect_print(deltas);*/

    pnl_rng_free(&rng);
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    pnl_vect_free(&payoffCoeff);
    pnl_vect_free(&trend);
    delete P;

    if (abs(mcPrice - prix) / mcPrice < 5 * ic) {
        exit(0);
    } else {
        exit(1);
    }

}
