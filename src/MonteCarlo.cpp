#include "MonteCarlo.hpp"

#include <iostream>
#include <ctime>
#include <cmath>

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

MonteCarlo::MonteCarlo(BlackScholesModel *mod, Option *opt, PnlRng *rng, double fdStep, int nbSamples) {
    mod_ = mod;
    opt_ = opt;
    rng_ = rng;
    fdStep_ = fdStep;
    nbSamples_ = nbSamples;
}

void MonteCarlo::price(double &prix, double &ic) {
    auto spotsMat = pnl_mat_create(opt_->nbTimeSteps() + 1, mod_->size_);
    pnl_mat_set_all(spotsMat, 0.0);
    double sum = 0.0;
    double squareSum = 0.0;
    for (int i = 0; i < nbSamples_; i++) {
        mod_->asset(spotsMat, opt_->maturity(), opt_->nbTimeSteps(), rng_);
        double tempPayoff = opt_->payoff(spotsMat);
        squareSum += pow(tempPayoff, 2);
        sum += tempPayoff;
        //pnl_mat_print(spotsMat);
        //std::cout << "----------------" << std::endl;
    }

    sum /= nbSamples_;
    squareSum /= nbSamples_;

    /* Calcul du prix */
    prix = exp(-(mod_->r_) * (opt_->maturity())) * sum;

    /* Intervalle de confiance */
    double estimateurVariance = exp(-2 * (mod_->r_) * (opt_->maturity())) * (squareSum - pow(sum, 2));
    ic = 1.96 * sqrt(estimateurVariance) / sqrt(nbSamples_);
};

void MonteCarlo::price(const PnlMat *past, double t, double &prix, double &ic) {
    double sum = 0;
    double squareSum = 0;
    auto pathMat = pnl_mat_create(opt_->nbTimeSteps() + 1, mod_->size_);

    for (int i = 0; i < nbSamples_; i++) {
        mod_->asset(pathMat, t, opt_->maturity(), opt_->nbTimeSteps(), rng_, past);
        double tempPayoff = opt_->payoff(pathMat);
        squareSum += pow(tempPayoff, 2);
        sum += tempPayoff;
    }
    sum /= nbSamples_;
    squareSum /= nbSamples_;
    /* Calcul du prix */
    prix = exp(-(mod_->r_) * (opt_->maturity())) * sum;

    /* Intervalle de confiance */
    double estimateurVariance = exp(-2 * (mod_->r_) * (opt_->maturity())) * (squareSum - pow(sum, 2));
    ic = sqrt(estimateurVariance);
}
void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta) {
    double sum=0;
    auto shift_path = pnl_mat_create(opt_->nbTimeSteps() + 1, mod_->size_);
    auto path= pnl_mat_create(opt_->nbTimeSteps() + 1, mod_->size_);
    for (int d=0; d < mod_->size_; d++){
        double delta_sum=0;
        for (int i = 0; i < nbSamples_; i++) {
            mod_->asset(path, t, opt_->maturity(), opt_->nbTimeSteps(), rng_, past);
            mod_->shiftAsset(shift_path, path, d, fdStep_, t, opt_->maturity() / opt_->nbTimeSteps());
            double payoffhUp=opt_->payoff(shift_path);
            mod_->shiftAsset(shift_path, path, d, -fdStep_, t, opt_->maturity() / opt_->nbTimeSteps());
            double payoffhDown=opt_->payoff(shift_path);
            delta_sum+=payoffhUp-payoffhDown;
        }
        LET(delta,d)=exp(-mod_->r_*(opt_->maturity()-t)) / (2.0*nbSamples_*fdStep_*MGET(past, past->m-1, d));
    }
}