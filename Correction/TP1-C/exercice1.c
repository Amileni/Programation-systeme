#include "pse.h"

extern char **environ;

int main(int argc, char *argv[]) {
  int i;
  printf("argc = %d\n", argc);
  if (argc == 1) {
    erreur("%s: absence argument\n", argv[0]);
  }
  else {
    for (i = 0; i <argc; i++)
      printf("%s\n", argv[i]);
  }
  exit(EXIT_SUCCESS);
}
