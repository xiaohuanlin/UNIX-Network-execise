#include <pthread.h>
#include <stdio.h>

typedef struct value_pack {
    int times;
} ValuePack;

#define MAX_WORKERS 10
static int res;
static int done;
static pthread_t done_tid;
static pthread_t workers[MAX_WORKERS];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void * calculate(void *ptr) {
    ValuePack *vptr = (ValuePack *)ptr;
    int count = 0;
    for (;count < vptr->times; count++) {
        pthread_mutex_lock(&mutex);
        res += 1;
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_lock(&mutex);
    done += 1;
    done_tid = pthread_self();
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
    return NULL;
}


int main() {
    ValuePack ptr;
    ptr.times = 1e6;

    int count = 0;
    for (;count < MAX_WORKERS; count++) {
        pthread_create(&(workers[count]), NULL, calculate, (void *)&ptr);
    }

    int left_to_do = MAX_WORKERS;

    while (left_to_do > 0) {
        pthread_mutex_lock(&mutex);
        while (done == 0) {
            pthread_cond_wait(&cond, &mutex);
            pthread_join(done_tid, NULL);
            printf("this is %llu and res: %d\n", done_tid, res);
        }
        pthread_mutex_unlock(&mutex);
        done--;
        left_to_do--;
    }
}