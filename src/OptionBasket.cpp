#include "OptionBasket.hpp"

OptionBasket::OptionBasket(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_, double strike_) : Option(T_,
                                                                                                                   nbTimeSteps_,
                                                                                                                   size_,
                                                                                                                   payoffCoeff_),
                                                                                                            strike_(strike_) {}

double OptionBasket::payoff(const PnlMat *path) {
    double payoff = 0;
    for (int d = 0; d < size(); d++) {
        payoff += MGET(path, nbTimeSteps(), d) * payoffCoeff(d);
    }
    payoff = payoff - strike();
    payoff = (payoff < 0) ? 0 : payoff;
    return payoff;
}

double OptionBasket::strike() {
    return this->strike_;
}
