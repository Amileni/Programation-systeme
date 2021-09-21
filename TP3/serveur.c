#include "pse.h"

#define NOM_FIFO      "fifo"
#define NOM_JOURNAL   "journal.log"
#define CMD           "serveur"

void remiseAZeroJournal(void);

int fdJournal;


int main(int argc, char *argv[]) {
  int stop = FAUX;
  int fin = FAUX;

  short port;
  int ecoute, canal, ret;
  struct sockaddr_in adrEcoute, adrClient;
  unsigned int lgAdrClient;
  char ligne[LIGNE_MAX];
  int lgLue, lgEcr;

  fdJournal = open(NOM_JOURNAL, O_CREAT|O_WRONLY|O_APPEND, 0600);
  if (fdJournal == -1)
    erreur_IO("ouverture journal");

  if (argc != 2)
    erreur("usage: %s port\n", argv[0]);

  port = (short)atoi(argv[1]);

  printf("%s: creating a socket\n", CMD);
  ecoute = socket (AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0)
    erreur_IO("socket");
  
  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);

  printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind (ecoute,  (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));

  printf("%s: listening to socket\n", CMD);
  ret = listen (ecoute, 5);

  if (ret < 0)
    erreur_IO("listen");

  while (!stop) {

    fin = FAUX;

    printf("%s: accepting a connection\n", CMD);
    lgAdrClient = sizeof(adrClient);
    canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);

    if (canal < 0)
      erreur_IO("accept");

    printf("%s: adr %s, port %hu\n", CMD,
          stringIP(ntohl(adrClient.sin_addr.s_addr)),
          ntohs(adrClient.sin_port));

    if (ret < 0)
      erreur_IO("bind");
    
    while(!fin) {
    
      lgLue = lireLigne(canal, ligne);
      
      if (lgLue < 0)
        erreur_IO("lireLigne");

      if (lgLue == 0) { // fifo ferme par l'ecrivain => arret brutal du client
        printf("serveur: arret du client\n");
        stop = VRAI;
      }

      else if (strcmp(ligne, "stop") == 0) {
        printf("Arret du serveur\n");
        stop = VRAI;
        fin = VRAI;
      }

      else if (strcmp(ligne, "fin") == 0) {
        printf("serveur: fin demandee\n");
        if (close(canal) == -1)
          erreur_IO("close canal");
        
        fin = VRAI;
      }

      else if (strcmp(ligne, "init") == 0) {
        printf("serveur: remise a zero du journal\n");
        remiseAZeroJournal();
      }

      else {
        lgEcr = ecrireLigne(fdJournal, ligne); 
        if (lgEcr == -1)
          erreur_IO("ecriture journal");
        printf("serveur: ligne de %d octets ecrite dans le journal\n", lgEcr);
      }
    }
  }

  if (close(canal) == -1)
    erreur_IO("close canal");

  if (close(ecoute) == -1)
    erreur_IO("close ecoute");

  if (close(fdJournal) == -1)
    erreur_IO("fermeture journal");  

  exit(EXIT_SUCCESS);
}

void remiseAZeroJournal(void) {
  // on ferme le fichier et on le rouvre en mode O_TRUNC
  if (close(fdJournal) == -1)
    erreur_IO ("fermeture jornal pour remise a zero");

  fdJournal = open(NOM_JOURNAL, O_TRUNC|O_WRONLY|O_APPEND, 0600);
  if (fdJournal == -1)
    erreur_IO ("reouverture journal pour remise a zero");
}
