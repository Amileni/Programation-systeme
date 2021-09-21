#include "pse.h"

#define NOM_FIFO    "fifo"
#define CMD   "client"

int main(int argc, char *argv[]) {
  int fin = FAUX;

  int sock, ret;
  struct sockaddr_in *adrServ;
  char ligne[LIGNE_MAX];
  int lgEcr;
  

  if (argc != 3)
    erreur("usage: %s machine port\n", argv[0]);

  printf("%s: creating a socket\n", CMD);
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    erreur_IO("socket");

  printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  adrServ = resolv(argv[1], argv[2]);
  if (adrServ == NULL)
    erreur("adresse %s port %s inconnus\n", argv[1], argv[2]);

  printf("%s: connecting the socket\n", CMD);
  ret = connect(sock, (struct sockaddr *)adrServ, sizeof(struct sockaddr_in));
  if (ret < 0)
    erreur_IO("connect");

  while (!fin) {
    printf("ligne> ");
    if (fgets(ligne, LIGNE_MAX, stdin) == NULL) {
      printf("arret par CTRL-D\n");

      printf("ligne> ");
      if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
        printf("arret par CTRL-D\n");

      else {
        lgEcr = ecrireLigne(sock, ligne);
        if (lgEcr == -1)
          erreur_IO("ecrireLigne");
      
        printf("%s: %d bytes sent\n", CMD, lgEcr);
      }

      fin = VRAI;
    }

    else {
      lgEcr = ecrireLigne(sock, ligne);
      if (lgEcr == -1)
        erreur_IO("ecrireLigne");
      
      printf("%s: %d bytes sent\n", CMD, lgEcr);

      if (strcmp(ligne, "fin\n") == 0 || strcmp(ligne, "stop\n") == 0)  // ecrireLigne a ajoute \n
        fin = VRAI;
    }
  }

  if (close(sock) == -1)
    erreur_IO("close");

  exit(EXIT_SUCCESS);
}
