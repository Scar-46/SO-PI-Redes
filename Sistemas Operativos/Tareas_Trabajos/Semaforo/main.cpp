#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "semaphore.h"

int main( int argc, char * argv[] ) {
  Semaphore s(0);
  if (fork()) {
    s.Wait();
    printf("Si\n");
  } else {
    printf("No\n");
    s.Signal();
  }
}

