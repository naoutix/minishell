#ifndef _PROCESSUS_H
#define _PROCESSUS_H

/* Etat des processus */
enum etat {SUSPENDU, ACTIF};
typedef enum etat etat;

/* Chaine pour mémoriser les processus */
typedef struct processus processus;
typedef processus * lproc;

/* Node de maillon */
struct processus {
	int id;			// id du processus
	int pid;		// pid du processus
	etat e;			// etat du processus
	char * cmd;		// ligne exécuté
    lproc suivant;	// Maillon suivant du processus
};

/* Créer une chaine vide 
 * Retourne : une chaine vide (NULL)
*/
lproc init_null();

/* Ajoute un element a la liste des processus 
 * Param:
 *	in:
 *		int id: id donnée au processus
 *		int pid : pid du processus
 *		etat e : etat du processus
 *		char * cmd : ligne de commande éxécuté
 *	in/out:
 *		lproc * processus : liste des processus
*/
void insertion (int id, int pid, etat e, char * cmd, lproc * processus);

/* Supprimer un element de la liste
 * Param:
 * 	in:
 * 		int Pid : pid du processus à supprimer.
 *	in/out :
 * 		lproc * processus : list des processus
*/
void supprimer(int pid, lproc * processus);

/* Changer l'etat d'un processus 
 * Param :
 * 	in:
 * 		int pid : pid du processus à changer;
 * 		etat e : nouvel etat;
 * 	in/out:
 * 		lproc * processus : liste des processus	
*/
void update_etat(int pid, etat e,lproc * processus);

/* Vider la liste 
 *	param :
 *		in/out:
 *		lproc * processus: liste à détruire
*/
void vider(lproc * processus);

/* obtenir le pid à partir de l'id 
 * Param:
 * 	in:
 * 		int id: id du processus à chercher
 * 		lproc processus : liste des processus
 * Retourne : le pid du processus
 *			  -1 si il ne le trouve pas
*/
int get_pid(int id, lproc processus);

/* Afficher de maniere convivial la liste des processus 
 * Param : 
 * 	in:
 * 		lproc processus : liste des processus
 * Affichage :
 * 	[id] pid	etat	cmd	
*/
void jobs(lproc processus);

/* Transformer en chaine de caratere simple la sequence 
 * Param :
 * 	in:
 * 		char ** seq2: tableau à copier
 *  in/out 
 * 		char ** seq1: Chaine destination
*/
void toString(char ** seq1, char **seq2);

#endif