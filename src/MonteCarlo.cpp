#include <iostream>
#include <ctime>
#include <cmath>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

using namespace std;

void price(double &prix, double &ic)
{
	auto spotsMat = pnl_mat_create (10, 1);
	pnl_mat_set_all(spotsMat, 10.0);
	
	auto sum = 0;
	auto squareSum = 0;
	for(int i = 0; i<  nbSamples_; i++)	
	{
		asset(spotsMat, opt_->T_, opt_->nbTimeSteps_, rng_);
		auto tempPayoff = opt_->payoff(spotsMat);
		squareSum += pow(tempPayoff, 2);
		sum += tempPayoff;		
	}

	sum /= nbSamples_;
	squareSum /= nbSamples_;

    /* Calcul du prix */
	prix = exp(-(mod_->r_)*(opt_->T_))*sum;  
    
    /* Intervalle de confiance */
	auto estimateurVariance = exp(-2*(mod_->r_)*(opt_->T_)) * (squareSum - pow(sum,2));
	ic = sqrt(estimateurVariance);
};

