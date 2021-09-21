#include "pse.h"

void *maFonction(void *arg) {
  char *chaine = (char *)arg;
  printf ("chaine recue: %s\n", chaine);
  pthread_exit(NULL);
}

int main(void) {
  pthread_t idThread;

  int ret = pthread_create(&idThread, NULL, maFonction, "Bonjour a tous");
  if (ret != 0)
    erreur_IO("pthread_create");  

  pthread_join(idThread, NULL);
  exit(EXIT_SUCCESS);
}
