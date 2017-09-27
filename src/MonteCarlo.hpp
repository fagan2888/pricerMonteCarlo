#pragma once

#include "Option.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"

class MonteCarlo {
public:
    BlackScholesModel *mod_; /*! pointeur vers le modèle */
    Option *opt_; /*! pointeur sur l'option */
    PnlRng *rng_; /*! pointeur sur le générateur */
    double fdStep_; /*! pas de différence finie */
    int nbSamples_; /*! nombre de tirages Monte Carlo */

    /**
     * Constructeur de la classe
     *
     * @param[in] modele Black Scholes
     * @param[in] l'option
     * @param[in] le générateur
     * @param[in] pas de différence finie
     * @param[in] nombre de tirages Monte Carlo
     */
    MonteCarlo(BlackScholesModel *mod, Option *opt, PnlRng *rng, double fdStep, int nbSamples);

    /**
    * Destructeur de classe
    */
    ~MonteCarlo();

    /**
    * Calcule le prix de l'option à la date 0
    *
    * @param[out] prix valeur de l'estimateur Monte Carlo
    * @param[out] ic largeur de l'intervalle de confiance
    */
    void price(double &prix, double &ic);

    /**
     * Calcule le prix de l'option à la date t
     *
     * @param[in]  past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] prix contient le prix
     * @param[out] ic contient la largeur de l'intervalle
     * de confiance sur le calcul du prix
     */
    void price(const PnlMat *past, double t, double &prix, double &ic);

    /**
     * Calcule le delta de l'option à la date t
     *
     * @param[in] past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] delta contient le vecteur de delta
     * de confiance sur le calcul du delta
     */
    void delta(const PnlMat *past, double t, PnlVect *delta);

    /**
     * Calcule le profit&loss du portefeuille de couverture
     *
     * @param[out] result contient les profit&loss du portefeuille au cours de la trajectoire
     * @param[in] path contient la trajectoire des sous-jacents
     * @return le profit&loss final du portefeuille de couverture
     */
    double hedgingPAndL(PnlVect *result, PnlMat *path, int H);
};
