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
	printf("ERROR: ");
	switch(err){
		case ERR_NOERROR:
			printf("Pas d'erreur à l'adresse 0x%08x\n", addr);
			break;
		case ERR_UNKNOWN: //L'instruction n'est pas reconnue.
			printf("Instruction inconnue à l'adresse 0x%08x\n", addr);
			exit(1);
		case ERR_ILLEGAL: //L'instruction est une opération illégale. 
			printf("Instruction illégale à l'adresse 0x%08x\n", addr);
			exit(1);
		case ERR_CONDITION://la code condition(CC) est illégale.
			printf("Condition illégale à l'adresse 0x%08x\n", addr);
			exit(1);
		case ERR_IMMEDIATE://la valeur des indicateurs(I et X) est illégale.
			printf("Valeur immédiate interdite à l'adresse 0x%08x\n", addr);
			exit(1);
		case ERR_SEGTEXT://Si on fait beaucoup de execution. Les textes débordent la segment de text. 
			printf("Erreur de segmentation : Violation de taille du segment de texte à l'adresse 0x%08x\n", addr);
			exit(1);
		case ERR_SEGDATA://Si on a beaucoup de données. La segment de données déborde dans la segment de pile.
			printf("Erreur de segmentation : Violation de taille du segment de données à l'adresse 0x%08x\n", addr);
			exit(1);
		case ERR_SEGSTACK://Si on empile beaucoup. La segment de pile déborde dans la segment de donnée. 
			printf("Erreur de segmentation : Violation de taille du segment de pile à l'adresse 0x%08x\n", addr);
			exit(1);
		default:
			exit(0);
	}
	exit(0);
}


//! Affichage d'un avertissement
/*!
 * \param warn code de l'avertissement
 * \param addr adresse de l'erreur
 */
void warning(Warning warn, unsigned addr){
	printf("WARNING: Program fini correctement au \tat 0x%08x\n",addr);
}
