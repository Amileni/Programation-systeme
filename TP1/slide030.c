#include "pse.h"

extern char **environ;

int main(int argc, char *argv[]) {
  printf("argc = %d\n", argc);
  if (argc == 1) {
    erreur("%s: absence argument\n", argv[0]);
  }
  exit(EXIT_SUCCESS);
}
