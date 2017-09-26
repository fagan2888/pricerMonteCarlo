#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>

using namespace std;

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot, PnlVect *trend) {
    size_ = size;
    r_ = r;
    rho_ = rho;
    sigma_ = sigma;
    spot_ = spot;
    trend_ = trend;
}

BlackScholesModel::~BlackScholesModel() {}

void BlackScholesModel::asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) {
    /// Cholesky decomposition
    PnlMat *gamma = pnl_mat_create_from_scalar(size_, size_, rho_);
    PnlMat *identity = pnl_mat_create(size_, size_);
    pnl_mat_set_id(identity);
    pnl_mat_mult_scalar(identity, 1 - rho_);
    pnl_mat_plus_mat(gamma, identity);
    pnl_mat_chol(gamma);

    pnl_mat_set_row(path, spot_, 0);
    PnlVect *G_i = pnl_vect_create(size_);
    PnlVect *L_d = pnl_vect_create(size_);
    for (int i = 1; i <= nbTimeSteps; i++) {
        pnl_vect_rng_normal(G_i, size_, rng);
        for (int d = 0; d < size_; d++) {
            double sigma_d = pnl_vect_get(sigma_, d);
            pnl_mat_get_row(L_d, gamma, d);
            double path_t_d = pnl_mat_get(path, i - 1, d) * exp((r_ - pow(sigma_d, 2) / 2) * T / nbTimeSteps +
                                                                sigma_d * sqrt(T / nbTimeSteps) *
                                                                pnl_vect_scalar_prod(L_d, G_i));
            pnl_mat_set(path, i, d, path_t_d);
        }
    }

    /// Free the memory
    pnl_mat_free(&gamma);
    pnl_mat_free(&identity);
    pnl_vect_free(&G_i);
    pnl_vect_free(&L_d);
}

void BlackScholesModel::asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past) {
    /// Cholesky decomposition
    PnlMat *gamma = pnl_mat_create_from_scalar(size_, size_, rho_);
    PnlMat *identity = pnl_mat_create(size_, size_);
    pnl_mat_set_id(identity);
    pnl_mat_mult_scalar(identity, 1 - rho_);
    pnl_mat_plus_mat(gamma, identity);
    pnl_mat_chol(gamma);

    /// Copy the past matrix on the path matrix
    int lastDatePast = (int) floor(t * (double) nbTimeSteps / T);
    double firstStep = lastDatePast*(T/nbTimeSteps) - t;
    PnlVect *tempRow = pnl_vect_create(size_);
    /*if (firstStep == 0.0) {///special case : t = ti
        pnl_mat_set_subblock(path, past, 0, 0);
        firstStep = T/nbTimeSteps;
        lastDatePast += 1;
    } else {*/
        //for (int i = 0; i <= lastDatePast; i++) {
        for (int i=0; i < past->m -1; i++){
            pnl_mat_get_row(tempRow, past, i);
            pnl_mat_set_row(path, tempRow, i);
        }
    //}
    /// Get the spot value  A VOIR ??
    PnlVect *spots_t = pnl_vect_create(size_);
    pnl_mat_get_row(spots_t, past, past->m - 1);

    /// Simulate the end of path
    PnlVect *G_i = pnl_vect_create(size_);
    PnlVect *L_d = pnl_vect_create(size_);
    double timeInterval = (lastDatePast +1 )* T / (double) nbTimeSteps-t;
    for (int i = lastDatePast + 1; i <= nbTimeSteps; i++) {
        pnl_vect_rng_normal(G_i, size_, rng);
        for (int d = 0; d < size_; d++) {
            double sigma_d = pnl_vect_get(sigma_, d);
            pnl_mat_get_row(L_d, gamma, d);
            double lastSpot = (i == lastDatePast+1) ? GET(spots_t, d) : pnl_mat_get(path, i - 1, d);
            double path_t_d = lastSpot * exp((r_ - pow(sigma_d, 2) / 2) * timeInterval +
                                             sigma_d * sqrt(timeInterval) *
                                             pnl_vect_scalar_prod(L_d, G_i));
            pnl_mat_set(path, i, d, path_t_d);
        }
        timeInterval = T / nbTimeSteps;
    }

    /// Free the memory
    pnl_mat_free(&gamma);
    pnl_mat_free(&identity);
    pnl_vect_free(&tempRow);
    pnl_vect_free(&spots_t);
    pnl_vect_free(&G_i);
    pnl_vect_free(&L_d);
}

void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep) {
    //int currentDate = (int) floor(t / timestep);
    int currentDate = (int) floor(t / timestep) + 1;
    pnl_mat_clone(shift_path, path);
    for (int i = currentDate; i < path->m; i++) {
        //pnl_mat_set(shift_path, i, d, MGET(path, currentDate, d) * (1 + h));
        pnl_mat_set(shift_path, i, d, MGET(path, i, d) * (1 + h));
    }
}

PnlMat *BlackScholesModel::simul_market(int H, double T, PnlRng *rng) {
    /// Cholesky decomposition
    PnlMat *gamma = pnl_mat_create_from_scalar(size_, size_, rho_);
    PnlMat *identity = pnl_mat_create(size_, size_);
    pnl_mat_set_id(identity);
    pnl_mat_mult_scalar(identity, 1 - rho_);
    pnl_mat_plus_mat(gamma, identity);
    pnl_mat_chol(gamma);

    /// Path simulation
    PnlMat *path = pnl_mat_create_from_zero(H, size_);
    pnl_mat_set_row(path, spot_, 0);
    PnlVect *G_i = pnl_vect_create(size_);
    PnlVect *L_d = pnl_vect_create(size_);
    for (int i = 1; i < H; i++) {
        pnl_vect_rng_normal(G_i, size_, rng);
        for (int d = 0; d < size_; d++) {
            double trend_d = pnl_vect_get(trend_, d);
            double sigma_d = pnl_vect_get(sigma_, d);
            double timeInterval = i * T / H;
            pnl_mat_get_row(L_d, gamma, d);
            double path_t_d = MGET(path, 0, d) * exp((trend_d - pow(sigma_d, 2) / 2) * timeInterval +
                                                     sigma_d * sqrt(timeInterval) *
                                                     pnl_vect_scalar_prod(L_d, G_i));
            pnl_mat_set(path, i, d, path_t_d);
        }
    }

    /// Free the memory
    pnl_mat_free(&gamma);
    pnl_mat_free(&identity);
    pnl_vect_free(&G_i);
    pnl_vect_free(&L_d);

    return path;
}

/* Une autre proposition pour simul_market */
/*void BlackScholesModel::simul_market(PnlMat *path, double T, int H, PnlRng *rng){
    
    pnl_mat_set_row(path, spot_, 0);
    PnlVect *pastPrices = pnl_vect_copy(spot_);
    PnlMat *corr = pnl_mat_create_from_scalar(size_, size_, rho_);
    PnlVect *G = pnl_vect_create(size_);
    for (int i = 0; i < H; i++){
        pnl_vect_rng_normal(G, size_, rng);
        for (int j = 0; j < size_; j++){
            PnlVect *viewCorr = pnl_vect_wrap_mat_row(corr, j);
            double prod = pnl_vect_scalar_prod(viewCorr,G);
            double sigmaS = GET(sigma_, j);
            double bs = GET(&pastPrices,j) * exp( ( GET(trends_,j)- (pow(sigmaS,2)/2))*(T/H) + sigmaS*sqrt(T/H)*prod) ;
            pnl_mat_set(path, i+1, j, bs);
        }
        pastPrices = pnl_vect_wrap_mat_row(path, i+1);
    }
}*/

/* Proposition pour asset avec prise en compte des spots historiques --> aide pour vérification */

/*
 void BlackScholesModel::asset_Past(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng){
    pnl_mat_set_row(path, spot_, 0);
    PnlVect *pastPrices = pnl_vect_copy(spot_);
    PnlMat *corr = pnl_mat_create_from_scalar(size_, size_, rho_);
    PnlVect *G = pnl_vect_create(size_);
    for (int i = 0; i < nbTimeSteps; i++){
        PnlMat *correl = pnl_mat_copy(corr);
        pnl_mat_chol(correl);
        pnl_vect_rng_uni(G, size_,0,0,rng);
        for (int j = 0; j < size_; j++){
            PnlVect *viewCorr = pnl_vect_wrap_mat_row(corr, j);
            double prod = pnl_vect_scalar_prod(viewCorr,G);
            double bs = GET(pastPrices,j) * exp( (r_ - (pow(GET(sigma_, j),2)/2))*(T/nbTimeSteps) +  GET(sigma_, j)*sqrt(T/nbTimeSteps)*prod) ;
            pnl_vect_set(pastPrices, j, bs);
        }
        pnl_mat_set_row(path, pastPrices, i+1);
        pnl_mat_free(&correl);
    }
    pnl_vect_free(&pastPrices);
}
 */

