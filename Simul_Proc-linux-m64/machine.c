/***** machine.c *****/

#include "machine.h"
#include "exec.h"
#include "debug.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void load_program(Machine *pmach,
                  unsigned textsize, Instruction text[textsize],
                  unsigned datasize, Word data[datasize],  unsigned dataend){

}

void read_program(Machine *mach, const char *programfile){
	


}

void dump_memory(Machine *pmach){

}

void print_program(Machine *pmach){

}

void print_data(Machine *pmach){

}

void print_cpu(Machine *pmach){

}

void simul(Machine *pmach, bool debug){

}
