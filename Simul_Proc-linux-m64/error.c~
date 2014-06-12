/***** error.c *****/

#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/*
    ERR_NOERROR = 0,	//!< Pas d'erreur
    ERR_UNKNOWN, 	//!< Instruction inconnue
    ERR_ILLEGAL,	//!< Instruction illégale
    ERR_CONDITION,	//!< Condition illégale
    ERR_IMMEDIATE,	//!< Valeur immédiate interdite
    ERR_SEGTEXT,	//!< Violation de taille du segment de texte
    ERR_SEGDATA,	//!< Violation de taille du segment de données
    ERR_SEGSTACK,	//!< Violation de taille du segment de pile

*/
//! Affichage d'une erreur et fin du simulateur
/*!
 * \note Toutes les erreurs étant fatales on ne revient jamais de cette
 * fonction. L'attribut \a noreturn est une extension (non standard) de GNU C
 * qui indique ce fait.
 * 
 * \param err code de l'erreur
 * \param addr adresse de l'erreur
 */
void error(Error err, unsigned addr){
	printf("Les erreurs:\n");
	switch(err){
		case ERR_NOERROR:
			printf("Pas d'erreur au \t%08x\n", addr);
			break;
		case ERR_UNKNOWN:
			printf("Instruction inconnue au \t%08x\n", addr);
			exit(1);
		case ERR_ILLEGAL:
			printf("Instruction illégale au \t%08x\n", addr);
			exit(1);
		case ERR_CONDITION:
			printf("Condition illégale au \t%08x\n", addr);
			exit(1);
		case ERR_IMMEDIATE:
			printf("Valeur immédiate interdite au \t%08x\n", addr);
			exit(1);
		case ERR_SEGTEXT:
			printf("Violation de taille du segment de texte au \t%08x\n", addr);
			exit(1);
		case ERR_SEGDATA:
			printf("Violation de taille du segment de données au \t%08x\n", addr);
			exit(1);
		case ERR_SEGSTACK:
			printf("Violation de taille du segment de pile au \t%08x\n", addr);
			exit(1);
		default:
			exit(0);
	}
}


//! Affichage d'un avertissement
/*!
 * \param warn code de l'avertissement
 * \param addr adresse de l'erreur
 */
void warning(Warning warn, unsigned addr){
	printf("WARNING: Program fini correctement au \tat 0x%08x\n",addr);
}
