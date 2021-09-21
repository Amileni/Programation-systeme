#include "pse.h"

int main(int argc, char *argv[]) {
  struct sockaddr_in *adresse;
  char *chaineIP;

  adresse = resolv(argv[1], argv[2]);
  if (adresse == NULL) {
    printf("erreur resolv\n");
    exit(EXIT_FAILURE);
  }

  printf("%X\n", ntohl(adresse->sin_addr.s_addr));
  printf("%hX\n", ntohs(adresse->sin_port));

  chaineIP = stringIP(ntohl(adresse->sin_addr.s_addr));
  printf("%s\n", chaineIP);

  exit(EXIT_SUCCESS);
}
