# pricerMonteCarlo
Outil de valorisation et de couverture de produits financiers par une méthode de Monte Carlo dans le modèle de Black-Scholes multidimensionnel. Le travail a été réalisé dans le cadre d'un projet de groupe de l'Ensimag avec Mohammed, Edouard, Ayoub et moi-même.

Pour obtenir les exécutables:  
mkdir build  
cd build  
cmake -DCMAKE_PREFIX_PATH=/matieres/5MMPMP6/pnl -DCMAKE_CXX_FLAGS="-std=c++11" ..  
make

Pour exécuter tous les tests:  
cd build  
make check  
