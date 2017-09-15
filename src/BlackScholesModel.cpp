//
// Created by fellajim on 9/15/17.
//

#include "BlackScholesModel.hpp"
#include <math.h>

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot){
    size_=size;
    this.r=r;
    rho_=rho;
    sigma_=sigma;
    spot_=spot;
}

void BlackScholesModel::asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) {
    /* Gaussian vector*/
    PnlVect* G=pnl_vect_create(size_);
    pnl_vect_rng_normal(G,rng);
    /*Cholesky*/
    PnlMat* gamma=pnl_mat_create_from_scalar(size_,size_,rho_);
    PnlMat* identity=pnl_mat_create(size_,size_);
    pnl_mat_set_id(identity);
    pnl_mat_mult_scalar(identity,1-rho_);
    pnl_mat_plus_mat(gamma,identity);
    pnl_mat_chol(gamma);
    PnlMat* results=pnl_mat_create(nbTimeSteps+1,size_);
    pnl_mat_set_row(path,spot_,0);
    for (auto i = 1; i <= nbTimeSteps;i++) {
        for (auto d=0; d < size_ ; d++) {
            double sigma_d=pnl_vect_get(sigma_,d);
            PnlVect* L_d=pnl_vect_create(size_);
            pnl_mat_get_row(L_d,gamma,d);
            PnlVect* G_i==pnl_vect_create(size_);
            pnl_vect_rng_normal(G_i,size_,rng);
            double path_t_d=pnl_mat_get(path,i-1,d)*exp((r-(sigma_d*sigma_d)/2)*T/nbTimeSteps+sigma_d*sqrt(T/nbTimeSteps)*pnl_vect_scalar_prod(L_d,G_i));
            pnl_mat_set(path,i,d,path_t_d);
        }
    }
}