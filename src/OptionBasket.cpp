#include "OptionBasket.hpp"

OptionBasket::OptionBasket(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_, double strike_) : Option(T_, nbTimeSteps_, size_, payoffCoeff_),
                                                                                    strike_(strike_) {}

double OptionBasket::payoff(const PnlMat *path) {
    double payoff = 0;

    for(int i = 0; i<2; i++) {
        //payoff += path.array[path.m,i] * payoffCoeff()
    }

    payoff = payoff - strike();
    payoff = (payoff < 0)? 0 : payoff;
    return payoff;
}

double OptionBasket::strike() {
    return this->strike_;
}