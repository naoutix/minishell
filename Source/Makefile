#Compilateur
CC =c99
#Cflag
CFLAGS =-Wextra -g -c
#Lflags
LFLAGS =-Wall -Wextra -g
#
#Fichier executable
EXE =minishell

all: $(EXE) 

minishell: processus.o readcmd.o minishell.o cd.o IO.o
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm *.o $(EXE)

depend:
	makedepend *.c -Y.

.PHONY: clean depend all
# DO NOT DELETE

IO.o: IO.h
cd.o: cd.h
minishell.o: processus.h cd.h IO.h readcmd.h
processus.o: processus.h
readcmd.o: readcmd.h
