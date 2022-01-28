#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#include "processus.h"
#include "cd.h"
#include "IO.h"
#include "readcmd.h"
extern char **environ;


/* Variable globale */
int count;      // Nombre de processus
lproc tab_jobs;  // liste des processus
pid_t pid = 0;  // processus fils en avant plan

/* Handler lié au ctrlC transformant SIGINT EN SIGKILL
 *   
*/
void ctrlC(int sig){
    // Tester si il ya des processus fils
    if (tab_jobs != NULL)
    {
        kill(pid,SIGKILL);
    }
}

/* Handler lié au ctrlZ transformant SIGTSTP EN SIGSTOP
 * 
*/
void ctrlZ(int sig){
    // Tester si il a des processus fils
    if (tab_jobs != NULL)
    {
        // Envoie du signal SIGSSTOp
        kill(pid,SIGSTOP);
    }
}

/* Handler permettant de suivre l'évolution des fils
 * Param :
 *  in:
 *      int sig : numéro du signal
*/
void suivi_fils (int sig) {

    sig ++;
    int etat_fils, pid_fils;

    do {
        pid_fils = (int) waitpid(-1, &etat_fils, WNOHANG | WUNTRACED | WCONTINUED);
        if ((pid_fils == -1) && (errno != ECHILD)) {

            perror("waitpid");

            exit(EXIT_FAILURE);

        } else if (pid_fils > 0) {

            if (WIFSTOPPED(etat_fils)) {
                
                update_etat(pid_fils,SUSPENDU,&tab_jobs);

            } else if (WIFCONTINUED(etat_fils)) {

                update_etat(pid_fils,ACTIF,&tab_jobs);
            
            } else if (WIFEXITED(etat_fils)) {

                supprimer(pid_fils,&tab_jobs);

            } else if (WIFSIGNALED(etat_fils)) {

                /* traiter signal */
                                printf("Wifi termsig:%d",WTERMSIG(etat_fils));

            } else if (WTERMSIG(etat_fils))
            {
                printf("Wifi termsig:%d",WTERMSIG(etat_fils));
            }

        }

    } while (pid_fils > 0);
    /* autres actions après le suivi des changements d'état */
}

/* Processus mis en avant plan
 * Param :
 *  in:
 *      int pid: numéro du processus fils a surveillé
*/
void foreground(int pid) {
    int status;
    if (waitpid(pid, &status, 0) < 0)
    {
        //printf("Erreur waitpid");
        // Il peut arriver que le temps de fermer tout les pipes le processus se termine avant
        // le waitpid. Le message d'erreur est donc ignorer
    }
    if (WIFEXITED(status))
    {
        supprimer(pid,&tab_jobs);
    }
    if (WIFSIGNALED(status)){
        supprimer(pid,&tab_jobs);
    }

}

/* Commande fg implantation
 * Param :
 *  in:
 *      char * seq : numéro du processus a mettre en avant plant
 *      int sig : signal à envoyé pour continuer
 * Note : Si char *seq est une chaine null il cherche le
 * premier processus et le met en avant plan
*/
pid_t fg(char * seq, int sig) {

    char * f= malloc(sizeof(*f));

    int pid_sig = -1;
    lproc processus; 
    processus = tab_jobs;
    if (seq != NULL) 
    {
        int id = atoi(seq);
        pid_sig = get_pid(id,tab_jobs);
    }
    else
    {
        if (processus != NULL) 
        {
            pid_sig = processus->pid;
        }
    }
                
    if (pid_sig != -1) {
        kill(pid_sig,sig);
    }
    else
    {
        printf("Id not found or no process to fg\n");
    }
    return (pid_t) pid_sig;
}

/* Commande bg implantation 
 * Param :
 *  in:
 *      char * seq : numéro du processus a continuer
 *      int sig : signal à envoyé pour continuer
 * Note : Si char *seq est une chaine null il cherche le
 * premier processus inactif et le réactive
*/
pid_t bg(char * seq, int sig) {
    int pid_sig = -1;
    lproc processus; 
    processus = tab_jobs;
    if (seq != NULL) 
    {
        int id = atoi(seq);
        pid_sig = get_pid(id,tab_jobs);
    }
    else
    {
        while (processus != NULL) 
        {
            if (processus->e == SUSPENDU) 
            {
                pid_sig = processus->pid;
                processus = NULL;
            }
            else
            {
                processus = processus->suivant;
            }
            
        }
    }
                
    if (pid_sig != -1) {
        kill(pid_sig,sig);
    }
    else
    {
        printf("Id not found or no process to bg\n");
    }
    return (pid_t) pid_sig;
}


/* Main programme */
int main() {
    /* Commande line */
    struct cmdline* commande;
    char ***seq;
    char *backgrounded;
    char *err;
	char *in;
	char *out;
    int error;

    /* Pipe */
    int pipe_cmd[2] ;   /* pipe entre deux commandes */
    int stop = 0;

    signal(SIGINT, ctrlC);
    signal(SIGTSTP, ctrlZ);
    signal(SIGCHLD, suivi_fils);
    /* Jobs */

    count = 0;
    tab_jobs = init_null();

    
	sigset_t ens;
	sigemptyset(&ens);
	sigaddset(&ens,SIGINT);
	sigaddset(&ens,SIGTSTP);
    while (!stop) {

        /* CD*/
        char * pwd;
        get_dir(&pwd);
        /* Affichage Shell */
        printf("%s@%s:%s$ ", getenv("USER"),getenv("NAME"),pwd);

        /* lecture Commande */
        commande = readcmd();  /* Lecture de la commande */
        seq = commande->seq;
        backgrounded = commande->backgrounded;
        err = commande->err;
        in = commande->in;
        out = commande->out;
        int in_desc = -1;
        if (in != NULL){
            in_desc = xopen(in,O_RDONLY);
        }
        int out_desc = -1;
        if (out != NULL){
            out_desc = xopen2(out, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        }

        if (seq[0] != NULL)
        {
            /** COMMANDES INTERNES **/
            /* CD*/
            if (strcmp(seq[0][0], "cd") == 0)
            {
                cd_action(seq[0][1]);
            }
            /* EXIT */
            else if (strcmp(seq[0][0], "exit") == 0)
            {
                stop = 1;
                vider(&tab_jobs);
            /* LIST */
            } else if (strcmp(seq[0][0], "list")== 0)
            {
                jobs(tab_jobs);
            }
            /* STOP */
            else if (strcmp(seq[0][0],"stop") == 0)
            {
                int id = atoi(seq[0][1]);
                int pidStop = get_pid(id,tab_jobs);
                if (pidStop != -1) {
                    kill(pidStop,SIGSTOP);
                }else{
                    printf("Error id not found\n");
                }
            }
            /* bg */
            else if (strcmp(seq[0][0],"bg") == 0)
            {
                bg(seq[0][1],SIGCONT);
            }
            /* fg */
            else if (strcmp(seq[0][0],"fg") == 0)
            {
                pid = fg(seq[0][1],SIGCONT);
                if (pid != -1)
                {
                    foreground(pid);
                }
            } 
            /** COMMANDES EXTERNES **/
            else {
                int nb_commande =0;                 // Nombre de commande sur la ligne
                while (seq[nb_commande] != NULL){
                    nb_commande++;
                }

                int tableau_pipe[nb_commande][2];   // Tableau contenant tout les pipes
                int element[2];
                /* Construction du Tableau */
                for(int k = 0; k<nb_commande-1;k++){
                    int retour =pipe(element);
                    tableau_pipe[k][0]=element[0];
                    tableau_pipe[k][1]=element[1];
                }
                int num_processus = 0;  // Contient l'id courant du processus sur le pipe
                for (num_processus=0; num_processus<nb_commande;num_processus++){
                    fflush(stdout);
                    pid = fork();
                    if (pid <0){
                        printf("Erreur Fork");
                    }
                    if (pid ==0) {
                        sigprocmask(SIG_BLOCK,&ens,NULL);
                        /* Fermer les pipes inutilise par le fils */ 
                        int j =0;
                        while (j<num_processus-1){
                            close(tableau_pipe[j][0]);
                            close(tableau_pipe[j][1]);
                            j++;
                        }
                        j=num_processus+1;
                        while (j<nb_commande-1)
                        {
                            close(tableau_pipe[j][0]);
                            close(tableau_pipe[j][1]);
                            j++;
                        }

                        /* lire le prochain pipe sauf pour le 1er */
                        if (num_processus != 0) {
                            close(tableau_pipe[num_processus-1][1]);            // lire dans le pipe avant
                            int desc = dup2(tableau_pipe[num_processus-1][0],0); // pipe_avant -> stdin
                        } else {
                            int new_in = dup2(in_desc,0);                       // ind_desc -> stdin
                        }
                        /* Ecrire dans le prochain pipe sauf pour le dernier */
                        if ( num_processus != nb_commande-1){
                            close(tableau_pipe[num_processus][0]);              // Ecrire dans le pipe suivant
                            int desc = dup2(tableau_pipe[num_processus][1],1);  // stdout -> pipe_suivant
                        } else {
                            int dupdesc2 = dup2(out_desc,1);                    // stdout -> out_desc 
                        }
                        /* Execution de la commande */
                        execvp(seq[num_processus][0],seq[num_processus]);
                        perror("Commande not found");
                        exit(1);
                    }
                    else {
                        count ++;
                        char * cmd;
                        cmd = (char*) malloc(strlen(seq[0][0])+1);
                        // Sauvegarder la ligne de commmande + Enregistre dans jobs
                        toString(&cmd, seq[num_processus]);
                        insertion(count, pid, ACTIF, cmd, &tab_jobs);
                    }
                }
                /* fermer tout les pipes pour le pere */
                for(int k = 0; k<nb_commande-1;k++){
                        close(tableau_pipe[k][0]);
                        close(tableau_pipe[k][1]);
                }
                if (backgrounded == NULL) {
                    foreground(pid);                
                }
                // background
            }
        }
    }
    return EXIT_SUCCESS;
}