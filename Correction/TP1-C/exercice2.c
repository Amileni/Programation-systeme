#include "pse.h"

int main(int argc, char *argv[]) {
  /* requete DNS */
  int code;
  struct addrinfo *infos, hints;
  struct sockaddr_in *adresse;

  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  code = getaddrinfo(argv[1], argv[2], &hints, &infos);
  if (code != 0) {
    fprintf(stderr, "Erreur: %s\n", gai_strerror(code));
    exit(EXIT_FAILURE);
  }
  adresse = (struct sockaddr_in *) infos->ai_addr;

  printf("%X\n", ntohl(adresse->sin_addr.s_addr));
  printf("%hX\n", ntohs(adresse->sin_port));

  freeaddrinfo(infos);
  exit(EXIT_SUCCESS);
}
