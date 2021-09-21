#include <stdio.h>
#include <pse.h>


int main(void) {
    int p = 0;
    char* fils2Argv[2];
    char* fils2Chemin = "fils2";

    fils2Argv[0] = fils2Chemin;
    fils2Argv[1] = NULL;

    p = fork();

    if (!p) {
        printf("Je suis le fils1\n");

        execv("./fils2", *fils2Argv);
    }
    else {
        printf("Je suis le père\n");
    }
    
    //sleep (5);
}