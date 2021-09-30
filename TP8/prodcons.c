#include "pse.h"
#include "tampon.h"

void *threadConsommateur(void* argc);
void *threadProducteur(void* argc);
void threadsMakerProd(int nbProd, pthread_t* idThreadProd, int time);
void threadsMakerConso(int nbConso, pthread_t* idThreadConso, int time);

Tampon tampon;

sem_t dispoProd;
sem_t dispoCons;

unsigned int nbLoop;

int main(int argc, char *argv[]) {
    tampon_init(&tampon);

    sem_init(&dispoProd, P, P);
    sem_init(&dispoCons, P, 0);

    pthread_t threadID[2];
    int ret = 0;

    int prodTime = atoi(argv[1]);
    int consoTime = atoi(argv[2]);
    nbLoop = atoi(argv[3]);

    ret = pthread_create(&threadID[0], NULL, threadProducteur, &prodTime);
    printf("Création thread Producteur\n");

    if (ret != 0)
      erreur_IO("creation thread");

    ret = pthread_create(&threadID[1], NULL, threadConsommateur, &consoTime);
    printf("Création thread Producteur\n");

    if (ret != 0)
      erreur_IO("creation thread");

    pthread_join(threadID[0], NULL);
    printf("Thread producteur terminé\n");

    pthread_join(threadID[1], NULL);
    printf("Thread consommateur terminé\nFin du programme\n");
}

//**************************************************
// Fonction thread utilisé pour la production dans tampon
// Simulation de ralentissement du thread, toujours plus rapide que la conso
// Prend en argument un typedef Tampon
//**************************************************

void *threadProducteur(void* argc) {
    unsigned int *time = (unsigned int *) argc; // Cast de la valeur d'entrée en type Tampon
    int idx;

    printf("time for prod : %d\n", *time);

    for(idx = 0; idx < nbLoop; idx++) {              // On répète suffisament pour remplir complètement le tableau
        sem_wait(&dispoProd);
        tampon_deposer(&tampon, idx * idx + P);  // On met la valeur de idx dans le buffer
        sem_post(&dispoCons);
        usleep(*time);                         // Attente de 200ms
    }

    pthread_exit(NULL);
}

//**************************************************
// Fonction thread utilisé pour la consommation du tampon
// Simulation de ralentissement du thread par rapport à la production
// Prend en argument un typedef Tampon
//**************************************************

void *threadConsommateur(void* argc) {
    unsigned int *time = (unsigned int *) argc; // Cast de la valeur d'entrée en type Tampon
    int idx;
    int val;

    printf("time for conso : %d\n", *time);

    for(idx = 0; idx < nbLoop; idx++) {                      // On répète suffisament pour remplir complètement le tableau
        sem_wait(&dispoCons);
        tampon_prendre(&tampon, &val);
        sem_post(&dispoProd);
        printf("Conso N°%lu : Entier lu :%d\n", pthread_self(),val); // On met la valeur de idx dans le buffer
        usleep(*time);                                       // Attente de 200ms
    }

    pthread_exit(NULL);
}

//********************************
// void threadsMakerConso(int nbConso);
// Aloue tous les worker au nombre de nbWorkers
// Prend en parametre : - idThreadProd : le tableau de threadID
//                      - time : le temps que doit prendre le prod
//********************************

void threadsMakerProd(int nbProd, pthread_t* idThreadProd, int time) {
  int idx = 0;
  int ret = 0;

  for(idx = 0; idx < nbProd; idx++) {
    ret = pthread_create(&idThreadProd[idx], NULL, threadProducteur, &time);
    printf("Création thread n°%d\n", idx);

    if (ret != 0)
      erreur_IO("creation thread");
  }
}

//********************************
// void threadsMakerConso(int nbConso);
// Aloue tous les worker au nombre de nbWorkers
// Prend en parametre : - idThreadConso : le tableau de threadID
//                      - time : le temps que doit prendre la conso
//********************************

void threadsMakerConso(int nbConso, pthread_t* idThreadConso, int time) {
  int idx = 0;
  int ret = 0;

  for(idx = 0; idx < nbConso; idx++) {
    ret = pthread_create(&idThreadConso[idx], NULL, threadConsommateur, &time);
    printf("Création thread n°%d\n", idx);

    if (ret != 0)
      erreur_IO("creation thread");
  }
}