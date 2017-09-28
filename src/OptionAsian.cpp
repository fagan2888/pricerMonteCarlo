#include "OptionAsian.hpp"

OptionAsian::OptionAsian(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_, double strike_) :
        Option(T_, nbTimeSteps_, size_, payoffCoeff_), strike_(strike_) {
}

double OptionAsian::payoff(const PnlMat *path) {
    double payoff = 0.0;
    double payoffTempo = 0.0;
    for (int d = 0; d < size_; d++) {
        payoffTempo = 0.0;
        for (int j = 0; j <= nbTimeSteps_; j++) {
            payoffTempo += MGET(path, j, d);
        }
        payoff += GET(payoffCoeff_,d) * payoffTempo / (nbTimeSteps_ + 1);
    }
    payoff = payoff - strike_;
    payoff = (payoff < 0) ? 0 : payoff;
    return payoff;
}

double OptionAsian::strike() {
    return this->strike_;
}
