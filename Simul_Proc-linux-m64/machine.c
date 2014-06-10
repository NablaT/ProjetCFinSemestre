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
  int bits_read;
  unsigned textsize;
  unsigned datasize;
  unsigned dataend;
  //Ouverture du fichier :
  int handle = open(programfile,O_RDONLY);
  
  if( handle < 0 ){
    fprintf(stderr, "Erreur d'ouverture un program dans <machine.c:read_program>.\n");
    exit(1);
  }

  if( (bits_read = read(handle, &textsize, sizeof(pmach -> _textsize) ) ) != sizeof(pmach->_textsize) ){
    fprintf(stderr, "Erreur de lecture de 'textsize' dans <machine.c:read_program> dans '%s': %d bits lus au lieu de %ld.\n", programfile, bits_read, sizeof(textsize));
    exit(1);
  }

  if( (bits_read = read(handle, &datasize, sizeof(pmach -> _datasize) ) ) != sizeof(pmach->_datasize) ){
    fprintf(stderr, "Erreur de lecture de 'datasize' dans <machine.c:read_program> dans '%s': %d bits lus au lieu de %ld.\n", programfile, bits_read, sizeof(datasize));
    exit(1);
  }

  if( (bits_read = read(handle, &dataend, sizeof(pmach -> _dataend) ) ) != sizeof(pmach->_dataend) ){
    fprintf(stderr, "Erreur de lecture de 'dataend' dans <machine.c:read_program> dans '%s': %d bits lus au lieu de %ld.\n", programfile, bits_read, sizeof(dataend) );
    exit(1);
  }

  Instruction *text = malloc( textsize * sizeof(Instruction) );
  //lecture des instructions :
  if( (bits_read = read( handle, text, textsize * sizeof(Instruction) ) ) != ( textsize * sizeof(Instruction) ) ) {
    fprintf(stderr, "Erreur de lecture de 'text' dans <machine.c:read_program> '%s': %d bits lus au lieu de %ld.\n",programfile,bits_read,textsize * sizeof(Instruction));
    exit(1);
  }

  Word *data = malloc(datasize * sizeof(Word));
  //lecture des données :
  if( (bits_read = read(handle, data, datasize * sizeof(Word))) != (datasize * sizeof(Word))) {
    fprintf(stderr, "Erreur de lecture de 'data' dans <machine.c:read_program> '%s': %d bits lus au lieu de %ld.\n",programfile,bits_read,datasize * sizeof(Word));
    exit(1);
  }

  //Fermeture du fichier :
  if( close(handle) != 0 ) {
    fprintf(stderr, "Erreur de fermeture du fichier binaire dans <machine.c:read_program>.\n");
    exit(1);
  }

  //On charge le programme dans la machine :
  load_program(pmach, textsize, text, datasize, data, dataend);

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

