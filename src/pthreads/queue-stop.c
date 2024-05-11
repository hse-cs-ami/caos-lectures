#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t nonempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;


struct node {
    struct node *next;
    int data;
};

struct node* list = NULL;
_Bool done = 0;

int pop() {
    // unlocked
    pthread_mutex_lock(&mutex);
    // locked
    while (!list && !done) {
        printf("in thread %d: waiting\n", gettid());
        pthread_cond_wait(&nonempty, &mutex);
        printf("in thread %d: awake\n", gettid());
    }
    int data;
    if (list) {
        data = list->data;
        struct node* next = list->next;
        free(list);
        list = next;

        if (list == NULL) {
            pthread_cond_signal(&empty);
        }
        pthread_mutex_unlock(&mutex);
        return data;
    } else {
        printf("in thread %d: stopping\n", gettid());
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }
}

void push(int data) {
    struct node *new = calloc(sizeof(*new), 1);
    new->data = data;

    pthread_mutex_lock(&mutex);
    if (done) {
        fprintf(stderr, "queue stopped\n");
        free(new);
        pthread_mutex_unlock(&mutex);
        return;
    }
    new->next = list;
    list = new;
    if (!list->next) {
        printf("signaling nonempty\n");
        pthread_cond_signal(&nonempty);
    }
    pthread_mutex_unlock(&mutex);
}

void stop() {
    pthread_mutex_lock(&mutex);
    done = 1;
    printf("broadcasting nonempty\n");
    pthread_cond_broadcast(&nonempty);
    while (list) {
        pthread_cond_wait(&empty, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void* threadfun(void *arg) {
    while (1) {
        int i = pop();
        printf("in thread %d: popped value %d\n", gettid(), i);
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s THREAD_COUNT\n", argv[0]);
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
        printf("%d threads\n", created);
    }

    for (int i = 0; i < 100; ++i) {
        push(i);
    }
    stop();

    for (int i = 0; i < created; ++i) {
        pthread_join(threads[i], NULL);
    }
}
