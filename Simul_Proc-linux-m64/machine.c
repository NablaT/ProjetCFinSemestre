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
  int cpt_bits_read;

  int opening= open(programfile,O_RDONLY);

  //Verifications avant la lecture par rapport aux champs de pmach.
  if(opening<0){
    fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
    exit(1);
  }

  //Verification du nombre de bits lus pour dataend. Si ce nombre ne correspond pas au nombre de bits de dataend dans pmatch alors on renvoie une erreur.
  cpt_bits_read = read(opening, &dataend, sizeof(pmach->_dataend));
  if( cpt_bits_read != sizeof(pmach->_dataend)) {
    fprintf(stderr, "Fichier: %s. Erreur de lecture de 'dataend' : %d bits lus au lieu de %ld\n",programfile,cpt_bits_read,sizeof(dataend));
    exit(1);
  }
  
  //Verification du nombre de bits lus pour datasize. 
  cpt_bits_read = read(opening, &datasize, sizeof(pmach->_datasize));
  if(cpt_bits_read != sizeof(pmach->_datasize)) {
    fprintf(stderr, "Fichier: %s. Erreur de lecture de 'datasize': %d bits lus au lieu de %ld\n",programfile,cpt_bits_read,sizeof(datasize));
    exit(1);
  }
  
  //Verification du nombre de bits lus pour textsize.
  cpt_bits_read = read(opening, &textsize, sizeof(pmach->_textsize));
  if(cpt_bits_read != sizeof(pmach->_textsize)) {
    fprintf(stderr, "Fichier: %s. Erreur de lecture de 'textsize' : %d bits lus au lieu de %ld\n", programfile,cpt_bits_read, sizeof(textsize));
    exit(1);
  }
  
  //On lit les instructions:
  Instruction *instruction=malloc(textsize * sizeof(Instruction));
  cpt_bits_read = read(opening, instruction, textsize*sizeof(Instruction));
  if(cpt_bits_read!=sizeof(textsize *sizeof(Instruction))){
      fprintf(stderr, "Fichier: %s. Erreur de lecture de 'text' : %d bits lus au lieu de %ld\n",programfile,cpt_bits_read,textsize * sizeof(Instruction));
    exit(1);
  }
  
  //On lit les données: 
  Word *data = malloc(datasize * sizeof(Word));
  cpt_bits_read = read(opening, data, datasize*sizeof(Word));
  if(cpt_bits_read != (datasize * sizeof(Word))) {
    fprintf(stderr, "Fichier: %s. Erreur de lecture de 'data': %d bits lus au lieu de %ld\n",programfile,cpt_bits_read,datasize * sizeof(Word));
    exit(1);
  }
  
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

}

void print_data(Machine *pmach){

}

void print_cpu(Machine *pmach){

}

void simul(Machine *pmach, bool debug){

}


}
>>>>>>> 06241e2814818c8953ae02ce01eefcbdc5cc218c

