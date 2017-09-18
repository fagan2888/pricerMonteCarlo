#ifndef MC_PRICER_OPTIONPERFORMANCE_HPP
#define MC_PRICER_OPTIONPERFORMANCE_HPP

#include "Option.hpp"

class OptionPerformance : public Option {
public:
    OptionPerformance(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_);

    virtual double payoff(const PnlMat *path);
};

#endif //MC_PRICER_OPTIONPERFORMANCE_HPP
