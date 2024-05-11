#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>


_Atomic int counter;


void* threadfun(void *arg) {
    int somevar;
    for (int i = 0; i < 10; ++i) {
        counter++;
        printf("in thread %lu, stack near %p, pid=%d, tid=%d, counter=%d\n",
               pthread_self(), &somevar, getpid(), gettid(), counter);
        sleep(100);
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: threads THREAD_COUNT\n");
        return 1;
    }
    int N = atoi(argv[1]);
    pthread_t threads[N];
    int created = 0;
    for (; created < N; ++created) {
        int result = pthread_create(&threads[created], NULL, threadfun, NULL);
        if (result != 0) {
            perror("pthread_create");
            break;
        }
    }
    for (int i = 0; i < created; ++i) {
        pthread_join(threads[i], NULL);
    }
}
