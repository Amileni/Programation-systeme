// F2.c
#include "pse.h"

int main(int argc, char *argv[]) {
  printf("F2: pid = %d, pid pere = %d\n", getpid(), getppid());
  sleep(10);
  exit(12);
}
