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

MonteCarlo::~MonteCarlo() {
    delete opt_;
    pnl_rng_free(&rng_);
}

void MonteCarlo::price(double &prix, double &ic) {
    PnlMat *spotsMat = pnl_mat_create(opt_->nbTimeSteps() + 1, mod_->size_);
    double sum, squareSum, tempPayoff = 0.0;
    for (int i = 0; i < nbSamples_; i++) {
        mod_->asset(spotsMat, opt_->maturity(), opt_->nbTimeSteps(), rng_);
        tempPayoff = opt_->payoff(spotsMat);
        squareSum += pow(tempPayoff, 2);
        sum += tempPayoff;
    }
    sum /= nbSamples_;
    squareSum /= nbSamples_;

    /// Price computing
    prix = exp(-(mod_->r_) * (opt_->maturity())) * sum;

    /// Confidence interval in 95%
    double varianceEstimator = exp(-2 * (mod_->r_) * (opt_->maturity())) * (squareSum - pow(sum, 2));
    ic = 1.96 * sqrt(varianceEstimator) / sqrt(nbSamples_);

    /// Free the memory
    pnl_mat_free(&spotsMat);
};

void MonteCarlo::price(const PnlMat *past, double t, double &prix, double &ic) {
    if (t > opt_->maturity()) {
        std::cout << "le temps t est supérieur à la maturité  !!" << std::endl;
        exit(1);
    }
    std::cout << "_____MonteCarlo Computation______" << std::endl;
    std::cout << "________________________________" << std::endl;
    PnlMat *pathMat = pnl_mat_create(opt_->nbTimeSteps() + 1, mod_->size_);
    double tempPayoff = 0.0;
    double squareSum = 0.0;
    double sum = 0.0;
    for (int i = 0; i < nbSamples_; i++) {
        mod_->asset(pathMat, t, opt_->maturity(), opt_->nbTimeSteps(), rng_, past);
        tempPayoff = opt_->payoff(pathMat);
        squareSum += pow(tempPayoff, 2);
        sum += tempPayoff;
    }
    sum /= nbSamples_;
    squareSum /= nbSamples_;

    /// Price computing
    prix = exp(-(mod_->r_) * (opt_->maturity() - t)) * sum;

    /// Confidence interval in 95%
    double varianceEstimator = exp(-2 * (mod_->r_) * (opt_->maturity() - t)) * (squareSum - pow(sum, 2));
    ic = 1.96 * sqrt(varianceEstimator) / sqrt(nbSamples_);

    /// Free the memory
    pnl_mat_free(&pathMat);
}

void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta) {
    if (t > opt_->maturity()) {
        std::cout << "le temps t est supérieur à la maturité  !!" << std::endl;
        exit(1);
    }
    PnlMat *shift_path = pnl_mat_create(opt_->nbTimeSteps() + 1, mod_->size_);
    PnlMat *path = pnl_mat_create(opt_->nbTimeSteps() + 1, mod_->size_);
    for (int d = 0; d < mod_->size_; d++) {
        double delta_sum = 0;
        for (int i = 0; i < nbSamples_; i++) {
            mod_->asset(path, t, opt_->maturity(), opt_->nbTimeSteps(), rng_, past);
            mod_->shiftAsset(shift_path, path, d, fdStep_, t, opt_->maturity() / opt_->nbTimeSteps());
            double payoffhUp = opt_->payoff(shift_path);
            mod_->shiftAsset(shift_path, path, d, -fdStep_, t, opt_->maturity() / opt_->nbTimeSteps());
            double payoffhDown = opt_->payoff(shift_path);
            delta_sum += payoffhUp - payoffhDown;
        }
        LET(delta, d) =
                exp(-mod_->r_ * (opt_->maturity() - t)) / (2.0 * nbSamples_ * fdStep_ * MGET(past, past->m - 1, d)) *
                delta_sum;
    }

    /// Free the memory
    pnl_mat_free(&shift_path);
    pnl_mat_free(&path);
}

double MonteCarlo::hedgingPAndL(PnlVect *result, PnlMat *path, int H) {
    /// Initialisation
    PnlVect *delta_past = pnl_vect_create_from_zero(mod_->size_);
    PnlVect *delta_current = pnl_vect_create_from_zero(mod_->size_);
    PnlVect *delta_temp = pnl_vect_create_from_zero(mod_->size_);

    PnlVect *path_i = pnl_vect_create_from_zero(mod_->size_);
    double prix, ic;
    PnlMat *past_i = pnl_mat_create(1, mod_->size_);
    pnl_mat_get_row(path_i, path, 0);
    pnl_mat_set_row(past_i, path_i, 0);

    std::cout << "*** start init ***" << std::endl;

    /// Calcul du prix initial de l'option
    price(prix, ic);

    /// Calcul des deltas initiaux
    delta(past_i, 0, delta_past);

    std::cout << "*** start calcul ***" << std::endl;

    /// Complétion de la matrice result
    pnl_vect_set(result, 0, prix - pnl_vect_scalar_prod(delta_past, path_i));
    for (int i = 1; i <= H; i++) {
        std::cout << " i = " << i << std::endl;
        //pnl_mat_extract_subblock(past_i, path, 0, i, 0, monteCarlo->mod_->size_);
        pnl_mat_get_row(path_i, path, i);
        if (i % (H / opt_->nbTimeSteps()) == 0) {
            pnl_mat_add_row(past_i, past_i->m, path_i);
        }

        delta(past_i, i * opt_->maturity() / H, delta_current);
        pnl_vect_clone(delta_temp, delta_past);
        pnl_vect_clone(delta_past, delta_current);
        pnl_vect_minus_vect(delta_current, delta_temp);
        LET(result, i) = GET(result, i - 1) * exp(mod_->r_ * opt_->maturity() / H) -
                         (pnl_vect_scalar_prod(delta_current, path_i));
    }

    std::cout << "*** end ***" << std::endl;
    pnl_mat_print(path);
    double payoff = opt_->payoff(past_i);
    std::cout << "payoff : " << payoff << std::endl;
    double pAngLResult = GET(result, H) + pnl_vect_scalar_prod(delta_past, path_i) - payoff;

    /// Free the memory
    pnl_vect_free(&delta_current);
    pnl_vect_free(&delta_past);
    pnl_vect_free(&delta_temp);
    pnl_vect_free(&path_i);

    return pAngLResult;
}
