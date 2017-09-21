#include "OptionAsian.hpp"

OptionAsian::OptionAsian(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_, double strike_) :
 Option(T_, nbTimeSteps_, size_, payoffCoeff_), strike_(strike_)
{
}

double OptionAsian::payoff(const PnlMat *path)
{
    double payoff = 0.0;
    double payoffTempo = 0.0;
    for (int i = 0; i < size(); i++)
	  {
        payoffTempo = 0.0;
        for (int j = 0; j <= nbTimeSteps(); j++)
		    {
            payoffTempo += MGET(path, j, i);
        }
        payoff += payoffCoeff(i) * payoffTempo / (nbTimeSteps() + 1);
    }
    payoff = payoff - strike();
    payoff = (payoff < 0)? 0 : payoff;
    return payoff;
}

double OptionAsian::strike()
{
    return this->strike_;
}
