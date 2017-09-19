//
// Created by fellajim on 9/15/17.
//

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
    PnlMat* pnlMat;
    int size,nbTimeSteps;
    size_t n_samples;
    PnlVect *payoffCoeff = pnl_vect_create_from_scalar(size, 1.0 / size);

    char *infile = argv[1];
    Param *P = new Parser(infile);

    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    if (P->extract("dividend rate", divid, size) == false)
    {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("sample number", n_samples);

    P->print();
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
    nbTimeSteps=11;
    PnlRng* rng=pnl_rng_create(PNL_RNG_KNUTH);
    pnlMat=pnl_mat_create_from_zero(nbTimeSteps+1,size);
    BlackScholesModel* blackScholesModel = new BlackScholesModel(size, r, rho,sigma, spot);
    blackScholesModel->asset(pnlMat,T,nbTimeSteps,rng);
    OptionAsian* optionAsian = new OptionAsian(T, nbTimeSteps, size, payoffCoeff, strike);
    MonteCarlo monteCarlo(blackScholesModel, optionAsian, rng, 10.0, n_samples);
    double prix = 0.0;
    double ic = 0.0;
    monteCarlo.price(prix, ic);    pnl_mat_print(pnlMat);
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    delete P;

    exit(0);

}