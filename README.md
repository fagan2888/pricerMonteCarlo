# pricerMonteCarlo
Pricer Monte Carlo pour options dans le cadre d'un projet Ensimag

Pour obtenir les exécutables:  
mkdir build  
cd build  
cmake -DCMAKE_PREFIX_PATH=/matieres/5MMPMP6/pnl -DCMAKE_CXX_FLAGS="-std=c++11" ..  
make

Pour exécuter tous les tests:  
cd build  
make check  
