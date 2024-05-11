#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


_Atomic int counter;


void* threadfun(void *arg) {
    sleep(1);
    pthread_exit(NULL);
    return NULL;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s THREAD_COUNT STACK_PAGES\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    int stack_pages = atoi(argv[2]);

    pthread_t threads[N];
    int created = 0;
    for (; created < N; ++created) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        int sss_result = pthread_attr_setstacksize(&attr, stack_pages * 4096);
        if (sss_result == EINVAL) {
            fprintf(stderr, "pthread_attr_setstacksize returned EINVAL\n");
            break;
        }
        int result = pthread_create(&threads[created], &attr, threadfun, NULL);
        if (result != 0) {
            perror("pthread_create");
            break;
        }
        printf("%d threads\n", created);
    }
    for (int i = 0; i < created; ++i) {
        printf("pthread_join returned %d\n", pthread_join(threads[i], NULL));
    }
}
