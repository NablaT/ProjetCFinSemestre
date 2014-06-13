#include <stdio.h>
#include "machine.h"
#include "error.h"

int i = 0;

void stack_data(Machine *pmach, int data);
int  pop_data(Machine *pmach);
void check_data_address(Machine *pmach, int addr);

void process_load(Machine *pmach, Instruction instr);
void process_store(Machine *pmach, Instruction instr);
void process_add(Machine *pmach, Instruction instr);
void process_sub(Machine *pmach, Instruction instr);

void process_branch(Machine *pmach, Instruction instr);
void process_call(Machine *pmach, Instruction instr);
void process_ret(Machine *pmach, Instruction instr);
void process_push(Machine *pmach, Instruction instr);
void process_pop(Machine *pmach, Instruction instr);

void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr);

//! Décodage et exécution d'une instruction
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return faux après l'exécution de \c HALT ; vrai sinon
 */
bool decode_execute(Machine *pmach, Instruction instr) {
	int pc = pmach->_pc - 1;
	// Brace yourselves !
	switch (instr.instr_generic._cop) {
		case ILLOP: error(ERR_ILLEGAL, pc); // arret intempestif
		case NOP: break;

		case LOAD: process_load(pmach, instr); break;
		case STORE: process_store(pmach, instr); break;
		case ADD: process_add(pmach, instr); break;
		case SUB: process_sub(pmach, instr); break;

		case BRANCH: process_branch(pmach, instr); break;
		case CALL: process_call(pmach, instr); break;
		case RET: process_ret(pmach, instr); break;
		case PUSH: process_push(pmach, instr); break;
		case POP: process_pop(pmach, instr); break;

		case HALT: 
			warning(WARN_HALT, pmach->_pc-1);
			return false;
		
		default: error(ERR_UNKNOWN, pc);
	}
	return true;
}

//! Trace de l'exécution
/*!
 * On écrit l'adresse et l'instruction sous forme lisible.
 *
 * \param msg le message de trace
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \param addr son adresse
 */
void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr) {
	printf("TRACE: %s: 0x%04x: ", msg, addr);
	print_instruction(instr, addr);
	printf("\n");
}

/*
 *		METHODES UTILES
 * 
 */

/*!
 * Vérifie que le pointeur de la pile ne va pas causer d'erreur de segmentation.
 * Lance une erreur ERR_SEGSTACK si c'est le cas.
 *
 * \param pmach Machine sur laquelle effectuer la vérification
 */
void stack_validation(Machine *pmach) {
	// Check the stack index
	if (pmach->_sp >= pmach->_datasize || pmach->_sp < pmach->_dataend) {
		error(ERR_SEGSTACK, pmach->_pc-1);
	}
}

/*!
 * Empile une valeur dans la pile d'une Machine.
 *
 * \param pmach Machine dans laquelle effectuer l'opération
 * \param data Valeur à empiler
 */
void stack_data(Machine *pmach, int data) {
	pmach->_data[(pmach->_sp)--] = data;
	stack_validation(pmach);
}

/*!
 * Dépile la valeur au sommet de la pile d'une Machine.
 * Vérifie aussi les problèmes de segmentation de pile.
 *
 * \param pmach Machine dans laquelle effectuer l'opération
 * \return La valeur au sommet de la pile
 */
int pop_data(Machine *pmach) {
	int data = pmach->_data[++(pmach->_sp)];
	stack_validation(pmach);
	return data;
}

/*!
 * Vérifie si une condition est vrai ou pas.
 * Lance une erreur si le code de la condition n'est pas reconnu.

 * \param pmach Machine dans laquelle effectuer l'opération
 * \param instr Instruction contenant la condition à vérifier
 * \return Vrai si la condition est respectée ; faux sinon
 */
bool check_condition(Machine *pmach, Instruction instr) {
	switch (instr.instr_generic._regcond) {
		case NC: return true;

		case EQ: return pmach->_cc == CC_Z;
		case NE: return pmach->_cc != CC_Z;

		case GT: return pmach->_cc == CC_P;
		case GE: return pmach->_cc != CC_N;

		case LT: return pmach->_cc == CC_N;
		case LE: return pmach->_cc != CC_P;
		default: error(ERR_CONDITION, pmach->_pc-1);
	}
	return false;
}

/*!
 * Met à jour la valeur de CC (pour les conditions) dans une Machine.
 *
 * \param pmach Machine dans laquelle effectuer l'opération
 * \param value Valeur à mettre dans CC
 */
void update_cc(Machine *pmach, int value) {
	if (value == 0) 	pmach->_cc = CC_Z;
	else if (value < 0) pmach->_cc = CC_N;
	else if (value > 0) pmach->_cc = CC_P;
}

/*!
 * Retourne la valeur contenue dans une instruction.
 * Les différents modes sont supportés : immediate, absolute & indexed.
 *
 * \param pmach Machine dans laquelle effectuer l'opération
 * \param instr Instruction de laquelle récupérer la valeur.
 * \return La valeur correspondante.
 */
int get_instruction_value(Machine *pmach, Instruction instr) {
	int value;
	if (instr.instr_generic._immediate) {
		value = instr.instr_immediate._value;
	} else {
		int address = instr.instr_absolute._address;
		if (instr.instr_generic._indexed) {
			address = pmach->_registers[instr.instr_indexed._rindex] + instr.instr_indexed._offset;
		}
		check_data_address(pmach, address) ;

		value = pmach->_data[address];	
	}
	return value;
}

/*!
 * Lance une erreur si l'instruction est immédiate.
 *
 * \param pmach Machine dans laquelle effectuer l'opération
 * \param instr Instruction à vérifier
 */
void block_immediate(Machine *pmach, Instruction instr) {
	if (instr.instr_generic._immediate) error(ERR_IMMEDIATE, pmach->_pc-1);
}

/*!
 * Applique un delta sur la valeur d'un registre.
 *
 * \param pmach Machine dans laquelle on effectue la modification
 * \param reg Numéro du registre à modifier
 * \param delta Variation de valeur à appliquer
 */
void change_register(Machine *pmach, int reg, int delta) {
	pmach->_registers[reg] += delta;
	update_cc(pmach, pmach->_registers[reg]);
}

/*!
 * Vérifie que la valeur de l'adresse ne provoque pas d'erreur de 
 * segmentation sur Data. Lance une erreur si c'est le cas.
 */
void check_data_address(Machine *pmach, int addr) {
	if (addr > pmach->_datasize) {
		error(ERR_SEGDATA, pmach->_pc - 1);
	}
}

/*======================================
 *
 *		TRAITEMENT DES INSTRUCTIONS
 *======================================
 */

/*!
 * Traitement de l'instruction LOAD.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_load(Machine *pmach, Instruction instr) {
	int value = get_instruction_value(pmach, instr);
	pmach->_registers[instr.instr_generic._regcond] = value;
	update_cc(pmach, value);
}

/*!
 * Traitement de l'instruction STORE.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_store(Machine *pmach, Instruction instr) {
	block_immediate(pmach, instr);
	pmach->_data[instr.instr_absolute._address] = pmach->_registers[instr.instr_generic._regcond];
}

/*!
 * Traitement de l'instruction ADD.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_add(Machine *pmach, Instruction instr) {
	int value = get_instruction_value(pmach, instr);
	change_register(pmach, instr.instr_generic._regcond, value);
}

/*!
 * Traitement de l'instruction SUB.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_sub(Machine *pmach, Instruction instr) {
	int value = get_instruction_value(pmach, instr);
	change_register(pmach, instr.instr_generic._regcond, -value);
}

/*!
 * Traitement de l'instruction CALL.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_call(Machine *pmach, Instruction instr) {
	block_immediate(pmach, instr);
	if (check_condition(pmach, instr)) {
		stack_data(pmach, pmach->_pc);
		check_data_address(pmach, instr.instr_absolute._address);
		pmach->_pc = instr.instr_absolute._address;
	}
}

/*!
 * Traitement de l'instruction RET.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_ret(Machine *pmach, Instruction instr) {
	pmach->_pc = pop_data(pmach);
}

/*!
 * Traitement de l'instruction PUSH.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_push(Machine *pmach, Instruction instr) {
	int value = get_instruction_value(pmach, instr);
	stack_data(pmach, value);
}

/*!
 * Traitement de l'instruction POP.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_pop(Machine *pmach, Instruction instr) {
	block_immediate(pmach, instr);
	//printf("Processing POP :");
	check_data_address(pmach, instr.instr_absolute._address);
	pmach->_data[instr.instr_absolute._address] = pop_data(pmach);
	printf("\n");
}

/*!
 * Traitement de l'instruction BRANCH.
 *
 * \param pmach Machine qui exécute l'instruction
 * \param instr L'instruction à exécuter
 */
void process_branch(Machine *pmach, Instruction instr) {
	block_immediate(pmach, instr);
	if (check_condition(pmach, instr)) {
		check_data_address(pmach, instr.instr_absolute._address);
		pmach->_pc = instr.instr_absolute._address;
	}
}