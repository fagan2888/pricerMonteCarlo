#include "Option.hpp"

Option::Option(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_) : T_(T_), nbTimeSteps_(nbTimeSteps_),
                                                                                size_(size_),
                                                                                payoffCoeff_(payoffCoeff_) {}

double Option::maturity() {
    return this->T_;
}

int Option::nbTimeSteps() {
    return this->nbTimeSteps_;
}

int Option::size() {
    return size_;
}

PnlVect Option::payoffCoeff() {
    return *(this->payoffCoeff_);
}

double Option::payoffCoeff(int i) {
    return GET(this->payoffCoeff_, i);
}