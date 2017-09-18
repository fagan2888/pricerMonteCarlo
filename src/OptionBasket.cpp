#include "OptionBasket.hpp"

OptionBasket::OptionBasket(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_, double strike_) : Option(T_, nbTimeSteps_, size_, payoffCoeff_),
                                                                                    strike_(strike_) {}

double OptionBasket::payoff(const PnlMat *path) {
    double payoff = 0;
    PnlVect* S_T=pnl_vect_create(size_);
    pnl_mat_get_row(S_T,path,nbTimeSteps_);
    for(int i = 0; i<size_; i++) {
        payoff+=pnl_vect_get(payoffCoeff_,i)*pnl_vect_get(S_T,i)-strike_;
        //payoff += path.array[path.m,i] * payoffCoeff()
    }

    payoff = payoff - strike();
    payoff = (payoff < 0)? 0 : payoff;
    return payoff;
}

double OptionBasket::strike() {
    return this->strike_;
}