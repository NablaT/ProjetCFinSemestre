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
  //Initialisation de textsize, datasize et dataend
  pmach->_textsize = textsize;
  pmach->_datasize=datasize; 
  pmach->_dataend=dataend; 
  //Init de SP ;
  pmach->_sp = datasize-1;
}

void read_program(Machine *pmach, const char *programfile){
  unsigned int textsize, datasize, dataend; 
  int opening= open(programfile,O_RDONLY);

  // Verifications avant la lecture par rapport aux champs de pmach.
  if(opening<0){
    fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
    exit(1);
  }

  // Verification du nombre de bits lus pour textsize.
  read(opening, &textsize, sizeof(pmach->_textsize));

  // Verification du nombre de bits lus pour datasize. 
  read(opening, &datasize, sizeof(pmach->_datasize));  

  // Verification du nombre de bits lus pour dataend. Si ce nombre ne correspond pas au nombre de bits de dataend dans pmach alors on renvoie une erreur.
  read(opening, &dataend, sizeof(pmach->_dataend));//sizeof
  
  // On lit les instructions:
  Instruction *instruction = malloc(textsize * sizeof(Instruction));
  read(opening, instruction, textsize*sizeof(Instruction));
  
  // On lit les données: 
  Word *data = malloc(datasize * sizeof(Word));
  read(opening, data, datasize*sizeof(Word));

  // On ferme le fichier et on verifie que la fermeture s'est bien deroulée.
  int file_close=close(opening); 
  if(file_close != 0){
    fprintf(stderr, "Erreur lors de la fermeture du fichier binaire\n");
    exit(1);
  }
  
  // On charge ensuite le programme à l'intérieur de la machine
  load_program(pmach, textsize,instruction,datasize,data,dataend); 
}

void dump_memory(Machine *pmach){
  putchar('\n');
  
  int file= open("dump.bin", O_TRUNC|O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR); //Dernier champ permission: qui peut lire ou ecrire dans le fichier
  //On verifie si il n'y a pas eu de problème lors de l'ouverture.
  if (file==-1){
    printf("Erreur lors de l'ouverture du fichier dump.bin\n");
    exit(1); 
  }
  //En premier on met textsize
  write(file, &pmach->_textsize,sizeof(pmach->_textsize));
  //Puis datasize
  write(file, &pmach->_datasize,sizeof(pmach->_datasize));
  //Puis dataend
  write(file, &pmach->_dataend,sizeof(pmach->_dataend));

  putchar('\n');
  printf("Instruction text[] = {\n");
  //Boucle pour sauvegarder et afficher les instructions
  for(int i = 0; i < pmach->_textsize; i++){

    if(i%4 == 0){

      putchar('\t');

    }
    //On écrit dans le fichier pour chaque itération
    write(file,&pmach->_text[i]._raw, sizeof(pmach->_text[0]));
    //Puis on affiche
    printf("0x%08x, ", pmach->_text[i]._raw);
    if(i%4 == 3){
      putchar('\n');

    }

  }
  printf("\n};\n");
  printf("unsigned textsize = %d;\n\n", pmach->_textsize);
  
  
  printf("Word data[] = {\n");
  //Boucle pour sauvegarder et afficher les données. 
  for(int i = 0 ; i < pmach->_datasize ; i++){
    //On écrit dans le fichier pour chaque itération
    write(file,&pmach->_data[i],sizeof(Word));
    //Puis on affiche
    printf("\t0x%08x, ", pmach->_data[i]);
    if (i % 4 == 3){
      putchar('\n');
    }
    if (pmach->_datasize % 4 != 0){
      putchar('\n');
    }
  }

  printf("};\n");
  
  printf("unsigned datasize = %d;\n", pmach->_datasize);
  printf("unsigned dataend = %d;\n", pmach->_dataend);
  int closing=close(file);
  if(closing!=0){
    printf("Erreur lors de la fermture du fichier binaire dump.bin\n");
    exit(1); 
  }
  
}

void print_program(Machine *pmach){
  putchar('\n');
  printf("*** PROGRAM (size: %d) ***", pmach->_textsize);
  putchar('\n');
    
  for(int i = 0; i < pmach->_textsize; i++){
    printf("0x%04x: 0x%08x\t ", i, pmach->_text[i]._raw);
    print_instruction(pmach->_text[i],pmach->_text[i].instr_absolute._address);
    putchar('\n');
  }
}

void print_data(Machine *pmach){
  putchar('\n');
  printf("*** DATA (size: %d, end = 0x%08x (%d)) ***", pmach->_datasize, pmach->_dataend, pmach->_dataend);
  putchar('\n');
    
  for(int i = 0;i < pmach->_datasize; i++){
    if((i%3 == 0) && (i != 0)){
      putchar('\n');
    }    
    printf("0x%04x: 0x%08x %d\t", i, pmach->_data[i], pmach->_data[i]);
    
  }
  putchar('\n');
}

/*!
  * Fonction put_cc. Cette fonction prend en parametre un code condition et renvoie la lettre correspondante.
  * Pour CC_U => U
  * Pour CC_Z => Z
  * Pour CC_P => P
  * Pour CC_N => N

  * \param cc Code condition
  * \return La lettre correspondante
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
  return 'A'; 
}

/*
 * Fonction print_registers. Cette fonction affiche tous les registres avec leur adresse et leur valeur.
 *
 * \param pmach Machine dont il faut afficher les registres
 */
void print_registers(Machine *pmach){
  for(int i = 0 ; i < NREGISTERS ; i++){
    printf("R%02d: 0x%08x\t%d\t", i, pmach->_registers[i], pmach->_registers[i]);
    if (i % 3 == 2){
      putchar('\n');
    }
  }
}

void print_cpu(Machine *pmach){
  
  printf("\n*** CPU ***\n");
  printf("PC:  0x%08x\tCC: ",pmach->_pc); //On affiche l'adresse de PC
  //On appelle la fonction put_cc qui renvoie le caractere à afficher en fonction du code condition de la machine courante.
  char char_to_put=put_cc(pmach->_cc);
  if(char_to_put=='A'){
    printf("Erreur code condition dans la fonction put_cc dans machine.c\n");
    exit(1);
  }
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

    trace("Execution",pmach,pmach->_text[pmach->_pc],pmach->_pc);

    if(pmach->_pc<pmach->_textsize){      
      stop=decode_execute(pmach, pmach->_text[pmach->_pc++]); //On decode et execute l'instruction suivante dont l'adresse est pc+1. Cette fonction renvoie faux lorsque l'instruction a decoder est HALT qui marque la fin.
      if(debug){
	         debug=debug_ask(pmach);
      }
    }
    else{
      error(ERR_SEGTEXT,pmach->_pc - 1); //On précise l'erreur rencontré: ERR_SEGTEXT qui correspond à la violation de la taille du segment de text ainsi que l'adresse à laquelle se trouve l'erreur, cette adresse se trouve à pc-1.
    }
  }
}
