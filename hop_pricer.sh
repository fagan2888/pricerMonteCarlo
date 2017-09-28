#!/bin/sh

echo "Pricer"
echo "Asian"
time ./build/pricer ./data/asian.dat
echo "Basket"
time ./build/pricer ./data/basket.dat
echo "Basket_1"
time ./build/pricer ./data/basket_1.dat
echo "Basket_2"
time ./build/pricer ./data/basket_2.dat
echo "Perf"
time ./build/pricer ./data/perf.dat
echo ""
echo "Pricer -c"
echo "Asian"
time ./build/pricer -c ./data/market-data/simul_asian.dat ./data/asian.dat
echo "Basket"
time ./build/pricer -c ./data/market-data/simul_basket.dat ./data/basket.dat
echo "Basket_1"
time ./build/pricer -c ./data/market-data/simul_basket_1.dat ./data/basket_1.dat
echo "Basket_2"
time ./build/pricer -c ./data/market-data/simul_basket_2.dat ./data/basket_2.dat
echo "Perf"
time ./build/pricer -c ./data/market-data/simul_perf.dat ./data/perf.dat
