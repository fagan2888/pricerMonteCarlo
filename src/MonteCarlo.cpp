#include "MonteCarlo.hpp"
#include <iostream>
#include <cmath>

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

MonteCarlo::MonteCarlo(BlackScholesModel *mod, Option *opt, PnlRng *rng, double fdStep, int nbSamples) {
    mod_ = mod;
    opt_ = opt;
    rng_ = rng;
    fdStep_ = fdStep;
    nbSamples_ = nbSamples;

    /// Initialisation des variables temporaires
    spotsMat = pnl_mat_create_from_zero(opt_->nbTimeSteps_ + 1, mod_->size_);
    shift_path = pnl_mat_create_from_zero(opt_->nbTimeSteps_ + 1, mod_->size_);
    path = pnl_mat_create_from_zero(opt_->nbTimeSteps_ + 1, mod_->size_);
}

MonteCarlo::~MonteCarlo() {
    pnl_mat_free(&spotsMat);
    pnl_mat_free(&shift_path);
    pnl_mat_free(&path);
}

void MonteCarlo::price(double &prix, double &ic) {
    double tempPayoff = 0.0;
    double squareSum = 0.0;
    double sum = 0.0;
    for (int i = 0; i < nbSamples_; i++) {
        mod_->asset(spotsMat, opt_->T_, opt_->nbTimeSteps_, rng_);
        tempPayoff = opt_->payoff(spotsMat);
        squareSum += pow(tempPayoff, 2);
        sum += tempPayoff;
    }
    sum /= nbSamples_;
    squareSum /= nbSamples_;

    /// Calcul du prix
    prix = exp(-(mod_->r_) * (opt_->T_)) * sum;

    /// Intervalle de confiance à 95%
    double varianceEstimator = exp(-2 * (mod_->r_) * (opt_->T_)) * (squareSum - pow(sum, 2));
    ic = 1.96 * sqrt(varianceEstimator) / sqrt(nbSamples_);
};

void MonteCarlo::price(const PnlMat *past, double t, double &prix, double &ic) {
    if (t > opt_->T_) {
        std::cerr << "le temps t est supérieur à la maturité  !!" << std::endl;
        exit(1);
    }
    double tempPayoff = 0.0;
    double squareSum = 0.0;
    double sum = 0.0;
    for (int i = 0; i < nbSamples_; i++) {
        mod_->asset(spotsMat, t, opt_->T_, opt_->nbTimeSteps_, rng_, past);
        tempPayoff = opt_->payoff(spotsMat);
        squareSum += pow(tempPayoff, 2);
        sum += tempPayoff;
    }
    sum /= nbSamples_;
    squareSum /= nbSamples_;

    /// Calcul du prix
    prix = exp(-(mod_->r_) * (opt_->T_ - t)) * sum;

    /// Intervalle de confiance à 95%
    double varianceEstimator = exp(-2 * (mod_->r_) * (opt_->T_ - t)) * (squareSum - pow(sum, 2));
    ic = 1.96 * sqrt(varianceEstimator) / sqrt(nbSamples_);
}

void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta) {
    if (t > opt_->T_) {
        std::cout << "le temps t est supérieur à la maturité  !!" << std::endl;
        exit(1);
    }
    for (int d = 0; d < mod_->size_; d++) {
        double delta_sum = 0;
        for (int i = 0; i < nbSamples_; i++) {
            mod_->asset(path, t, opt_->T_, opt_->nbTimeSteps_, rng_, past);
            mod_->shiftAsset(shift_path, path, d, fdStep_, t, opt_->T_ / opt_->nbTimeSteps_);
            double payoffhUp = opt_->payoff(shift_path);
            mod_->shiftAsset(shift_path, path, d, -fdStep_, t, opt_->T_ / opt_->nbTimeSteps_);
            double payoffhDown = opt_->payoff(shift_path);
            delta_sum += payoffhUp - payoffhDown;
        }
        LET(delta, d) =
                exp(-mod_->r_ * (opt_->T_ - t)) / (2.0 * nbSamples_ * fdStep_ * MGET(past, past->m - 1, d)) *
                delta_sum;
    }
}

double MonteCarlo::hedgingPAndL(PnlMat *path, int H) {
    /// Initialisation
    PnlVect *result = pnl_vect_create_from_zero(H+1);
    PnlVect *delta_past = pnl_vect_create_from_zero(mod_->size_);
    PnlVect *delta_current = pnl_vect_create_from_zero(mod_->size_);
    PnlVect *delta_temp = pnl_vect_create_from_zero(mod_->size_);

    PnlVect *path_i = pnl_vect_create_from_zero(mod_->size_);
    double prix, ic;
    PnlMat *past_i = pnl_mat_create(1, mod_->size_);
    pnl_mat_get_row(path_i, path, 0);
    pnl_mat_set_row(past_i, path_i, 0);

    /// Calcul du prix initial de l'option
    price(prix, ic);

    /// Calcul des deltas initiaux
    delta(past_i, 0, delta_past);

    /// Complétion de la matrice result
    pnl_vect_set(result, 0, prix - pnl_vect_scalar_prod(delta_past, path_i));
    for (int i = 1; i <= H; i++) {
        pnl_mat_get_row(path_i, path, i);
        //if (i % (H / opt_->nbTimeSteps_) == 0) {
        pnl_mat_add_row(past_i, past_i->m, path_i);
        //}

        delta(past_i, i * opt_->T_ / H, delta_current);
        if (i % (H / opt_->nbTimeSteps_) != 0)
            pnl_mat_del_row(past_i, past_i->m - 1);
        pnl_vect_clone(delta_temp, delta_past);
        pnl_vect_clone(delta_past, delta_current);
        pnl_vect_minus_vect(delta_current, delta_temp);
        LET(result, i) = GET(result, i - 1) * exp(mod_->r_ * opt_->T_ / H) -
                         (pnl_vect_scalar_prod(delta_current, path_i));
    }

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
