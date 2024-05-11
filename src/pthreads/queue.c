#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t nonempty = PTHREAD_COND_INITIALIZER;


struct node {
    struct node *next;
    int data;
};

struct node* list = NULL;


int pop() {
    pthread_mutex_lock(&mutex);

    while (list == NULL) {
        pthread_cond_wait(&nonempty, &mutex);   // atomically: mutex_unlock(); sleep(); mutex_lock(); 
    }

    int data = list->data;
    struct node* next = list->next;
    free(list);
    list = next;

    pthread_mutex_unlock(&mutex);

    return data;
}

void push(int data) {
    struct node *new = calloc(sizeof(*new), 1);
    new->data = data;

    pthread_mutex_lock(&mutex);
    new->next = list;
    list = new;
    if (!list->next) {
        pthread_cond_signal(&nonempty);
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

    for (int i = 0; i < created; ++i) {
        pthread_join(threads[i], NULL);
    }
}
