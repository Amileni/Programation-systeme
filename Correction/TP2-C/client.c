#include "pse.h"

#define NOM_FIFO    "fifo"

int main(void) {
  int fdFifo;
  int fin = FAUX;
  char ligne[LIGNE_MAX];

  // lorsque le client ecrit dans le fifo alors que le serveur a ete arrete
  // il recoit le signal SIGPIPE qui par defaut met fin brutalement au
  // processus sans message d'erreur;
  // avec l'instruction qui suit on ignore ce signal, ecrireLigne sortira
  // en echec et le client affichera un message d'erreur
  signal(SIGPIPE, SIG_IGN);

  fdFifo = open(NOM_FIFO, O_WRONLY);
  if (fdFifo == -1)
    erreur_IO("ouverture fifo");

  while (!fin) {
    printf("ligne> ");
    if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
      // saisie de CTRL-D
      fin = VRAI;
    else {
      if (ecrireLigne(fdFifo, ligne) == -1)
        erreur_IO("ecriture fifo");

      if (strcmp(ligne, "fin\n") == 0)  // ecrireLigne a ajoute \n
        fin = VRAI;
    }
  }

  if (close(fdFifo) == -1)
    erreur_IO("fermeture fifo");

  exit(EXIT_SUCCESS);
}
