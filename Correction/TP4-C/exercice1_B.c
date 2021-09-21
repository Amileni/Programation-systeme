// exercice1_B.c
#include "pse.h"

void attendreFinFils(int pidFils1, int pidFils2);

int main(void) {
  int p1, p2;
  char *args1[] = {"F1", "bonjour", NULL};
  char *args2[] = {"F2", NULL};

  p1 = fork();
  if (p1 == -1) {
    erreur_IO("creation fils 1");
  }
  else if (p1 == 0) { // fils 1
    execv("F1", args1);
    erreur_IO("exec F1");
  }
  else { // pere
    p2 = fork();
    if (p2 == -1) {
      erreur_IO("creation fils 2");
    }
    else if (p2 == 0) {
      execv("F2", args2);
      erreur_IO("exec F2");
    }
    else {
      printf("pere: pid = %d, pid fils 1 = %d, pid fils 2 = %d\n", getpid(),
             p1, p2);
      attendreFinFils(p1, p2);
      attendreFinFils(p1, p2);
    }
  }

  exit(EXIT_SUCCESS);
}

// attend qu'un fils se termine, puis affiche lequel s'est termine ainsi que
// son code d'exit
void attendreFinFils(int pidFils1, int pidFils2) {
  int p, status;

  p = wait(&status);
  if (p == -1)
    erreur_IO("attente fin fils");
  else if (p == pidFils1)
    printf("pere: fin fils1");
  else if (p == pidFils2)
    printf("pere: fin fils2");
  else
    printf("pere: fin fils, pid inconnu\n");

  printf(", code exit %d\n", WEXITSTATUS(status));
}
