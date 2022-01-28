#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "processus.h"

lproc init_null(){
	return NULL;
}

void insertion (int id, int pid, etat e, char * cmd, lproc * processus){
    lproc node;
	// Allocation mémoire nécessaire 
	node = malloc(sizeof(struct processus));

	// Affectation
	node -> cmd = cmd;
	node -> pid = pid;
	node -> id = id;
	node -> suivant = *processus;
	node -> e = e;

	// nouvelle liste composée du nouveau maillon
	*processus = node;
}

void supprimer(int pid, lproc * processus){
	lproc suppr; // Variable temporaire contenant le processus a supprimer
	if(*processus!=NULL)
	{
		if((*processus)-> pid == pid)
		{
			// Processus a supprimer trouvé
			suppr = (*processus); // Stockage
			(*processus) = (*processus)-> suivant; // Recollement de la chaine

            // Libération mémoire dynamique
			free(suppr->cmd);
			free(suppr);
		} 
		else 
		{
			supprimer(pid, &((*processus) -> suivant));
		}
	} 
	else
    {
        printf("Pid not found");
    }
    
}

void update_etat(int pid,etat e , lproc * processus){
	if (*processus!= NULL)
    {
        if((*processus)-> pid == pid)
        {
			// Processus trouvé !
            (*processus) ->e = e;
		} 
        else 
        {
			// Processus non trouvé
			update_etat(pid,e, &((*processus) -> suivant));
		}
	} 
    else
    {
		// Processus inexistant
        printf("Id not found");
    }
}

void vider(lproc * processus){
	// Détruit tout la liste

	if (*processus != NULL)
	{
		supprimer((*processus)->pid,processus);
		vider(processus);
	}
}

int get_pid(int id, lproc processus) {
    int pid =-1;
	if(processus!=NULL)
    {
		if(processus->id == id)
        {
			// Processus trouvé
            pid = processus->pid;
		} 
        else 
        {
			// processus non trouvé
			pid = get_pid(pid, processus-> suivant);
		}
	} 
    else
    {
		// Processus inexistant
    }
    return pid;
}

void jobs(lproc processus){
	// Afficher toute la chaine
	if (processus != NULL){
		// id pid
		printf("[%d] %d\t", processus->id, processus -> pid);
		// etat
		if (processus-> e == 0){
			printf("suspendu\t");
		} else {
			printf("actif\t");
		}
		// Commande
		printf("%s \n", processus-> cmd);
		// processus suivant
		if (processus -> suivant != NULL){
			jobs(processus-> suivant);
		} else {
			printf("\n");
		}
	}
}

void toString(char **seq1, char **seq2) {
	int i = 0;
    strcpy(*seq1,seq2[i]); // On copie la première sequence
    i++;
	while (seq2[i] != NULL) {
        *seq1 = realloc(*seq1,strlen(seq2[i])+strlen(*seq1)+2); // Reallocation
		strcat(*seq1, " " ); // Ajout d'un espace non présent dans le tableau
        strcat(*seq1,seq2[i]); // Concatenation
        i++;
	}
}