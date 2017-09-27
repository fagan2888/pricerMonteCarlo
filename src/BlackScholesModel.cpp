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

    /// Création de la matrice de Cholesky
    cholesky = pnl_mat_create_from_scalar(size_, size_, rho_);
    PnlMat *identity = pnl_mat_create(size_, size_);
    pnl_mat_set_id(identity);
    pnl_mat_mult_scalar(identity, 1 - rho_);
    pnl_mat_plus_mat(cholesky, identity);
    pnl_mat_chol(cholesky);
    pnl_mat_free(&identity);
}

BlackScholesModel::~BlackScholesModel() {
    pnl_mat_free(&cholesky);
}

void BlackScholesModel::asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) {
    /// Simulation de la trajectoire
    pnl_mat_set_row(path, spot_, 0);
    PnlVect *G_i = pnl_vect_create(size_);
    PnlVect *L_d = pnl_vect_create(size_);
    for (int i = 1; i <= nbTimeSteps; i++) {
        pnl_vect_rng_normal(G_i, size_, rng);
        for (int d = 0; d < size_; d++) {
            double sigma_d = pnl_vect_get(sigma_, d);
            pnl_mat_get_row(L_d, cholesky, d);
            double path_t_d = pnl_mat_get(path, i - 1, d) * exp((r_ - pow(sigma_d, 2) / 2) * T / nbTimeSteps +
                                                                sigma_d * sqrt(T / nbTimeSteps) *
                                                                pnl_vect_scalar_prod(L_d, G_i));
            pnl_mat_set(path, i, d, path_t_d);
        }
    }

    /// Libération de la mémoire
    pnl_vect_free(&G_i);
    pnl_vect_free(&L_d);

}

void BlackScholesModel::asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past) {
    /// Copie de la trajectoire passée dans la trajectoire totale
    int lastDatePast = (int) floor(t * (double) nbTimeSteps / T);

    PnlVect *tempRow = pnl_vect_create(size_);
    for (int i = 0; i <= lastDatePast; i++) {
        pnl_mat_get_row(tempRow, past, i);
        pnl_mat_set_row(path, tempRow, i);
    }
    /// Get the spot value

    PnlVect *spots_t = pnl_vect_create(size_);
    pnl_mat_get_row(spots_t, past, past->m - 1);

    /// Simulation de la trajectoire
    PnlVect *G_i = pnl_vect_create(size_);
    PnlVect *L_d = pnl_vect_create(size_);

    double timeInterval = (lastDatePast + 1) * T / (double) nbTimeSteps - t;
    for (int i = lastDatePast + 1; i <= nbTimeSteps; i++) {
        pnl_vect_rng_normal(G_i, size_, rng);
        for (int d = 0; d < size_; d++) {
            double sigma_d = pnl_vect_get(sigma_, d);
            pnl_mat_get_row(L_d, cholesky, d);
            double lastSpot = (i == lastDatePast + 1) ? GET(spots_t, d) : pnl_mat_get(path, i - 1, d);
            double path_t_d = lastSpot * exp((r_ - pow(sigma_d, 2) / 2) * timeInterval +
                                             sigma_d * sqrt(timeInterval) *
                                             pnl_vect_scalar_prod(L_d, G_i));
            pnl_mat_set(path, i, d, path_t_d);
        }
        timeInterval = T / nbTimeSteps;
    }

    /// Libération de la mémoire
    pnl_vect_free(&tempRow);
    pnl_vect_free(&spots_t);
    pnl_vect_free(&G_i);
    pnl_vect_free(&L_d);
}

void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep) {
    int currentDate = (int) floor(t / timestep);
    pnl_mat_clone(shift_path, path);
    for (int i = currentDate; i < path->m; i++) {
        pnl_mat_set(shift_path, i, d, MGET(path, i, d) * (1 + h));
    }
}

PnlMat *BlackScholesModel::simul_market(int H, double T, PnlRng *rng) {
    /// Simulation de la trajectoire

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
            pnl_mat_get_row(L_d, cholesky, d);
            double path_t_d = MGET(path, 0, d) * exp((trend_d - pow(sigma_d, 2) / 2) * timeInterval +
                                                     sigma_d * sqrt(timeInterval) *
                                                     pnl_vect_scalar_prod(L_d, G_i));
            pnl_mat_set(path, i, d, path_t_d);
        }
    }

    /// Libération de la mémoire
    pnl_vect_free(&G_i);
    pnl_vect_free(&L_d);

    return path;
}
