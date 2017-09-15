#include "Option.hpp"

Option::Option(double T_, int nbTimeSteps_, int size_) : T_(T_), nbTimeSteps_(nbTimeSteps_), size_(size_) {}

double Option::maturity() {
    return this->T_;
}

int Option::nbTimeSteps() {
    return this->nbTimeSteps_;
}

int Option::size() {
    return size_;
}