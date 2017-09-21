#include "OptionPerformance.hpp"

OptionPerformance::OptionPerformance(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_) : Option(T_,
                                                                                                             nbTimeSteps_,
                                                                                                             size_,
                                                                                                             payoffCoeff_) {}

double OptionPerformance::payoff(const PnlMat *path) {
    double payoff = 1;
    double before = 0;
    double now = 0;
    for (int i = 1; i < nbTimeSteps()+1; i++) {
        before = 0;
        now = 0;
        for (int j = 0; j < size(); j++) {
            before += payoffCoeff(j) * MGET(path, i - 1, j);
            now += payoffCoeff(j) * MGET(path, i, j);
        }
        payoff += (now / before - 1 < 0) ? 0 : now / before - 1;
    }
    return payoff;
}