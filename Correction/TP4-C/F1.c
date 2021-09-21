// F1.c
#include "pse.h"

int main(int argc, char *argv[]) {
  printf("F1: arg = %s, pid = %d, pid pere = %d\n", argv[1], getpid(),
         getppid());
  sleep(10);
  exit(11);
}
