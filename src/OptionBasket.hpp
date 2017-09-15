#ifndef MC_PRICER_OPTIONBASKET_HPP
#define MC_PRICER_OPTIONBASKET_HPP

#include "Option.hpp"

class OptionBasket : public Option {
public:
    OptionBasket(double T_, int nbTimeSteps_, int size_, double strike);
    virtual double payoff(const PnlMat *path);

protected:
    double strike;
};

#endif //MC_PRICER_OPTIONBASKET_HPP
