# minishell
Un makefile a été créé afin de faciliter la compilation et l’exécution de mon programme. 
Exécutez dans un shell les commandes suivantes dans le répertoire source de l’archive :

make
./minishell

Ou éventuellement avec gdb :

gdb ./minishell

Sinon utiliser :

gcc -Wall -Wextra --std=c99 -g processus.c readcmd.c minishell.c cd.c -o minishell
./minishell

Cette archive contient:
- un dossier livrables ou vous trouverez des documents nécessaire à la comprehension de mon travail
- un dossier source ou vous trouverez tous les fichiers sources.