/*!
 * \file debug.h
 * \brief Fonctions de mise au point interactive.
 */
#include <stdbool.h>
#include "machine.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
//! Dialogue de mise au point interactive pour l'instruction courante.
/*!
 * Cette fonction gère le dialogue pour l'option \c -d (debug). Dans ce mode,
 * elle est invoquée après l'exécution de chaque instruction.  Elle affiche le
 * menu de mise au point et on exécute le choix de l'utilisateur. Si cette
 * fonction retourne faux, on abandonne le mode de mise au point interactive
 * pour les instructions suivantes et jusqu'à la fin du programme.
 * 
 * \param mach la machine/programme en cours de simulation
 * \return vrai si l'on doit continuer en mode debug, faux sinon
 */
 bool debug_ask(Machine *pmach)
 {

 	char* choix = malloc(20*sizeof(char));

 	while(true)
 	{
		printf("DEBUG?");
		scanf("%s", choix);



		printf("\n");
		switch(choix[0]) // RET sert à rien ?
		{
			case 'h':

				printf(
					"h\t"	"help\n"
					"c\t"	"continue (exit interactive debug mode)\n"
					"s\t"	"step by step (next instruction)\n"
					"RET\t"	"[ INUTILE ]\n"
					"r\t"	"print registers\n"
					"d\t"	"print data memory\n"
					"t\t"	"print text (program) memory\n"
					"p\t"	"print text (program) memory\n"
					"m\t"	"print registers and data memory\n"
					);
				break;

			case 'c':
				return false;
				break;
			case 's':
				return true;
				//simul(pmach, true);
				break;
			case 'r': // Affichage des registres
				print_cpu(pmach);
				break;
			case 'd':
				print_data(pmach);
				break;
			case 't':
			case 'p':
				print_program(pmach);
				break;
			case 'm':
				print_cpu(pmach);
				print_data(pmach);
				break;
			default:
				//silence
				break;

			}
	}

 }
