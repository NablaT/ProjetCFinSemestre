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


//! Chargement d'un programme
/*!
 * La machine est réinitialisée et ses segments de texte et de données sont
 * remplacés par ceux fournis en paramètre.
 *
 * \param pmach la machine en cours d'exécution
 * \param textsize taille utile du segment de texte
 * \param text le contenu du segment de texte
 * \param datasize taille utile du segment de données
 * \param data le contenu initial du segment de texte
 */
void load_program(Machine *pmach,
                  unsigned textsize, Instruction text[textsize],
                  unsigned datasize, Word data[datasize],  unsigned dataend){
  //On met à jour les registres à 0
  for(int i=0;i<NREGISTERS;i++){
    pmach->_registers[i]=0;
  }
  //Meme chose pour PC et CC
  pmach->_pc=0; 
  pmach->_cc=CC_U;//Code conditionà à 0.
  // On remplace tableau text
  pmach->_text=text;// Mémoire pour les instructions
  //Ainsi que data
  pmach->_data=data; //Mémoire de données

  //Initialisation de datasize et dataend
  pmach->_datasize=datasize; 
  pmach->_dataend=dataend; 
  //Init de SP ;
  pmach->_sp = datasize-1;


}

//! Lecture d'un programme depuis un fichier binaire
/*!
 * Le fichier binaire a le format suivant :
 * 
 *    - 3 entiers non signés, la taille du segment de texte (\c textsize),
 *    celle du segment de données (\c datasize) et la première adresse libre de
 *    données (\c dataend) ;
 *
 *    - une suite de \c textsize entiers non signés représentant le contenu du
 *    segment de texte (les instructions) ;
 *
 *    - une suite de \c datasize entiers non signés représentant le contenu initial du
 *    segment de données.
 *
 * Tous les entiers font 32 bits et les adresses de chaque segment commencent à
 * 0. La fonction initialise complétement la machine.
 *
 * \param pmach la machine à simuler
 * \param programfile le nom du fichier binaire
 *
 */

void read_program(Machine *pmach, const char *programfile){
  unsigned int textsize, datasize, dataend; 
  

  int opening= open(programfile,O_RDONLY);

  //Verifications avant la lecture par rapport aux champs de pmach.
  if(opening<0){
    fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
    exit(1);
  }

  //Verification du nombre de bits lus pour dataend. Si ce nombre ne correspond pas au nombre de bits de dataend dans pmach alors on renvoie une erreur.
  read(opening, &dataend, sizeof(pmach->_dataend));//sizeof
  
  //Verification du nombre de bits lus pour datasize. 
  read(opening, &datasize, sizeof(pmach->_datasize));
  
  //Verification du nombre de bits lus pour textsize.
  read(opening, &textsize, sizeof(pmach->_textsize));

  //On lit les instructions:
  Instruction *instruction=malloc(textsize * sizeof(Instruction));
  read(opening, instruction, textsize*sizeof(Instruction));
  
  //On lit les données: 
  Word *data = malloc(datasize * sizeof(Word));
  read(opening, data, datasize*sizeof(Word));

  //On ferme le fichier et on verifie que la fermeture s'est bien deroulée.
  int file_close=close(opening); 
  if(file_close!=0){
    fprintf(stderr, "Erreur lors de la fermeture du fichier binaire\n");
    exit(1);
  }
  
  //On charge ensuite le programme à l'intérieur de la machine
  load_program(pmach, textsize,instruction,datasize,data,dataend); 
}

void dump_memory(Machine *pmach){

}

void print_program(Machine *pmach){
    putchar('\n');
    printf("*** PROGRAM (size: %d) ***", pmach->textsize);
    putchar('\n');
    
    for(int i = 0; i < pmach->textsize; i++){
       printf("0x%04x: 0x%08x\t ", i, pmach->_text[i]._raw);
       print_instruction(pmach->_text[i],pmach->_text[i].instr_absolute._address);
       putchar('\n');
    }
}

void print_data(Machine *pmach){
    putchar('\n');
    printf("*** DATA (size: %d, end = 0x%08x (%d)) ***", pmach->datasize, pmach->dataend, pmach->dataend);
    putchar('\n');
    
    for(int i = 0;i < pmach->datasize; i++){
       printf("0x%04x: 0x%08x %d\t", i, pmach->data[i], pmach->data[i]);
    
       if((i%2 == 0) && (i != 0)){
          putchar('\n');
       }
    }
    putchar('\n');
}

/*
  Fonction put_cc. Cette fonction prend en parametre un code condition et renvoie la lettre correspondante.
  Pour CC_U => U
  Pour CC_Z => Z
  Pour CC_P => P
  Pour CC_N => N
 */

char put_cc(Condition_Code cc){
  switch(cc){
  case CC_U: 
    return 'U';
  case CC_Z:
    return 'Z'; 
  case CC_P: 
    return 'P';
  case CC_N: 
    return 'N';
  }
  return NULL; 
}

/* Fonction print_registers. Cette fonction affiche tous les registres avec leur adresse et leur valeur.
 */
void print_registers(Machine *pmach){
  for(int i = 0 ; i < NREGISTERS ; i++){
    printf("R%02d: 0x%08x\t%d\t", i, pmach->_registers[i], pmach->_registers[i]);
    if (i % 3 == 2){
      putchar('\n');
    }
  }
}

//! Affichage des registres du CPU
/*!
 * Les registres généraux sont affichées en format hexadécimal et décimal.
 *
 * \param pmach la machine en cours d'exécution
 */
void print_cpu(Machine *pmach){
  
  printf("\n*** CPU ***\n");
  printf("PC:  0x%08x\tCC: ",pmach->_pc); //On affiche l'adresse de PC
  //On appelle la fonction put_cc qui renvoie le caractere à afficher en fonction du code condition de la machine courante.
  char char_to_put=put_cc(pmach->_cc);
  putchar(char_to_put);
  putchar('\n');
  putchar('\n');
  //Appelle de la fonction print_registers qui affiche les registres avec leur adresse et leur valeur
  print_registers(pmach);
  putchar('\n');
}

void simul(Machine *pmach, bool debug){
  bool stop=true; 
  while(stop){
    //On appelle la fonction trace qui se trouve dans exec.c. On lui donne en parametre le message à afficher, la machine qui est en cours d'execution (pmach), l'instruction en cours et l'adresse de l'instruction grâce à pc. 

    ///////// A ENLEVER APRES RECUPERATION D'EXEC DE TOM ////////////////////////:
    trace("Execution",pmach,pmach->_text[pmach->_pc],pmach->_pc);

    /////////////////////////////////////////////////////////////////
    if(pmach->_pc<pmach->_textsize){
      
      stop=decode_execute(pmach, pmach->_text[pmach->_pc++]); //On decode et execute l'instruction suivante dont l'adresse est pc+1. Cette fonction renvoie faux lorsque l'instruction a decoder est HALT qui marque la fin.
      if(debug){
	debug=debug_ask(pmach);////////////////////////////// A RAJOUTER//////////////
      }
    }
    else{
      error(ERR_SEGTEXT,pmach->_pc - 1); //On précise l'erreur rencontré: ERR_SEGTEXT qui correspond à la violation de la taille du segment de text ainsi que l'adresse à laquelle se trouve l'erreur, cette adresse se trouve à pc-1.
    }
  }
}
