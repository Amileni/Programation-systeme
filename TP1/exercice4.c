#include "pse.h"

int main(int argc, char *argv[]) {
  /* requete DNS */
  
  struct sockaddr_in *adresse;
  char* IP;
  int i = 0;

  adresse = resolv(argv[1], argv[2]);
  IP = stringIP(adresse->sin_addr.s_addr);


  for (i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }

  printf("%s\n", IP);

  exit(EXIT_SUCCESS);
}
