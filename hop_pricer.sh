#!/bin/sh

echo "Pricer"
echo "Asian"
./build/pricer ./data/asian.dat
echo "Basket"
./build/pricer ./data/basket.dat
echo "Basket_1"
./build/pricer ./data/basket_1.dat
echo "Basket_2"
./build/pricer ./data/basket_2.dat
echo "Perf"
./build/pricer ./data/perf.dat
echo ""
echo "Pricer -c"
echo "Asian"
./build/pricer -c ./data/market-data/simul_asian.dat ./data/asian.dat
echo "Basket"
./build/pricer -c ./data/market-data/simul_basket.dat ./data/basket.dat
echo "Basket_1"
./build/pricer -c ./data/market-data/simul_basket_1.dat ./data/basket_1.dat
echo "Basket_2"
./build/pricer -c ./data/market-data/simul_basket_2.dat ./data/basket_2.dat
echo "Perf"
./build/pricer -c ./data/market-data/simul_perf.dat ./data/perf.dat
