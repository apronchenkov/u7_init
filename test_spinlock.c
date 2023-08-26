// Stats for the custom implementation for macbook air m2:
//

// test_spinlock 1
// 0.789
// 0.790
// 0.787
// 0.790
// 0.789
// 0.788
// 0.789
// 0.789
// 0.789
// 0.789
// 0.791
// 0.790
// 0.790

// ./test_spinlock 2
// 6.994
// 6.984
// 7.028
// 7.010
// 6.987
// 6.983
// 6.984
// 6.987
// 6.952
// 6.924
// 6.997
// 7.003
// 6.881
// 6.980

// ./test_spinlock 3
// 6.902
// 6.915
// 6.978
// 6.960
// 7.103
// 6.908
// 7.141
// 6.691
// 6.658
// 6.922
// 7.049
// 7.111
// 7.171
// 7.086

// ./test_spinlock 4
// 7.110
// 7.089
// 7.036
// 7.105
// 7.075
// 7.125
// 7.161
// 7.162
// 7.037
// 7.118
// 7.151
// 7.178
// 7.100

#include "@/public/spinlock.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static long long count = 100000000;
static u7_spinlock spinlock = U7_SPINLOCK_INIT;

static void* run(void* arg) {
  (void)arg;
  long long cnt = 0;
  for (;;) {
    u7_spinlock_lock(&spinlock);
    if (count == 0) {
      printf("%lld\n", cnt);
      u7_spinlock_unlock(&spinlock);
      break;
    }
    count -= 1;
    u7_spinlock_unlock(&spinlock);
    cnt += 1;
  }
  return NULL;
}

static void usage(void) {
  fprintf(stderr, "usage: number_of_threads [count]\n\n");
  exit(-1);
}

int main(int argc, char** argv) {
  if (argc == 1 || argc > 3) {
    usage();
  }
  unsigned int number_of_threads = 1;
  if (sscanf(argv[1], "%u", &number_of_threads) != 1) {
    usage();
  }
  if (argc == 3 && sscanf(argv[2], "%lld", &count) != 1) {
    usage();
  }
  pthread_t* thread_ids =
      (pthread_t*)malloc(number_of_threads * sizeof(pthread_t));
  for (unsigned int i = 0; i < number_of_threads; ++i) {
    pthread_create(&thread_ids[i], NULL, &run, NULL);
  }
  for (unsigned int i = 0; i < number_of_threads; ++i) {
    pthread_join(thread_ids[i], NULL);
  }
  return 0;
}
