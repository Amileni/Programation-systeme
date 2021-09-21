#include "pse.h"

#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    int inputFd;
    int outputFd;
    int run = 1;
    char* inFile = "fifo";
    char* outFile = "journal.log";
    char buf[BUF_SIZE];

    while(run) {
        inputFd = open(inFile, O_RDONLY);
        outputFd = open(outFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (inputFd == -1) {
            erreur_IO("open input");
        }

        lireLigne(inputFd, buf);
        printf("Ligne : %s\n", buf);
        close(inputFd);

        if (strcmp(buf, "fin") == 0) {
            run = 0;
        }

        if (strcmp(buf, "init") == 0) {
            outputFd = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }

        outputFd = open(outFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
        ecrireLigne(outputFd, buf);
        close(outputFd);
    }
}