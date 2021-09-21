// exercice2.c
#include "pse.h"

void dialogueCalcul(void);

int main(void) {
  char nomFic[20];
  int p;
  char *args[3];
  int ret;
  char reponse;

  printf("nom fichier : ");
  scanf("%s", nomFic);

  p = fork();
  if (p == -1)
    erreur_IO("creation fils");
  else if (p == 0) {
    args[0] = "gedit";
    args[1] = nomFic;
    args[2] = NULL;
    execv("/usr/bin/gedit", args);
    erreur_IO("exec gedit");
  }
  else {
    dialogueCalcul();

    // waitpid sans attente pour voir si gedit est termine ou pas
    ret = waitpid(p, NULL, WNOHANG);
    if (ret == -1)
      erreur_IO("test si edition terminee");

    else if (ret > 0)
      printf("edition terminee\n");

    else { // ret nul => gedit non termine
      printf("voulez-vous attendre la fin de l'edition ? ");
      scanf(" %c", &reponse);
      if (reponse == 'o') {
        ret = wait(NULL);
        if (ret == -1)
          erreur_IO("attente fin edition");
        printf("edition terminee\n");
      }
    }
  }

  exit(EXIT_SUCCESS);
}

void dialogueCalcul(void) {
  int n1, n2;

  printf("entrer deux nombres : ");
  scanf("%d%d", &n1, &n2);
  printf("produit : %d\n", n1 * n2);
}
