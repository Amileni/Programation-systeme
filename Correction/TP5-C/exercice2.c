// exercice2.c
#include "pse.h"

#define NB_THREADS  10

void *fonctionThread(void *arg);

int numThread[NB_THREADS];
int resultat[NB_THREADS];

int main(void) {
  pthread_t idThread[NB_THREADS];
  int i;
  int ret;

  for (i= 0; i < NB_THREADS; i++) {
    numThread[i] = i + 1;
    ret = pthread_create(&idThread[i], NULL, fonctionThread, &numThread[i]);
    if (ret != 0)
      erreur_IO("pthread_create");
  }

  for (i= 0; i < NB_THREADS; i++) {
    ret = pthread_join(idThread[i], NULL);
    if (ret != 0)
      erreur_IO("pthread_join");
    printf("somme de 1 a %d = %d\n", i + 1, resultat[i]);
  }

  exit(EXIT_SUCCESS);
}

void *fonctionThread(void *arg) {
  int *pNum;
  int num;

  pNum = (int *)arg;
  num = *pNum;
  resultat[num - 1] = (num * (num + 1)) / 2; 

  pthread_exit(NULL);
}
