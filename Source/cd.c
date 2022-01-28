#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "cd.h"


void cd_action(char * seq) {
    int error;        
    if (seq == NULL)
    {
        //l'utilasteur n'as pas spécifié de chemin
        error = chdir(getenv("HOME"));
    } 
    else if (seq[0]=='~')
    {
        // L'utilsateur a utilser la variable d'environement "~"
        if (strlen(seq)>1)
        {
            //l'utilsateur utilse un chemin prenant racine dans son repertoire courant
            error = chdir(strcat(getenv("HOME"),seq+1));
        }
        else
        {
            //l'utilsateur se deplace dans son repetoire courant
            error = chdir(getenv("HOME"));
        }
                
    }
    else
    {
        // l'utilsateur se déplace grâce a chemin absolue
        error = chdir(seq);
    }

    if (error != 0) 
    {
        // Chemin non reconnue
        printf("No such file or directory\n");
    }
}

void get_dir(char ** pwd){
    
    // On estime la taille de la chaine
    char* buffer;
    buffer=getcwd(NULL,0);

    // on l'alloue et affectation
    *pwd = malloc(strlen(buffer)+1);
    getcwd(*pwd,strlen(buffer)+1);

}