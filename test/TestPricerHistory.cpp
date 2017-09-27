#include <iostream>
#include <string>
#include "../src/parser.hpp"
#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionBasket.hpp"
#include "../src/OptionPerformance.hpp"
#include "pnl/pnl_matrix.h"

using namespace std;

int main(int argc, char **argv) {

    if (argc < 2) {
        cout << "No input file" << endl;
    }

    if (argc > 2) {
        cout << "Multiple input files --> one input file" << endl;
        exit(1);
    }

    double T, r, strike, corr, fdStep;
    PnlVect *spot, *sigma, *payoffCoeff;
    string type;
    int size, nbTimeSteps;
    size_t n_samples;

    char *infile = argv[1];
    //Param *P = new Parser(infile);

    //P->extract("option type", type);
    //P->extract("maturity", T);
    //P->extract("option size", size);
    //P->extract("spot", spot, size);
    //P->extract("volatility", sigma, size);
    //P->extract("interest rate", r);
    //P->extract("correlation", corr);
    //P->extract("payoff coefficients", payoffCoeff,size);

    //if (P->extract("dividend rate", divid, size) == false)
    //{
    //    divid = pnl_vect_create_from_zero(size);
    //}
    //P->extract("strike", strike);
    //P->extract("sample number", n_samples);
    //P->extract("timestep number", nbTimeSteps);

    //P->print();

    type = "basket";
    strike =100;
    T = 3.0;
    //t = 0.0;
    size = 40;
    fdStep = 0.01;
    spot = pnl_vect_create_from_scalar(size, 100.0);
    sigma = pnl_vect_create_from_scalar(size, 0.2);
    //delta = pnl_vect_create_from_scalar(size, 0.0);
    r = 0.04879;
    corr = 0.0;
    payoffCoeff = pnl_vect_create_from_scalar(size, 0.025);
    n_samples = 1;
    nbTimeSteps = 200;

    //PnlMat* history = pnl_mat_create_from_scalar(1, size, 100.0);//pnl_mat_create_from_file(infile);
    PnlMat *history = pnl_mat_create_from_file(infile);

    //pnl_mat_print(history);

    /* Création de l'option en fonction du type */
    Option *opt = NULL;

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
    //opt = n1;tep = 0.0;

    /* Générateur aléatoire */
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    PnlVect *trend = pnl_vect_create_from_zero(size);
    BlackScholesModel* bsmod = new BlackScholesModel(size, r, corr, sigma, spot, trend);
    MonteCarlo monteCarlo(bsmod, opt, rng, fdStep, (int) n_samples); /* n_samples */
    /*HedgingPortfolio hedgingPortfolio(nbTimeSteps,&monteCarlo);
    PnlVect* results=pnl_vect_create_from_scalar(nbTimeSteps,0);
    hedgingPortfolio.hedgingPAndL(results,history);
    pnl_vect_print(results);
	*/
    double prix;
    double ic;
    std::cout << "_____MonteCarlo Computation_____"<< std::endl;
    monteCarlo.price(prix, ic);
    std::cout << prix << " | " << ic << std::endl;

    PnlVect* results = pnl_vect_create_from_scalar(nbTimeSteps, 0.0);
    monteCarlo.hedgingPAndL(results, history, nbTimeSteps);
    pnl_vect_print(results);
    //monteCarlo.delta(history, t, delta);
    //pnl_vect_print(delta);

    //monteCarlo.price(history, t , prix, ic);
    //monteCarlo.price(prix, ic);
    //std::cout << prix << " | " << ic << std::endl;

    pnl_rng_free(&rng);
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&payoffCoeff);
    pnl_mat_free(&history);
    pnl_vect_free(&trend);
    //delete P;

    exit(0);
}
