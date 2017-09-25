//
// Created by fellajim on 9/22/17.
//

#include "HedgingPortfolio.hpp"
#include <cmath>
#include <iostream>

void HedgingPortfolio::hedgingPAndL(PnlVect* result,PnlMat* path){
    PnlVect* delta_0= pnl_vect_create(H);
    PnlVect* delta_i= pnl_vect_create(H);
    PnlMat* path_i= pnl_vect_create(H,monteCarlo->mod->size_);
    double prix,ic;
    monteCarlo->price(prix,ic);
    monteCarlo->delta(monteCarlo->mod_->spot,0,delta_0);
    pnl_mat_get_row(path_i,path,0);
    LET(result,0)=prix-pnl_vect_scalar_prod(delta_0,path_i);
    for (int i=1 ; i<H+1 ; i++) {
        PnlMat past_0=pnl_mat_wrap_mat_rows(path,0,i-1);
        PnlMat past_i=pnl_mat_wrap_mat_rows(path,0,i);
        monteCarlo->delta(&past_0,(i-1)*T/H,delta_0);
        monteCarlo->delta(&past_i,i*T/H,delta_i);
        pnl_vect_minus_vect(delta_i,delta_0);
        pnl_mat_get_row(path_i,path,i);
        LET(result,i)=LET(result,i-1)*exp(monteCarlo->mod_->r_*monteCarlo->opt_->maturity()/H )-(pnl_vect_scalar_prod(delta_i,path_i));
    }
    pnl_mat_get_row(delta_i,deltas,H);
    pnl_mat_get_row(path_i,path,H);
    double pAngLResult=GET(result,H)+pnl_vect_scalar_prod(delta_i,path_i);

}