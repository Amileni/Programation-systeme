#include "pse.h"

void *maFonction(void *id) {
    int* idRecu = (int* ) id;
    int somme = 0;
    int idx;

    for (idx = 0; idx < idRecu; idx++) {
        somme += idx;
    }

    printf ("bonjou %d\n", somme);
    pthread_exit(NULL);
}

int main(void) {
    pthread_t idThread[10];
    int ret;
    int idx;

    for (idx = 0; idx < 10; idx ++) {
        ret = pthread_create(&idThread[idx], NULL, maFonction, idx);
        if (ret != 0)
            erreur_IO("pthread_create");
    }
    
    for (idx = 0; idx < 10; idx++)
        pthread_join(idThread[idx], NULL);

    exit(EXIT_SUCCESS);
}
