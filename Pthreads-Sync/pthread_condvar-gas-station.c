/*
 * Conditional Variables & Mutexes: Gas Station example.
 *
 * Ref:
 *   https://code-vault.net/lesson/18ec1942c2da46840693efe9b51ff44a
 *  Video: https://www.youtube.com/watch?v=0sVGnxg6Z3k
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* Configuration */
#define NUM_FILL_LOOPS            5  // Fill gas tank n-times
#define AMOUNT_FILLED_PER_LOOP   15

// Need this much amount of fuel available in tank before any one can pull fill
#define MIN_FUEL_AVAILABLE       40

// # of threads filling gas tank
#define NUM_FILLERS              2

// # of cars drawing fuel at on time. (Think lanes)
#define NUM_CARS                 NUM_FILLERS

pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int fuel = 0;

/*
 * Producer: Fill fuel into fuel tank at gas station.
 *
 * This actor will fill a certain of amount of gas into the tank, looping
 * around n-times.
 */
void *
fuel_filling(void * arg)
{
    for (int i = 0; i < NUM_FILL_LOOPS; i++) {
        pthread_mutex_lock(&mutexFuel);
        fuel += AMOUNT_FILLED_PER_LOOP;
        printf("Filled fuel... %d\n", fuel);
        pthread_mutex_unlock(&mutexFuel);
        pthread_cond_signal(&condFuel);
        sleep(1);
    }
    return NULL;
}

void *
car(void * arg)
{
    pthread_mutex_lock(&mutexFuel);
    while (fuel < MIN_FUEL_AVAILABLE) {
        printf("No fuel. Waiting...\n");
        pthread_cond_wait(&condFuel, &mutexFuel);
        // Equivalent to:
        // pthread_mutex_unlock(&mutexFuel);
        // wait for signal on condFuel
        // pthread_mutex_lock(&mutexFuel);
    }
    fuel -= MIN_FUEL_AVAILABLE;
    printf("Got fuel. Now left: %d\n", fuel);
    pthread_mutex_unlock(&mutexFuel);
    return NULL;
}

int
main(int argc, char * argv[])
{
    pthread_t th[NUM_FILLERS];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&condFuel, NULL);
    for (int i = 0; i < NUM_FILLERS; i++) {
        if (i == 1) {
            if (pthread_create(&th[i], NULL, &fuel_filling, NULL) != 0) {
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &car, NULL) != 0) {
                perror("Failed to create thread");
            }
        }
    }

    for (int i = 0; i < NUM_FILLERS; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);
    return 0;
}
