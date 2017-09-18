#ifndef MC_PRICER_OPTIONASIAN_HPP
#define MC_PRICER_OPTIONASIAN_HPP

#include "Option.hpp"

class OptionAsian: public Option {
public:
    OptionAsian(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_, double strike_);

    virtual double payoff(const PnlMat *path);

    double strike();

protected:
    double strike_;
};

#endif //MC_PRICER_OPTIONASIAN_HPP
