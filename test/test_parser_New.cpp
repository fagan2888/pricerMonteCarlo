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
    Param *P = new Parser("../data/basket.dat");
/*
model type          <string>  bs
option size         <int>     2
strike              <float>   100
spot                <vector>  100
maturity            <float>   1.5
volatility          <vector>  0.2
interest rate       <float>   0.02
correlation         <float>   0.0
*/
/*
option type         <string>  asian
payoff coefficients <vector>  0.5

timestep number     <int>     150
sample number       <long>    50000
*/
    P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
	P->extract("correlation", corr);
	P->extract("payoff coefficients", payoffCoeff,size);

    if (P->extract("dividend rate", divid, size) == false)
    {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("sample number", n_samples);
	P->extract("timestep number", nbTimeSteps);

    P->print();
	
	/* Création de l'option en fonction du type */
	OptionBasket * optA = new OptionBasket(T, nbTimeSteps, size, payoffCoeff, strike); 	

	BlackScholesModel* bsmod = new BlackScholesModel(size, r, corr, sigma, spot);

	/* Pas de temps */
    double fdStep = 1.0;
	/* Générateur aléatoire */
	PnlRng* rng = pnl_rng_create(PNL_RNG_KNUTH);
	//pnl_rng_sseed(rng, 0);

	//MonteCarlo * monte = new MonteCarlo(bsmod, optA, rng, fdStep, 50000); /* n_samples */

	MonteCarlo monteCarlo(bsmod, optA, rng, fdStep, 50000);

	double prix;
	double ic;
	monteCarlo.price(prix, ic);
  	std::cout << prix << " | " << ic << std::endl;

	//cout << "mc price " << prix << endl;#include "pnl/pnl_random.h"

	/*
    cout << endl;
    cout << "option type " << type << endl;
    cout << "maturity " << T << endl;
    cout << "strike " << strike << endl;
    cout << "option size " << size << endl;
    cout << "interest rate " << r << endl;
    cout << "dividend rate ";
    pnl_vect_print_asrow(divid);
    cout << "spot ";
    pnl_vect_print_asrow(spot);
    cout << "volatility ";
    pnl_vect_print_asrow(sigma);
    cout << "Number of samples " << n_samples << endl;
	*/

    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
	pnl_vect_free(&payoffCoeff);
    delete P;

    exit(0);
}
