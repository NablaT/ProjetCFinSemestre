#include "../machine.h"

//! Premier exemple de segment de texte.
/*!
 * \note On voit ici l'intérêt de l'initialisation d'une union avec nommage
 * explicite des champs.
 */

Instruction text[] = {
//   type		            cop  imm	 ind    regcond operand
//-------------------------------------------------------------
    {.instr_generic   = {   LOAD,   true, false,    4,  8}},
    {.instr_generic   = {   LOAD,   true, false,    5,  12}},
    {.instr_immediate = {   STORE,  false, false,    5,  10 }},
    {.instr_immediate = {   PUSH,   true, false, 	0, 	1	}},  // 0
    {.instr_immediate = {   PUSH,   true, false, 	0, 	4	}},  // 1
    {.instr_generic =   {   NOP,                 }},  // 5
    {.instr_generic =   {   NOP,                 }},  // 5
    {.instr_immediate = {   POP,    false, false,  0,  1   }},  // 1
    {.instr_immediate = {   LOAD,   true,  false, 4,  10  }},
    {.instr_indexed =   {   ADD,    false, true,  0,  4, +2   }},  // 1
    {.instr_generic =   {   HALT,					}},  // 5
    {.instr_generic =   {   ILLOP,					}},  // 5
    {.instr_generic =   {   ILLOP,					}},  // 5
    {.instr_generic =   {   ILLOP,					}},  // 5
};

//! Taille utile du programme
const unsigned textsize = sizeof(text) / sizeof(Instruction);

//! Premier exemple de segment de données initial
Word data[20] = {
    10, // 0: premier opérande
    5,  // 1: second opérande
    20, // 2: résultat
    0,  // 3
};

//! Fin de la zone de données utiles
const unsigned dataend = 5;

//! Taille utile du segment de données
const unsigned datasize = sizeof(data) / sizeof(Word);  

