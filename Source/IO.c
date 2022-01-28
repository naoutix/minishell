#include "IO.h"
#include <stdlib.h>

int xopen(const char *pathname, int flags){
  int op = open(pathname,flags);
  if (op <0){
    perror("Erreur open");
  }
  return op;
}

int xopen2(const char *pathname, int flags, mode_t mode){
  int op = open(pathname, flags, mode);
  if (op <0){
    perror("Erreur open");
  }

  return op;
}

ssize_t xread(int fd, void *buf, size_t count){

  ssize_t rd = read(fd, buf, count);
  if (rd <0){
    perror("Erreur read");
  }

  return rd;
}

int xclose (int desc){
  int cl = close (desc);

  if (cl <0){
    perror("Error close");
  }

  return cl;
}