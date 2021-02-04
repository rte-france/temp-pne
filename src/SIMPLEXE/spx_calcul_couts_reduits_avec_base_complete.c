// Copyright (C) 2007-2018, RTE (http://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: Apache-2.0

/***********************************************************************

   FONCTION: Calcul des couts reduits des variables hors base 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_define.h"
# include "spx_fonctions.h"

/*----------------------------------------------------------------------------*/

void SPX_CalculerLesCoutsReduitsAvecBaseComplete( PROBLEME_SPX * Spx )
{
int Var; int il; int ilMax; double S; int i; int * NumerosDesVariablesHorsBase;
int * Cdeb; int * CNbTerm; int * NumeroDeContrainte; double * Pi; double * ACol;
double * C; double * CBarre; 

NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
Pi      = Spx->Pi;
ACol    = Spx->ACol;
CBarre  = Spx->CBarre;
C       = Spx->C;
NumeroDeContrainte = Spx->NumeroDeContrainte;

/* Boucle sur les variables hors base */

for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
  Var = NumerosDesVariablesHorsBase[i];
  /* Utilisation du chainage de la transposee de la matrice 
     des contraintes */
  il    = Cdeb[Var];
  ilMax = il + CNbTerm[Var];
  S     = 0.;
  while ( il < ilMax ) {
    S += Pi[NumeroDeContrainte[il]] * ACol[il];
    il++;
  }
  CBarre[Var] = C[Var] - S;	
}

Spx->CBarreAEteCalculeParMiseAJour = NON_SPX;

return;
}

/*----------------------------------------------------------------------------*/