/*
 * pthread_lock_basic: Simplest example to show why lock() / unlock() is needed.
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Global counter of # of mails received / processed.
int mails = 0;

#define MILLION (1000 * 1000)

#define NUM_THREADS 4

pthread_mutex_t mutex;

void *
routine()
{
    // Each thread process 10M mails.
    for (int i = 0; i < 10 * MILLION; i++) {
        mails++;
        // read mails
        // increment
        // write mails
    }
    return NULL;
}

int
main(int argc, char* argv[])
{
    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[NUM_THREADS];
    for (int tctr = 0; tctr < NUM_THREADS; tctr++) {
        if (pthread_create(&threads[tctr], NULL, &routine, NULL) != 0) {
            return 1;
        }
    }
    for (int tctr = 0; tctr < NUM_THREADS; tctr++) {
        if (pthread_join(threads[tctr], NULL) != 0) {
            return 5;
        }
    }

    pthread_mutex_destroy(&mutex);

    printf("Number of mails: Expected: %d (%d M), Actual: %d (%4.2f M)\n",
            (NUM_THREADS * 10 * MILLION), (NUM_THREADS * 10),
            mails, (mails * 1.0 / MILLION));
    return 0;
}
