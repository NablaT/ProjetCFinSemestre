#include <stdio.h>
#include "machine.h"

int i = 0;

void stack_data(Machine *pmach, int data);
int pop_data(Machine *pmach);

void process_load(Machine *pmach, Instruction instr);
void process_store(Machine *pmach, Instruction instr);
void process_add(Machine *pmach, Instruction instr);
void process_sub(Machine *pmach, Instruction instr);

void process_branch(Machine *pmach, Instruction instr);
void process_call(Machine *pmach, Instruction instr);
void process_ret(Machine *pmach, Instruction instr);
void process_push(Machine *pmach, Instruction instr);
void process_pop(Machine *pmach, Instruction instr);

char* code_op_name (Code_Op cop) {
	switch(cop) {
		case	ILLOP: return "ILLOP";
		case	NOP: return "NOP";

		case	LOAD: return "LOAD";
		case	STORE: return "STORE";
		case	ADD: return "ADD";
		case	SUB: return "SUB";

		case	BRANCH: return "BRANCH";
		case	CALL: return "CALL";
		case	RET: return "RET";
		case	PUSH: return "PUSH";
		case	POP: return "POP";

		case	HALT: return "HALT";
		default: return "";
	}
}

void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr);

//! Décodage et exécution d'une instruction
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return faux après l'exécution de \c HALT ; vrai sinon
 */
bool decode_execute(Machine *pmach, Instruction instr) {
	// Brace yourselves !
	//printf("0x%x : %s\n", pmach->_pc -1 , code_op_name(instr.instr_generic._cop));
	trace("Execution", pmach, instr, pmach->_pc);
	//printf("reg[2]=%d\n", pmach->_registers[2]);
	switch (instr.instr_generic._cop) {
		case ILLOP: return false; // arret intempestif
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
		
		default: return false;
	}
	//pmach->_registers[2] = i;
	//if (++i == 10) return false;
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
 *		METHODES PRATIQUES
 * 
 */

void stack_data(Machine *pmach, int data) {
	printf("\n==> Stacking value %d to %x\n", data, pmach->_sp);
	pmach->_data[(pmach->_sp)--] = data;
}

int pop_data(Machine *pmach) {
	return pmach->_data[++(pmach->_sp)];
}

bool check_condition(Machine *pmach, Instruction instr) {
	if (instr.instr_generic._regcond == NC) return true;
	printf("|Checking condition : %d => %d", instr.instr_generic._regcond, pmach->_cc);
	if (instr.instr_generic._regcond == EQ && pmach->_cc == CC_Z) return true;
	// TODO : voir si la valeur inconnue retourne true ?
	if (instr.instr_generic._regcond == NE && pmach->_cc != CC_Z) return true;

	if (instr.instr_generic._regcond == GT && pmach->_cc == CC_P) return true;
	if (instr.instr_generic._regcond == GE && pmach->_cc != CC_N) return true;

	if (instr.instr_generic._regcond == LT && pmach->_cc == CC_N) return true;
	if (instr.instr_generic._regcond == LE && pmach->_cc != CC_P) return true;

	return false;
}

void update_cc(Machine *pmach, int value) {
	printf("\n\t|Updating CC with %d: ", value);
	if (value == 0) 	pmach->_cc = CC_Z;
	else if (value < 0) pmach->_cc = CC_N;
	else if (value > 0) pmach->_cc = CC_P;
	printf("%d |", pmach->_cc);
}


int get_instruction_value(Machine *pmach, Instruction instr) {
	int value;
	if (instr.instr_generic._immediate) {
		printf("Immediate (value:%d)/ ", instr.instr_immediate._value);
		value = instr.instr_immediate._value;
		//im = instr.instr_immediate;

	} else {
		int address = instr.instr_absolute._address;

		if (instr.instr_generic._indexed) {
			address = pmach->_registers[instr.instr_indexed._rindex] + instr.instr_indexed._offset;
			printf("\n  (indexed:R[0x%x]: 0x%x + 0x%x = 0x%x)",
				instr.instr_indexed._rindex,
				pmach->_registers[instr.instr_indexed._rindex],
				instr.instr_indexed._offset,
				address);
		}

		value = pmach->_data[address];	
		
		printf("Not immediate (from adress: DATA[0x%x]=%d)/ ",
			address,
			value);
	}
	return value;
}

void change_register(Machine *pmach, int reg, int delta) {
	pmach->_registers[reg] += delta;
	update_cc(pmach, pmach->_registers[reg]);
}

/*
 *		TRAITEMENT DES INSTRUCTIONS
 *
 */

void process_load(Machine *pmach, Instruction instr) {
	printf("Processing LOAD :");
	int value = get_instruction_value(pmach, instr);
	printf(" R[%d] = %d", instr.instr_generic._regcond, value);
	pmach->_registers[instr.instr_generic._regcond] = value;
	update_cc(pmach, value);
	printf("\n");
}

void process_store(Machine *pmach, Instruction instr) {
	printf("Processing STORE : Data[%x] = REG[%d] = %d", instr.instr_absolute._address,
		instr.instr_generic._regcond,
		pmach->_registers[instr.instr_generic._regcond]);
	pmach->_data[instr.instr_absolute._address] = pmach->_registers[instr.instr_generic._regcond];
	printf("\n");
}

void process_add(Machine *pmach, Instruction instr) {
	printf("Processing ADD :");
	int value = get_instruction_value(pmach, instr);
	printf("Adding to REG[%d] /", instr.instr_generic._regcond);
	change_register(pmach, instr.instr_generic._regcond, value);
	printf("Next value : %d (%x) /", pmach->_registers[instr.instr_generic._regcond], pmach->_registers[instr.instr_generic._regcond]);
	printf("\n");
}

void process_sub(Machine *pmach, Instruction instr) {
	printf("Processing SUB :");
	int value = get_instruction_value(pmach, instr);
	printf("Subbing to REG[%d] /", instr.instr_generic._regcond);
	change_register(pmach, instr.instr_generic._regcond, -value);
	printf("Next value : %d (%x) /", pmach->_registers[instr.instr_generic._regcond], pmach->_registers[instr.instr_generic._regcond]);
	printf("\n");
}

void process_call(Machine *pmach, Instruction instr) {
	printf("Processing CALL : %d=%d /", pmach->_cc, instr.instr_generic._regcond);
	if (check_condition(pmach, instr)) {
		stack_data(pmach, pmach->_pc);
		printf("Going to address %x", instr.instr_absolute._address);
		pmach->_pc = instr.instr_absolute._address;
	}
	printf("\n");
}

void process_ret(Machine *pmach, Instruction instr) {
	printf("Processing RET : ");
	pmach->_pc = pop_data(pmach);
	printf("Going back to 0x%x", pmach->_pc);
	printf("\n");
}

void process_push(Machine *pmach, Instruction instr) {
	printf("Processing PUSH :");
	int value = get_instruction_value(pmach, instr);
	stack_data(pmach, value);
	printf("\n");
}

void process_pop(Machine *pmach, Instruction instr) {
	printf("Processing POP :");
	pmach->_data[instr.instr_absolute._address] = pop_data(pmach);
	printf("\n");
}

void process_branch(Machine *pmach, Instruction instr) {
	printf("Processing BRANCH :");
	if (check_condition(pmach, instr)) {
		printf("True ! Going to %x", instr.instr_absolute._address);
		pmach->_pc = instr.instr_absolute._address;
	}
	printf("\n");
}