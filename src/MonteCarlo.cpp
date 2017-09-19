#include "MonteCarlo.hpp"

#include <iostream>
#include <ctime>
#include <cmath>

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

MonteCarlo::MonteCarlo(BlackScholesModel* mod, Option* opt, PnlRng* rng, double fdStep, int nbSamples)
{
	mod_ = mod;
	opt_ = opt;
	rng_ = rng;
	fdStep_ = fdStep;
	nbSamples_ = nbSamples;
}



void MonteCarlo::price(double &prix, double &ic)
{
	auto spotsMat = pnl_mat_create (10, 1);
	pnl_mat_set_all(spotsMat, 10.0);

	auto sum = 0;
	auto squareSum = 0;
	for(int i = 0; i<  nbSamples_; i++)
	{
		mod_->asset(spotsMat, opt_->maturity(), opt_->nbTimeSteps(), rng_);
		auto tempPayoff = opt_->payoff(spotsMat);
		squareSum += pow(tempPayoff, 2);
		sum += tempPayoff;
	}

	sum /= nbSamples_;
	squareSum /= nbSamples_;

    /* Calcul du prix */
	prix = exp(-(mod_->r_)*(opt_->maturity()))*sum;

    /* Intervalle de confiance */
	auto estimateurVariance = exp(-2*(mod_->r_)*(opt_->maturity())) * (squareSum - pow(sum,2));
	ic = sqrt(estimateurVariance);
};

void MonteCarlo::price(const PnlMat *past, double t, double &prix, double &ic) {
    auto spotsMat=
}
