#include "HedgingPortfolio.hpp"
#include <cmath>
#include <iostream>


HedgingPortfolio::HedgingPortfolio(int H, MonteCarlo *mCarlo) {
    this->H = H;
    monteCarlo = mCarlo;
}

HedgingPortfolio::~HedgingPortfolio() {
    delete monteCarlo;
}

double HedgingPortfolio::hedgingPAndL(PnlVect *result, PnlMat *path) {
    PnlVect *delta_0 = pnl_vect_create(monteCarlo->mod_->size_);
    PnlVect *delta_i = pnl_vect_create(monteCarlo->mod_->size_);
    PnlVect *path_i = pnl_vect_create(monteCarlo->mod_->size_);
    double prix, ic;
    monteCarlo->price(prix, ic);
    PnlMat past_0 = pnl_mat_wrap_mat_rows(path, 0, 0);
    monteCarlo->delta(&past_0, 0, delta_0);
    pnl_mat_get_row(path_i, path, 0);
    LET(result, 0) = prix - pnl_vect_scalar_prod(delta_0, path_i);
    for (int i = 1; i < H + 1; i++) {
        PnlMat past_0 = pnl_mat_wrap_mat_rows(path, 0, i - 1);
        PnlMat past_i = pnl_mat_wrap_mat_rows(path, 0, i);
        monteCarlo->delta(&past_0, (i - 1) * monteCarlo->opt_->maturity() / H, delta_0);
        monteCarlo->delta(&past_i, i * monteCarlo->opt_->maturity() / H, delta_i);
        pnl_vect_minus_vect(delta_i, delta_0);
        pnl_mat_get_row(path_i, path, i);
        LET(result, i) = LET(result, i - 1) * exp(monteCarlo->mod_->r_ * monteCarlo->opt_->maturity() / H) -
                         (pnl_vect_scalar_prod(delta_i, path_i));
    }
    double pAngLResult = GET(result, H) + pnl_vect_scalar_prod(delta_i, path_i);

    /// Free the memory
    pnl_vect_free(&delta_0);
    pnl_vect_free(&delta_i);
    pnl_vect_free(&path_i);

}