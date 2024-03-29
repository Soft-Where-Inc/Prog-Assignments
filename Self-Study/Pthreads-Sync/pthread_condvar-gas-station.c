/*
 * Conditional Variables & Mutexes: Gas Station example.
 *
 * Ref:    https://code-vault.net/lesson/18ec1942c2da46840693efe9b51ff44a
 *  Video: https://www.youtube.com/watch?v=0sVGnxg6Z3k
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

/* Configuration */
#define NUM_FILL_LOOPS            5  // Fill gas tank n-times
#define AMOUNT_FILLED_PER_LOOP   15

// Need this much amount of fuel available in tank before any one can pull fill
#define MIN_FUEL_AVAILABLE       40

// # of threads filling gas tank
#define NUM_FILLERS              3

// # of cars drawing fuel at on time. (Think lanes)
#define NUM_CARS                 8

// Total # of threads in the system
#define NUM_THREADS     (NUM_FILLERS + NUM_CARS)

int fuel = 0;
pthread_mutex_t mutexFuel; // To manage 'fuel' capacity
pthread_cond_t condFuel;   // To synchronize between filler & user

/*
 * Producer: Fill fuel into fuel tank at gas station.
 *
 * This actor will fill a certain of amount of gas into the tank, looping
 * around n-times.
 */
void *
fuel_filling(void * arg)
{
    int thread_id = *(int *) arg;
    while (1) {
        for (int i = 0; i < NUM_FILL_LOOPS; i++) {
            pthread_mutex_lock(&mutexFuel);
            fuel += AMOUNT_FILLED_PER_LOOP;
            printf("[PumpID=%d] Filled fuel ... Available fuel=%d\n",
                    thread_id, fuel);

            // The order of these two calls seems to not matter for overall
            // correctness of the program. As long as we unlock the mutex,
            // the thread receiving the signal can react to the signal.
            // If we don't unlock, the other thread will just block waiting
            // to acquire the lock.
            // pthread_cond_signal(&condFuel);

            pthread_cond_broadcast(&condFuel);
            pthread_mutex_unlock(&mutexFuel);
            sleep(1);
        }
    }
    return NULL;
}

/*
 * Consumer: Draw fuel from tank if sufficient fuel exists.
 *  Wait till there is enough fuel to draw from.
 */
void *
car(void * arg)
{
    int thread_id = *(int *) arg;

    pthread_mutex_lock(&mutexFuel);
    while (fuel < MIN_FUEL_AVAILABLE) {
        printf("[Car ID=%d] Available fuel=%d is inadequate. Waiting...\n",
                thread_id, fuel);

        // This seems to work ... but is not good enough.
        /*
        pthread_mutex_unlock(&mutexFuel);
        sleep(1);
        pthread_mutex_lock(&mutexFuel);
        */
        /* This single call is equivalent to:
         * pthread_mutex_unlock(&mutexFuel);
         * wait for signal on condFuel
         * pthread_mutex_lock(&mutexFuel);
         */
        pthread_cond_wait(&condFuel, &mutexFuel);
    }
    printf("[Car ID=%d] **** Available fuel=%d. Get fuel=%d, fuel left=%d. Exiting.\n",
            thread_id, fuel, MIN_FUEL_AVAILABLE, (fuel - MIN_FUEL_AVAILABLE));
    fuel -= MIN_FUEL_AVAILABLE;
    pthread_mutex_unlock(&mutexFuel);
    return NULL;
}

int
main(int argc, char * argv[])
{
    pthread_t th[NUM_THREADS]   = {0};
    int       tid[NUM_THREADS]  = {0};
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&condFuel, NULL);

    int tctr = 0;
    // Start the set of car-threads, so some of them will wait before the
    // threads that start filling fuel tanks start-up
    for (int i = 0; i < NUM_CARS; i++, tctr++) {
        tid[tctr] = tctr;
        if (pthread_create(&th[i], NULL, &car, (void *) &tid[tctr]) != 0) {
            perror("Failed to create thread for car pulling fuel");
        }
    }

    // Start n-threads filling up the fuel tank
    for (int i = 0; i < NUM_FILLERS; i++, tctr++) {
        tid[tctr] = tctr;
        if (pthread_create(&th[i], NULL, &fuel_filling, (void *) &tid[tctr]) != 0) {
            perror("Failed to create fuel-filling thread");
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);
    return 0;
}
