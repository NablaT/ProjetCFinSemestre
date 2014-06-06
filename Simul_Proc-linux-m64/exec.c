#include <stdio.h>
#include "machine.h"

int i = 0;

void stack_data(Machine *pmach, int data);
int pop_data(Machine *pmach);

void process_load(Machine *pmach, Instruction instr);
void process_store(Machine *pmach, Instruction instr);
void process_add(Machine *pmach, Instruction instr);
void process_sub(Machine *pmach, Instruction instr);
void process_call(Machine *pmach, Instruction instr);
void process_push(Machine *pmach, Instruction instr);

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


//! Décodage et exécution d'une instruction
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return faux après l'exécution de \c HALT ; vrai sinon
 */
bool decode_execute(Machine *pmach, Instruction instr) {
	// Brace yourselves !
	printf("%d : %s\n", pmach->_pc , code_op_name(instr.instr_generic._cop));
	//printf("reg[2]=%d\n", pmach->_registers[2]);
	switch (instr.instr_generic._cop) {
		case LOAD: process_load(pmach, instr); break;
		case STORE: process_store(pmach, instr); break;
		case ADD: process_add(pmach, instr); break;
		case SUB: process_sub(pmach, instr); break;
		case CALL: process_call(pmach, instr); break;
		case PUSH: process_push(pmach, instr); break;
		case ILLOP: return false; // arret intempestif
	}
	//pmach->_registers[2] = i;
	if (++i == 6) return false;
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

}

void stack_data(Machine *pmach, int data) {
	pmach->_data[(pmach->_sp)--] = data;
}

int pop_data(Machine *pmach) {
	return pmach->_data[++(pmach->_sp)];
}

int get_instruction_value(Machine *pmach, Instruction instr) {
	int value;
	if (instr.instr_generic._immediate) {
		printf("Immediate (value:%d)/ ", instr.instr_immediate._value);
		value = instr.instr_immediate._value;
		//im = instr.instr_immediate;

	} else {
		value = pmach->_data[instr.instr_absolute._address];
		printf("Not immediate (from adress:%x=%d)/ ", instr.instr_absolute._address, value);
	}
	return value;
}


void process_load(Machine *pmach, Instruction instr) {
	printf("Processing LOAD :");
	int value = get_instruction_value(pmach, instr);
	printf(" R[%d] = %d", instr.instr_generic._regcond, value);
	pmach->_registers[instr.instr_generic._regcond] = value;
}

void process_store(Machine *pmach, Instruction instr) {
	printf("Processing STORE : Data[%x] = REG[%d] = %d", instr.instr_absolute._address,
		instr.instr_generic._regcond,
		pmach->_registers[instr.instr_generic._regcond]);
	pmach->_data[instr.instr_absolute._address] = pmach->_registers[instr.instr_generic._regcond];
	printf("\n");
}

void process_push(Machine *pmach, Instruction instr) {
	printf("Processing PUSH :");
	int value = get_instruction_value(pmach, instr);
	stack_data(pmach, value);
	/*pmach->_data[pmach->_sp] = value;
	pmach->_sp--;*/
	printf("\n");
}

void process_add(Machine *pmach, Instruction instr) {
	printf("Processing ADD :");
	int value = get_instruction_value(pmach, instr);
	printf("Adding to REG[%d] /", instr.instr_generic._regcond);
	pmach->_registers[instr.instr_generic._regcond] += value;
	printf("Next value : %d (%x) /", pmach->_registers[instr.instr_generic._regcond], pmach->_registers[instr.instr_generic._regcond]);

	printf("\n");
}

void process_sub(Machine *pmach, Instruction instr) {
	printf("Processing SUB :");
	int value = get_instruction_value(pmach, instr);
	printf("Adding to REG[%d] /", instr.instr_generic._regcond);
	pmach->_registers[instr.instr_generic._regcond] -= value;
	printf("Next value : %d (%x) /", pmach->_registers[instr.instr_generic._regcond], pmach->_registers[instr.instr_generic._regcond]);

	printf("\n");
}

void process_call(Machine *pmach, Instruction instr) {
	printf("Processing CALL : %d=%d /", pmach->_cc, instr.instr_generic._regcond);
	if (instr.instr_generic._regcond == NC || instr.instr_generic._regcond) {
		stack_data(pmach, pmach->_pc);
		printf("Going to address %x", instr.instr_absolute._address);
		pmach->_pc = instr.instr_absolute._address;
	}
	printf("\n");
}

void process_pop(Machine *pmach, Instruction instr) {
	printf("Processing POP :");
	pmach->_data[instr.instr_absolute._address] = pmach->_data[++(pmach->_sp)];
	printf("\n");
}

