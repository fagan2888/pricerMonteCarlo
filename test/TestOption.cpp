#include "../src/OptionBasket.hpp"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

    double maturity = 4;
    int nbTimeSteps = 1;
    int size = 1;
    double strike = 10;

    /// Test sur OptionBasket
    OptionBasket optionBasket(maturity, nbTimeSteps, size, strike);
    cout << (optionBasket.strike() == 10) << endl;

    return 0;
}