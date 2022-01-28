#ifndef __CD_C
#define __CD_C

/* Permet de changer de dossier le processus courant 
 * Param :
 *      in:
 *          char * seq: chaine contenant le dossier courant
*/
void cd_action(char * seq);

/* Recupere le dossir ou l'on se trouve 
 * param :
 *  in/out :
 *      char ** pwd : chaine qui contiendra le dossier courant;
*/
void get_dir(char ** pwd);

#endif