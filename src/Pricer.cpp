#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

#include "parser.hpp"
#include "BlackScholesModel.hpp"
#include "MonteCarlo.hpp"
#include "OptionAsian.hpp"
#include "OptionBasket.hpp"
#include "OptionPerformance.hpp"

using namespace std;

int main(int argc, char **argv)
{
    /*int option,c;
    while ((option = getopt(argc,argv,"c")) != EOF)
        switch(option)
        {
            case 'c': c = 1; cout << "c is enabled" << c << endl;
                if (argc > 4) {
                    cout << "Multiple input files --> one input file" << endl;
                    exit(1);
                }
                break;
            //case '': cout << "nothing is enabled" << endl; break;
            case '?': fprintf(stderr, "usuage is -c: <value> \n"); exit(1);
            //default: cout << endl;
        }
    
    if (argc < 2) {
        cout << "No input file" << endl;
    }*/
    
    char *infile;
    
    //Vérifications sur les arguments
    if ((argc != 2) && (argc != 4)){
        cout << "Bad number arguments \nTape --help for more information." << endl;
    }
    if (argc == 2){
        if (strcmp(argv[1], "--help") == 0){
            cout << "./pricer data_input : computes the price (with the associated confidence interval) and the deltas at t=0. Also gives the timing of functions called." << endl;
            cout << "./pricer -c market_file data_input : computes the P&L." << endl;
            return 0;
        } else {
            infile = argv[1];
        }
    }
    if (argc == 4){
        if (strcmp(argv[1], "-c") == 0){
            infile = argv[3];
        } else {
            cout << "Wrong way of calling the function: must be ./pricer -c market_file data_input \nTape --help for more information." << endl;
        }
    }
    
    double T, r, strike, corr, mcPrice;
    PnlVect *spot, *sigma, *divid, *payoffCoeff;
    string type;
    int size, nbTimeSteps;
    size_t n_samples;

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
    P->extract("timestep number", nbTimeSteps);
    /*if (P->extract("fd step", fdStep) == false){
        fdStep = 0.1;
    }*/
                         
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
    
    //Calcul du prix et des delta en zéro
    if (argc == 2){
        
        double prix;
        double ic;
        
        clock_t startPrice = clock();
        monteCarlo.price(prix, ic);
        clock_t timePrice = (clock() - startPrice) / (double)(CLOCKS_PER_SEC/1000);
        
        PnlVect* deltas = pnl_vect_create_from_zero(size);
        PnlMat* history = pnl_mat_create_from_scalar(nbTimeSteps + 1, size, 100.0);
        
        clock_t startDelta = clock();
        monteCarlo.delta(history,0,deltas);
        clock_t timeDelta = (clock() - startDelta) / (double)(CLOCKS_PER_SEC/1000);
        
        cout << "En t = 0 :" << endl;
        cout << "Prix = " << prix << endl;
        cout << "ic = " << ic << endl;
        cout << "L'intervalle de confiance à 95 % est donc : [" << prix -ic << "," << prix +ic << "]" << endl;
        cout << "Temps de calcul du prix (méthode price) T = " << timePrice << "ms" << endl;
        cout << "Delta :" <<endl;
        pnl_vect_print_asrow(deltas);
        cout << "Temps de calcul des delta (méthode delta) T = " << timeDelta << "ms" << endl;
        
        //Free memory
        pnl_vect_free(&deltas);
        pnl_mat_free(&history);
        
    }
    if (argc == 4) {

        PnlMat* history = pnl_mat_create_from_file(infile);
        
        PnlVect* results = pnl_vect_create_from_scalar(nbTimeSteps+1, 0.0);
        
        clock_t startPL = startPL = clock();
        double profitAndLoss = monteCarlo.hedgingPAndL(results, history, nbTimeSteps);
        clock_t timePL = (clock() - startPL) / (double)(CLOCKS_PER_SEC);
        
        cout << "P&L = " << profitAndLoss << endl;
        cout << "Temps de calcul de P&L (méthode hedgingPAndL) T = " << timePL << "ms" << endl;
        
        //Free memory
        pnl_mat_free(&history);
        pnl_vect_free(&results);
        //Deletion of hedgingPortfolio
        //delete &hedgingPortfolio;
    }
                         
    //Free memory
    pnl_rng_free(&rng);
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    pnl_vect_free(&payoffCoeff);
    pnl_vect_free(&trend);
    //Deletion of monteCarlo
    //delete &monteCarlo;
    delete P;
    
    return 0;
}

