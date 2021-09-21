#include "pse.h"

extern char **environ;

int main(int argc, char *argv[]) {

  int i = 0;

  printf("argc = %d\n", argc);
  if (argc == 1) {
    erreur("%s: absence argument\n", argv[0]);
  }

  for (i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }
  exit(EXIT_SUCCESS);
}
