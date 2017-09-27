#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/// \brief Classe Option abstraite
class Option {
public:
    double T_; /// maturité
    int nbTimeSteps_; /// nombre de pas de temps de discrétisation
    int size_; /// dimension du modèle, redondant avec BlackScholesModel::size_
    PnlVect *payoffCoeff_; /// coefficient permettant le calcul du payoff

    /**
    * Constructeur de la classe
    *
    * @param[in] maturité
    * @param[in] nombre de pas de temps de discrétisation
    * @param[in] dimension du modèle, redondant avec BlackScholesModel::size_
    * @param[in] coefficient permettant le calcul du payoff
    */
    Option(double T_, int nbTimeSteps_, int size_, PnlVect *payoffCoeff_);

    /**
     * Destructeur de classe
     */
    virtual ~Option();

    /**
     * Calcule la valeur du payoff sur la trajectoire
     *
     * @param[in] path est une matrice de taille (N+1) x d
     * contenant une trajectoire du modèle telle que créée
     * par la fonction asset.
     * @return phi(trajectoire)
     */
    virtual double payoff(const PnlMat *path) = 0;

    double maturity();

    int nbTimeSteps();

    int size();

    PnlVect payoffCoeff();

    double payoffCoeff(int i);

};


