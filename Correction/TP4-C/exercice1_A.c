// exercice1_A.c
#include "pse.h"

int main(void) {
  int p1, p2;

  p1 = fork();
  if (p1 == -1) {
    erreur_IO("creation fils 1");
  }
  else if (p1 == 0) {
    printf("fils 1\n");
    sleep(10);
  }
  else { // pere
    p2 = fork();
    if (p2 == -1) {
      erreur_IO("creation fils 2");
    }
    else if (p2 == 0) {
      printf("fils 2\n");
      sleep(10);
    }
    else {
      printf("pere apres creation des fils\n");
      sleep(10);
    }
  }

  exit(EXIT_SUCCESS);
}
