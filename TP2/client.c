#include "pse.h"

#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    int outputFd;
    char* outFile = "fifo";
    char buf[BUF_SIZE];

    while(1) {
        outputFd = open(outFile, O_WRONLY, 0644);

        fgets(buf, sizeof(buf), stdin);

        ecrireLigne(outputFd, buf);
        printf("Ligne : %s\n", buf);
        close(outputFd);
    }
}