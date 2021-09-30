#include "pse.h"

#define CMD         "serveur"
#define NOM_JOURNAL "journal.log"
#define NB_WORKERS  2

// acces concurrent au canal d'un worker : la recherche d'un worker libre par
// le thread principal peut être en concurrence avec la remise du canal a -1
// par le worker
// => utilisation d'un mutex par worker; on l'ajoute dans la structure DataSpec

// acces concurrent au descripteur du journal : un worker qui utilise le
// descripteur pour ecrire dans le journal peut etre en concurrence avec un
// worker qui remet le journal a zero (invalidation du descripteur par close,
// modification du descripteur par reouverture)
// => utilisation d'un mutex

void creerCohorteWorkers(void);
int chercherWorkerLibre(void);
void *threadWorker(void *arg);
void sessionClient(int canal);
void remiseAZeroJournal(void);
void init_semaphore(sem_t *sem, int n);
void wait_semaphore(sem_t *sem);
void post_semaphore(sem_t *sem);
void lock_mutex(pthread_mutex_t *mutex);
void unlock_mutex(pthread_mutex_t *mutex);

int fdJournal;
pthread_mutex_t mutexJournal = PTHREAD_MUTEX_INITIALIZER;
DataSpec dataWorkers[NB_WORKERS];
sem_t semWorkersLibres;

int main(int argc, char *argv[]) {
  short port;
  int ecoute, canal, ret;
  struct sockaddr_in adrEcoute, adrClient;
  unsigned int lgAdrClient;
  int numWorkerLibre;

  if (argc != 2)
    erreur("usage: %s port\n", argv[0]);

  port = (short)atoi(argv[1]);

  fdJournal = open(NOM_JOURNAL, O_CREAT|O_WRONLY|O_APPEND, 0644);
  if (fdJournal == -1)
    erreur_IO("ouverture journal");

  creerCohorteWorkers();
  init_semaphore(&semWorkersLibres, NB_WORKERS);

  printf("%s: creating a socket\n", CMD);
  ecoute = socket(AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0)
    erreur_IO("socket");
  
  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);
  printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind(ecoute, (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));
  if (ret < 0)
    erreur_IO("bind");
  
  printf("%s: listening to socket\n", CMD);
  ret = listen(ecoute, 5);
  if (ret < 0)
    erreur_IO("listen");

  while (VRAI) {
    printf("%s: accepting a connection\n", CMD);
    lgAdrClient = sizeof(adrClient);
    canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);
    if (canal < 0)
      erreur_IO("accept");

    printf("%s: adr %s, port %hu\n", CMD,
        stringIP(ntohl(adrClient.sin_addr.s_addr)), ntohs(adrClient.sin_port));

    wait_semaphore(&semWorkersLibres);
    numWorkerLibre = chercherWorkerLibre();

    dataWorkers[numWorkerLibre].canal =  canal;
    post_semaphore(&dataWorkers[numWorkerLibre].sem);
  }

  if (close(ecoute) == -1)
    erreur_IO("fermeture ecoute");

  if (close(fdJournal) == -1)
    erreur_IO("fermeture journal");

  exit(EXIT_SUCCESS);
}

void creerCohorteWorkers(void) {
  int i, ret;

  for (i = 0; i < NB_WORKERS; i++) {
    dataWorkers[i].canal = -1;
    dataWorkers[i].tid = i;
    ret = pthread_create(&dataWorkers[i].id, NULL, threadWorker,
                         &dataWorkers[i]);
    if (ret != 0)
      erreur_IO("creation worker");

    ret = pthread_mutex_init(&dataWorkers[i].mutex, NULL);
    if (ret != 0)
      erreur_IO("init mutex worker");
  }
}

// retourne le no. du worker libre trouve ou -1 si pas de worker libre
int chercherWorkerLibre(void) {
  int i;
  int canal;

  for (i = 0; i < NB_WORKERS; i++) {
    lock_mutex(&dataWorkers[i].mutex);
    canal = dataWorkers[i].canal;
    unlock_mutex(&dataWorkers[i].mutex);

    if (canal == -1)
      return i;
  }

  return -1;
}

void *threadWorker(void *arg) {
  DataSpec *dataSpec = (DataSpec *)arg; 

  while (VRAI) {
    wait_semaphore(&dataSpec->sem);
    printf("worker %d: reveil\n", dataSpec->tid);

    sessionClient(dataSpec->canal);

    printf("worker %d: sommeil\n", dataSpec->tid);

    lock_mutex(&dataSpec->mutex);
    dataSpec->canal = -1;
    unlock_mutex(&dataSpec->mutex);

    post_semaphore(&semWorkersLibres);
  }

  pthread_exit(NULL);
}

void sessionClient(int canal) {
  int fin = FAUX;
  char ligne[LIGNE_MAX];
  int lgLue, lgEcr;

  while (!fin) {
    lgLue = lireLigne(canal, ligne);
    if (lgLue == -1)
      erreur_IO("lecture canal");

    else if (lgLue == 0) {  // arret du client (CTRL-D, interruption)
      fin = VRAI;
      printf("%s: arret du client\n", CMD);
    }
    else {  // lgLue > 0
      if (strcmp(ligne, "fin") == 0) {
        fin = VRAI;
        printf("%s: fin session client\n", CMD);
      }
      else if (strcmp(ligne, "init") == 0) {
        remiseAZeroJournal();
        printf("%s: remise a zero du journal\n", CMD);
      }
      else {
        lock_mutex(&mutexJournal);
        lgEcr = ecrireLigne(fdJournal, ligne);
        unlock_mutex(&mutexJournal);
        if (lgEcr < 0)
          erreur_IO("ecriture journal");
        printf("%s: ligne de %d octets ecrite dans journal\n", CMD, lgEcr);
      }
    }
  }

  if (close(canal) == -1)
    erreur_IO("fermeture canal");
}

void remiseAZeroJournal(void) {
  lock_mutex(&mutexJournal);

  if (close(fdJournal) < 0)
    erreur_IO("fermeture journal pour remise a zero");

  fdJournal = open("journal.log", O_TRUNC|O_WRONLY|O_APPEND);
  if (fdJournal < 0)
    erreur_IO("reouverture journal");

  unlock_mutex(&mutexJournal);
}

void init_semaphore(sem_t *sem, int n) {
  int ret;
  ret = sem_init(sem, 0, n);
  if (ret == -1)
    erreur_IO("init semaphore");
}

void wait_semaphore(sem_t *sem) {
  int ret;
  ret = sem_wait(sem);
  if (ret == -1)
    erreur_IO("wait semaphore");
}

void post_semaphore(sem_t *sem) {
  int ret;
  ret = sem_post(sem);
  if (ret == -1)
    erreur_IO("post semaphore");
}

void lock_mutex(pthread_mutex_t *mutex) {
  int ret;
  ret = pthread_mutex_lock(mutex);
  if (ret == -1)
    erreur_IO("lock mutex");
}

void unlock_mutex(pthread_mutex_t *mutex) {
  int ret;
  ret = pthread_mutex_unlock(mutex);
  if (ret == -1)
    erreur_IO("unlock mutex");
}
