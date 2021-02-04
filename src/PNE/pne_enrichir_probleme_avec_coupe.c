// Copyright (C) 2007-2018, RTE (http://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: Apache-2.0

/***********************************************************************

   FONCTION: Calcul des coupes 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"   

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_EnrichirLeProblemeCourantAvecUneCoupe( PROBLEME_PNE * Pne, 
                                                char     Type,
                                                int      NombreDeTermes,
					                                     	double   SecondMembre,
						                                    double   Distance,
						                                    double * Coefficient,
						                                    int *    IndicesDesVariables ) 
{
int i; 

/* Il faut d'abord stocker la coupe dans une structure temporaire qui sera exploitee pour la mise a 
   jour des coupes dans la partie branch and bound */
/* Allocations si necessaire */
if ( Pne->NombreDeCoupesCalculees == 0 ) {
  Pne->CoupesCalculees = ( COUPE_CALCULEE **) malloc( ( Pne->NombreDeCoupesCalculees + 1 ) * sizeof( void * ) );
}
else {
  Pne->CoupesCalculees = ( COUPE_CALCULEE **) realloc( Pne->CoupesCalculees, 
                                                        ( Pne->NombreDeCoupesCalculees + 1 ) * sizeof( void * ) );
}
if ( Pne->CoupesCalculees == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_EnrichirLeProblemeCourantAvecUneCoupe \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}

/* Stockage de la coupe */
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees] = ( COUPE_CALCULEE * ) malloc( sizeof( COUPE_CALCULEE ) );
if ( Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees] == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_CalculerUneGomoryEnVariablesMixtes \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}

Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->Type = Type;
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->IndexDansCliques = -1;
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->IndexDansCoupesDeProbing = -1;
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->IndexDansContraintesDeBorneVariable = -1;
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->IndexDansKNegligees = -1;
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->IndexDansGNegligees = -1;
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->NumeroDeLaContrainte = Pne->Coupes.NombreDeContraintes;
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->NombreDeTermes = NombreDeTermes;
 
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->Coefficient = (double *) malloc( NombreDeTermes * sizeof( double ) );
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->IndiceDeLaVariable = (int *) malloc( NombreDeTermes * sizeof( int   ) );
if ( Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->Coefficient == NULL || 
     Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->IndiceDeLaVariable == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_CalculerUneGomoryEnVariablesMixtes \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env, Pne->AnomalieDetectee ); 
}

for ( i = 0 ; i < NombreDeTermes ; i++ ) {
  Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->Coefficient[i] = Coefficient[i];
  Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->IndiceDeLaVariable[i] = IndicesDesVariables[i];
}

Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->SecondMembre = SecondMembre;
Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees]->Distance     = Distance;

Pne->NombreDeCoupesCalculees++;
  
Pne->NombreDeCoupesAjoute++;

return;
}
   


