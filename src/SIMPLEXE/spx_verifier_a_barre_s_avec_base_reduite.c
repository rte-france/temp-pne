// Copyright (C) 2007-2018, RTE (http://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: Apache-2.0

/***********************************************************************

   FONCTION: Verification de A_BARRE_S = B-1 * AS
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h" 

# include "pne_fonctions.h"

# include "lu_define.h"
# include "lu_fonctions.h"

# define SEUIL_DE_VERIFICATION_DE_ABarreS_1    1.e-6 /*1.e-7*/   
# define SEUIL_DE_VERIFICATION_DE_ABarreS_2    1.e-6 /*1.e-7*/   
# define NOMBRE_DE_VERIFICATIONS               100 /*100*/

# define NBITER_RAFFINEMENT 1

# define VERBOSE_SPX 0

/*----------------------------------------------------------------------------*/
/*            Verification du calcul de ABarreS pour savoir s'il faut
              refactoriser la base: on verifie sur k contraintes tirees
              au hasard  		         			      */

void SPX_VerifierABarreSAvecBaseReduite( PROBLEME_SPX * Spx )
{
int il; int ilMax; int Cnt; int Var; double X; int NbFois; int Nombre; int i;   
int NombreDeVerifications; double * Bs; int * CdebProblemeReduit; int * CNbTermProblemeReduit;
int * IndicesDeLigneDesTermesDuProblemeReduit; double * ValeurDesTermesDesColonnesDuProblemeReduit;
int * Mdeb; int * NbTerm; int * Indcol; double * A; int LimiteContraintes;
int * ContrainteDeLaVariableEnBase; double * ABarreS; int NmX; double Ecart; char Imprecision;
int * CntDeABarreSNonNuls; int j; int ic; int icMax; int * VariableEnBaseDeLaContrainte;
double * ErBMoinsUn; char * PositionDeLaVariable; int RangDeLaMatriceFactorisee;
int * OrdreColonneDeLaBaseFactorisee; int rr; int * LigneDeLaBaseFactorisee; int r;
int * ColonneDeLaBaseFactorisee;

Imprecision = NON_SPX;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;
OrdreColonneDeLaBaseFactorisee = Spx->OrdreColonneDeLaBaseFactorisee;
ColonneDeLaBaseFactorisee = Spx->ColonneDeLaBaseFactorisee;
LigneDeLaBaseFactorisee = Spx->LigneDeLaBaseFactorisee;

LimiteContraintes = RangDeLaMatriceFactorisee;

A  = Spx->A;
Bs = Spx->Bs;
ABarreS = Spx->ABarreS;
if ( Spx->TypeDeStockageDeABarreS == COMPACT_SPX ) {
  ErBMoinsUn = Spx->ErBMoinsUn;
  for ( i = 0 ; i < LimiteContraintes ; i++ ) ErBMoinsUn[i]= 0;	
	
  CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;	
  for ( j = 0 ; j < Spx->NbABarreSNonNuls ; j++ ) ErBMoinsUn[CntDeABarreSNonNuls[j]] = ABarreS[j];	
	ABarreS = ErBMoinsUn;	
}

CdebProblemeReduit = Spx->CdebProblemeReduit;
CNbTermProblemeReduit = Spx->CNbTermProblemeReduit;
IndicesDeLigneDesTermesDuProblemeReduit = Spx->IndicesDeLigneDesTermesDuProblemeReduit;
ValeurDesTermesDesColonnesDuProblemeReduit = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;

Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
PositionDeLaVariable = Spx->PositionDeLaVariable;

NombreDeVerifications = NOMBRE_DE_VERIFICATIONS;

NmX = (int) ceil( 0.5 * LimiteContraintes );
if ( NombreDeVerifications > NmX ) NombreDeVerifications = NmX;

/* On tire un nombre au hasard le nombre de verifications qu'on va faire */
  # if UTILISER_PNE_RAND == OUI_SPX
    Spx->A1 = PNE_Rand( Spx->A1 );
    X = Spx->A1 * (float) NombreDeVerifications;
	# else	
    X = rand() * Spx->UnSurRAND_MAX * (float) NombreDeVerifications;
  # endif

NombreDeVerifications = (int) X;
if ( NombreDeVerifications <= 0 ) NombreDeVerifications = 1;

Var = Spx->VariableEntrante;
ic    = CdebProblemeReduit[Var];
icMax = ic + CNbTermProblemeReduit[Var];
while ( ic < icMax ) {
  Bs[IndicesDeLigneDesTermesDuProblemeReduit[ic]] = -ValeurDesTermesDesColonnesDuProblemeReduit[ic];
  ic++; 
}

NbFois = 0;
while ( NbFois < NombreDeVerifications ) { 
  /* On tire un nombre au hasard compris entre 0 et LimiteContraintes - 1 */
  # if UTILISER_PNE_RAND == OUI_SPX
    Spx->A1 = PNE_Rand( Spx->A1 );
    X = Spx->A1 * (LimiteContraintes - 1);		
	# else		
    X = rand() * Spx->UnSurRAND_MAX * (LimiteContraintes - 1);		
  # endif
	
  Nombre = (int) X;	
  if ( Nombre >= LimiteContraintes - 1 ) Nombre = LimiteContraintes - 1; 
  rr = Nombre;	

  X = Bs[rr];
  /* Verification */
	Cnt = LigneDeLaBaseFactorisee[rr];
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
	  Var = Indcol[il];
    if ( PositionDeLaVariable[Var] != EN_BASE_LIBRE ) goto Next_il_0;
		r = OrdreColonneDeLaBaseFactorisee[ContrainteDeLaVariableEnBase[Var]];
		if ( r >= RangDeLaMatriceFactorisee ) goto Next_il_0; /* Normalement ca n'arrive pas */ 
    X += A[il] * ABarreS[r];	  
	  Next_il_0:
    il++;
  }
  if ( fabs( X ) > SEUIL_DE_VERIFICATION_DE_ABarreS_1 ) {
	  Imprecision = OUI_SPX;
    if ( Spx->NombreDeChangementsDeBase < 10 ) { 
	    /* Si ca se produit dans les premieres iterations apres une factorisation */
	    Spx->FlagStabiliteDeLaFactorisation = 1;						
	  }
	  break;
  } 
  NbFois++;
}

if ( Imprecision == OUI_SPX ) {
  /* Suspiscion: on recalcule l'ecart moyen pour voir s'il faut refactoriser */
  VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;

  for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {		
    Var   = VariableEnBaseDeLaContrainte[ColonneDeLaBaseFactorisee[r]];
	  X     = ABarreS[r];
    ic    = CdebProblemeReduit[Var];
    icMax = ic + CNbTermProblemeReduit[Var];
    while ( ic < icMax ) {
	    Bs[IndicesDeLigneDesTermesDuProblemeReduit[ic]] += ValeurDesTermesDesColonnesDuProblemeReduit[ic] * X;
      ic++;
    }
		
  }

  Ecart = 0.0;
  for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {		
    Ecart += fabs( Bs[r] );
	}
  Ecart/= RangDeLaMatriceFactorisee;
	
  if ( Ecart > SEUIL_DE_VERIFICATION_DE_ABarreS_2 && Spx->NombreDeChangementsDeBase > 0 ) {  
    #if VERBOSE_SPX
      printf("SPX_VerifierABarreS Iteration %d erreur de resolution sur ABarreS: %e ",Spx->Iteration,Ecart); 
      printf(" ecart trop grand on refactorise la base\n"); 
    #endif				
    Spx->FactoriserLaBase = OUI_SPX;
		/*Spx->FaireDuRaffinementIteratif = NBITER_RAFFINEMENT;*/
		/* On augmente le seuil dual de pivotage */
 	  Spx->SeuilDePivotDual = COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * VALEUR_DE_PIVOT_ACCEPTABLE;	

    Spx->FaireChangementDeBase = NON_SPX;
		
	}	
	else {
	  Spx->FaireDuRaffinementIteratif = NBITER_RAFFINEMENT;				
    Spx->FaireChangementDeBase = NON_SPX;
 	  Spx->SeuilDePivotDual = COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * VALEUR_DE_PIVOT_ACCEPTABLE;		
	}
}

for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) Bs[r] = 0;

return;
}

