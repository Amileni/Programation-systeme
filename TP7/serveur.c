#include "pse.h"

#define CMD           "serveur"
#define NOM_JOURNAL   "journal.log"
#define NB_WORKERS    3

void *threadSessionClient(void *arg);
void sessionClient(int canal);
void remiseAZeroJournal(void);
void threadsMaker(int nbWorkers);

int fdJournal;
DataSpec dataWorkers[NB_WORKERS];
sem_t semAvailable;
const int semSize = NB_WORKERS;

int main(int argc, char *argv[]) {
  short port;
  int ecoute, canal, ret, idx;
  struct sockaddr_in adrEcoute, adrClient;
  unsigned int lgAdrClient;

  // Créer tous les threads qui seront utilisés par le serveur
  threadsMaker(NB_WORKERS);

  fdJournal = open(NOM_JOURNAL, O_CREAT|O_WRONLY|O_APPEND, 0600);
  if (fdJournal == -1)
    erreur_IO("ouverture journal");

  if (argc != 2)
    erreur("usage: %s port\n", argv[0]);

  port = (short)atoi(argv[1]);

  printf("%s: creating a socket\n", CMD);
  ecoute = socket (AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0)
    erreur_IO("socket");
  
  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);
  printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind (ecoute,  (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));
  if (ret < 0)
    erreur_IO("bind");
  
  printf("%s: listening to socket\n", CMD);
  ret = listen (ecoute, 5);
  if (ret < 0)
    erreur_IO("listen");
  
  while (VRAI) {
    printf("%s: accepting a connection\n", CMD);
    lgAdrClient = sizeof(adrClient);
    canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);
    if (canal < 0)
      erreur_IO("accept");

    printf("%s: adr %s, port %hu\n", CMD,
         stringIP(ntohl(adrClient.sin_addr.s_addr)),
         ntohs(adrClient.sin_port));

    idx = 0;
    while (VRAI) {
      printf("Is worker %d available ?\n", idx);

      if (dataWorkers[idx].canal == -1) {
        dataWorkers[idx].canal = canal;
        sem_post(&dataWorkers[idx].sem);

        break;
      }

      idx++;
      if (idx == NB_WORKERS)
        idx = 0;
      
      usleep(1000);
    }
  }

  if (close(ecoute) == -1)
    erreur_IO("fermeture ecoute");  

  if (close(fdJournal) == -1)
    erreur_IO("fermeture journal");  

  exit(EXIT_SUCCESS);
}

//********************************
// void threadsMaker(int nbWorkers);
// Aloue tous les worker au nombre de nbWorkers
// Init les DataSpec.canal des threads à -1 pour être lue comme libre
// Init les DataSpec.sem à 0 pour que le thread voit qu'il n'a pas de ressources
//********************************

void threadsMaker(int nbWorkers) {
  int idx = 0;
  int ret = 0;

  for(idx = 0; idx < nbWorkers; idx++) {
    dataWorkers[idx].canal = -1;
    sem_init(&dataWorkers[idx].sem, 0, 0);

    ret = pthread_create(&dataWorkers[idx].id, NULL, threadSessionClient, &dataWorkers[idx]);
    printf("Création thread n°%d\n", idx);

    if (ret != 0)
      erreur_IO("creation thread");
  }
}

//****************************************
// void *threadSessionClient(void *arg);
// Fonction utilisé par les threads
// Prend en argument un struct DataSpec qui contient le canal
// Utilise le semAvailable pour determiner les ressources dispo
//****************************************

void *threadSessionClient(void *arg) {
  DataSpec* dataSpec = (DataSpec *)arg;

  while(VRAI) {
    sem_wait(&dataSpec->sem);
    
    sessionClient(dataSpec->canal);

    dataSpec->canal = -1;
  }

  pthread_exit(NULL);
}

// session d'echanges avec un client
// fermeture du canal a la fin de la session
void sessionClient(int canal) {
  int fin;
  char ligne[LIGNE_MAX];
  int lgLue, lgEcr;

  fin = FAUX;
  while (!fin) {
    lgLue = lireLigne(canal, ligne);
    if (lgLue == -1)
      erreur_IO("lecture ligne");

    if (lgLue == 0) { // connexion fermee  => arret brutal du client
      printf("serveur: arret du client\n");
      fin = VRAI;
    }
    else if (strcmp(ligne, "fin") == 0) {
      printf("serveur: fin session client\n");
      fin = VRAI;
    }
    else if (strcmp(ligne, "init") == 0) {
      printf("serveur: remise a zero du journal\n");
      remiseAZeroJournal();
    }
    else {
      lgEcr = ecrireLigne(fdJournal, ligne); 
      if (lgEcr == -1)
        erreur_IO("ecriture journal");
      printf("serveur: ligne de %d octets ecrite dans le journal\n", lgEcr);
    }
  }

  if (close(canal) == -1)
    erreur_IO("fermeture canal");  
}

void remiseAZeroJournal(void) {
  // on ferme le fichier et on le rouvre en mode O_TRUNC

  if (close(fdJournal) == -1)
    erreur_IO ("fermeture jornal pour remise a zero");

  fdJournal = open(NOM_JOURNAL, O_TRUNC|O_WRONLY|O_APPEND, 0600);
  if (fdJournal == -1)
    erreur_IO ("reouverture journal pour remise a zero journal");
}
