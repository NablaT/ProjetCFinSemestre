#include "instruction.h"
#include "stdio.h"
#include <string.h>

//! Forme imprimable des codes opérations
/*extern*/ const char *cop_names[] = {"ILLOP", "NOP", "LOAD", "STORE", "ADD", "SUB", "BRANCH", "CALL", "RET", "PUSH", "POP", "HALT"};

//! Forme imprimable des conditions
/*extern*/ const char *condition_names[] = { "NC", "EQ", "NE", "GT", "GE", "LT", "LE" };

//! Impression d'une instruction sous forme lisible (désassemblage)
/*!
 * \param instr l'instruction à imprimer
 * \param addr son adresse
 */
void print_instruction(Instruction instr, unsigned addr)
{
	//Affichage du nom d'instruction : récupération de la position dans le tableau 
	//cop_names dans la structure générique du header
	printf("%s ", cop_names[instr.instr_generic._cop] );
	 
	// Selon l'instruction qui va être execute, le terme suivant sera une condition ou un numero de registre ou...rien.
	switch (instr.instr_generic._cop) {

		// Dans ces cas là, c'est une condition, donc affichage de la condition via le tab condition_names
		case BRANCH:
		case CALL:
			printf("%s, ", condition_names[instr.instr_generic._regcond] ); 
			break;

		// Dans ces cas là, c'est un registre, donc faut l'afficher proprement...
		case LOAD:
		case STORE:
		case ADD:
		case SUB:
				//%02d means "format the integer with 2 digits, left padding it with zeroes"
				printf("R%02d, ", instr.instr_generic._regcond);
			break;

		default:
			break;
	}

	// Affichage des opérandes
	switch (instr.instr_generic._cop)
	{
		case HALT:
		case RET:
		case ILLOP:
		case NOP:
			// Dans ces cas là, pas d'opérandes
			break;

		default:

			if (instr.instr_generic._immediate) 
			{ // Si I = 1 , l'opérande est une valeur immédiate
				printf("#%d", instr.instr_immediate._value);
			} 
			else 
			{				
				if (instr.instr_generic._indexed) 
				{ 
				// Si I = 0 et X = 1, l'opérande est une adresse relative indexée sur un registre
				// En deux parties : un numéro de registre et un offset
					printf("%d[R%02d]", instr.instr_indexed._offset, instr.instr_indexed._rindex); // crochets pour accès mémoire indirect
				}
				else
				{ // Si I = 0 et X = 0, L'opérande est une adresse absolue
					printf("@0x%04x", instr.instr_absolute._address); // %x -> hexadecimal	
				}
			}
		break;
	}
}