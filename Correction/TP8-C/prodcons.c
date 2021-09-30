// prodcons.c
#include "pse.h"
#include "tampon.h"

#define NB_PROD   2
#define NB_CONSO  3

void *threadProd(void *arg);
void *threadConso(void *arg);
void tampon_deposer_avec_mutex(Tampon *t, int v);
void tampon_prendre_avec_mutex(Tampon *t, int *v);

int periodeProd, periodeConso;
sem_t semProd, semConso;
int numProd[NB_PROD], numConso[NB_PROD];
Tampon tampon;
pthread_mutex_t mutexDeposer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexPrendre = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
  pthread_t idProd[NB_PROD], idConso[NB_CONSO];
  int i;

  if (argc < 3)
    erreur("erreur arguments\n");
  periodeProd = atoi(argv[1]);
  periodeConso = atoi(argv[2]);

  tampon_init(&tampon);

  if (sem_init(&semProd, 0, P) == -1)
    erreur_IO("init semaphore prod");

  if (sem_init(&semConso, 0, 0) == -1)
    erreur_IO("init semaphore conso");

  for (i = 0; i < NB_PROD; i++) {
    numProd[i] = i;
    if (pthread_create(&idProd[i], NULL, threadProd, &numProd[i]) != 0)
      erreur_IO("create prod");
  }

  for (i = 0; i < NB_CONSO; i++) {
    numConso[i] = i;
    if (pthread_create(&idConso[i], NULL, threadConso, &numConso[i]) != 0)
      erreur_IO("create conso");
  }

  for (i = 0; i < NB_PROD; i++) {
    if (pthread_join(idProd[i], NULL) != 0)
      erreur_IO("join prod");
  }

  for (i = 0; i < NB_CONSO; i++) {
    if (pthread_join(idConso[i], NULL) != 0)
      erreur_IO("join conso");
  }
  
  exit(EXIT_SUCCESS);
}

// le producteur n'utilise pas son numero recu en argument
void *threadProd(void *arg) {
  int val = 0;

  while(VRAI) {
    if (sem_wait(&semProd) == -1)
      erreur_IO("wait semaphore prod");

    tampon_deposer_avec_mutex(&tampon, val);

    if (sem_post(&semConso) == -1)
      erreur_IO("post semaphore conso");

    usleep(periodeProd * 1000);
    val++;
  }

  pthread_exit(NULL);
}

void *threadConso(void *arg) {
  int *pnum;
  int num;
  int val;

  pnum = (int *)arg;
  num = *pnum;

  while(VRAI) {
    if (sem_wait(&semConso) == -1)
      erreur_IO("wait semaphore conso");

    tampon_prendre_avec_mutex(&tampon, &val);

    if (sem_post(&semProd) == -1)
      erreur_IO("post semaphore prod");
    
    printf("conso %d: %d\n", num, val);
    usleep(periodeConso * 1000);
  }

  pthread_exit(NULL);
}

void tampon_deposer_avec_mutex(Tampon *t, int v) {
  if (pthread_mutex_lock(&mutexDeposer) != 0)
    erreur_IO("lock mutex deposer");

  tampon_deposer(t, v);

  if (pthread_mutex_unlock(&mutexDeposer) != 0)
    erreur_IO("unlock mutex deposer");
}

void tampon_prendre_avec_mutex(Tampon *t, int *v) {
  if (pthread_mutex_lock(&mutexPrendre) != 0)
    erreur_IO("lock mutex prendre");

  tampon_prendre(t, v);

  if (pthread_mutex_unlock(&mutexPrendre) != 0)
    erreur_IO("unlock mutex prendre");
}
