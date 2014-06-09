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
void read_program(Machine *mach, const char *programfile){

}