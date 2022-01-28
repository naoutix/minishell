#ifndef __IO_H
#define __IO_H
#include <unistd.h>   /* pimitives de base : fork, ...*/
#include <stdio.h>    /* entrées sorties */
#include <fcntl.h>    /* opérations sur les fichiers */

/* Variante de open qui affiche un message d'erreur */
int xopen(const char *pathname, int flags);

/* Variante de open qui affiche un message d'erreur */
int xopen2(const char *pathname, int flags, mode_t mode);

/* Variante de read qui affiche un message d'erreur */
ssize_t xread(int fd, void *buf, size_t count);

/* Variante de close qui affiche un message d'erreur */
int xclose (int desc);

/* Variante de write qui affiche un message d'erreur */
ssize_t xwrite(int fd, const void *buf, size_t count);

#endif