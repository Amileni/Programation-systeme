#include "pse.h"
#include "tampon.h"

void *threadConsommateur(void* argc);
void *threadProducteur(void* argc);

sem_t dispoProd;
sem_t dispoCons;

int main(int argc, char *argv[]) {
    Tampon tampon;
    tampon_init(&tampon);

    sem_init(&dispoProd, P, 0);
    sem_init(&dispoCons, P, 0);

    pthread_t threadID[2];
    int ret = 0;

    ret = pthread_create(&threadID[0], NULL, threadProducteur, &tampon);
    printf("Création thread Producteur\n");

    if (ret != 0)
      erreur_IO("creation thread");

    ret = pthread_create(&threadID[1], NULL, threadConsommateur, &tampon);
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
    Tampon *tampon = (Tampon *) argc; // Cast de la valeur d'entrée en type Tampon
    int idx;

    for(idx = 0; idx < P; idx++) {              // On répète suffisament pour remplir complètement le tableau
        tampon_deposer(tampon, idx * idx + P);  // On met la valeur de idx dans le buffer
        usleep(200000);                         // Attente de 200ms
    }

    pthread_exit(NULL);
}

//**************************************************
// Fonction thread utilisé pour la consommation du tampon
// Simulation de ralentissement du thread par rapport à la production
// Prend en argument un typedef Tampon
//**************************************************

void *threadConsommateur(void* argc) {
    Tampon *tampon = (Tampon *) argc; // Cast de la valeur d'entrée en type Tampon
    int idx;
    int val;

    for(idx = 0; idx < P; idx++) {                      // On répète suffisament pour remplir complètement le tableau
        tampon_prendre(tampon, &val);
        printf("Entier lu :%d\n", val); // On met la valeur de idx dans le buffer
        sleep(1);                                       // Attente de 200ms
    }

    pthread_exit(NULL);
}