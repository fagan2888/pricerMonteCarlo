#include <iostream>
#include <string>
#include "../src/parser.hpp"
#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionBasket.hpp"
#include "../src/OptionPerformance.hpp"


using namespace std;

/*enum option_kind {
  ASIAN, 
  BASKET, 
  PERF, 
};*/


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
	P->extract("payoff coefficients", payoffCoeff,size);

    if (P->extract("dividend rate", divid, size) == false)
    {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("sample number", n_samples);
	P->extract("timestep number", nbTimeSteps);

    P->print();
	
	//enum option_kind para;	

	/* Création de l'option en fonction du type */

	Option * opt;

	if(type.compare("asian") == 0)
		opt = new OptionAsian(T, nbTimeSteps, size, payoffCoeff, strike);
	if(type.compare("basket") == 0)
		opt = new OptionBasket(T, nbTimeSteps, size, payoffCoeff, strike);
	if(type.compare("performance") == 0)
		opt = new OptionPerformance(T, nbTimeSteps, size, payoffCoeff);

	/*switch (para) {
  		case ASIAN:            
    		OptionAsian * opt = new OptionAsian(T, nbTimeSteps, size, payoffCoeff, strike); 
    		break;
  		case BASKET:            
    		OptionBasket * opt = new OptionBasket(T, nbTimeSteps, size, payoffCoeff, strike); 
   			break;
  		case PERFORMANCE:            
    		OptionPerformance * opt = new OptionPerformance(T, nbTimeSteps, size, payoffCoeff, strike); 
    		break;
  		default:            
    		cout<<"Error, bad option type, quitting\n";
    		abort();
  	}*/

	BlackScholesModel* bsmod = new BlackScholesModel(size, r, corr, sigma, spot);

	/* Pas de temps */
    double fdStep = 1.0;
	/* Générateur aléatoire */
	PnlRng* rng = pnl_rng_create(PNL_RNG_KNUTH);
	pnl_rng_sseed(rng, 0);

	MonteCarlo monteCarlo(bsmod, opt, rng, fdStep, 50000); /* n_samples */

	double prix;
	double ic;
	monteCarlo.price(prix, ic);
  	std::cout << prix << " | " << ic << std::endl;

	//cout << "mc price " << prix << endl;

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
