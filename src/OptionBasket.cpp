#include "OptionBasket.hpp"

OptionBasket::OptionBasket(double T_, int nbTimeSteps_, int size_, double strike) : Option(T_, nbTimeSteps_, size_),
                                                                                    strike(strike) {}

double OptionBasket::payoff(const PnlMat *path) {
    double payoff = 0;




    return payoff;
}