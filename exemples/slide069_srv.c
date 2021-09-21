#include "pse.h"

#define    CMD      "serveur"

int main(int argc, char *argv[]) {
  short port;
  int ecoute, canal, ret;
  struct sockaddr_in adrEcoute, adrClient;
  unsigned int lgAdrClient;
  char ligne[LIGNE_MAX];
  int lgLue;
  
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
  if (ret < 0)
    erreur_IO("bind");
  
  printf("%s: listening to socket\n", CMD);
  ret = listen (ecoute, 5);
  if (ret < 0)
    erreur_IO("listen");
  
  printf("%s: accepting a connection\n", CMD);
  lgAdrClient = sizeof(adrClient);
  canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);
  if (canal < 0)
    erreur_IO("accept");

  printf("%s: adr %s, port %hu\n", CMD,
         stringIP(ntohl(adrClient.sin_addr.s_addr)),
         ntohs(adrClient.sin_port));
  
  lgLue = lireLigne(canal, ligne);
  if (lgLue < 0)
    erreur_IO("lireLigne");

  else if (lgLue == 0)
    printf("%s: arret du client\n", CMD);
  
  else
    printf("%s: reception %d octets : \"%s\"\n", CMD, lgLue, ligne);

  if (close(canal) == -1)
    erreur_IO("close canal");

  if (close(ecoute) == -1)
    erreur_IO("close ecoute");

  exit(EXIT_SUCCESS);
}
