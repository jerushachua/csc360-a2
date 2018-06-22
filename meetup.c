/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 *
 * n is the meet_size
 * mf is MEET_FIRST (1) or MEET_LAST (0)
 */

typedef struct {
  int n;
  int count;
  sem_t mutex;
  sem_t turnstile1;
  sem_t turnstile2;
} barrier_t;

barrier_t barrier;

void initialize_meetup(int n, int mf) {
    char label[100];
    int i;

    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
     sem_init(&barrier.mutex, 0, 1);
     sem_init(&barrier.turnstile1, 0, 0);
     sem_init(&barrier.turnstile2, 0, 0);

}


void join_meetup(char *value, int len) {

    // part 1
    sem_wait(&barrier.mutex);
    if(++barrier.count == barrier.n){
        int i;
        for(i = 0; i < barrier.n; i++){
            sem_post(&barrier.turnstile1);
        }
    }
    sem_post(&barrier.mutex);
    sem_wait(&barrier.turnstile1);

    // part 2
    sem_wait(&barrier.mutex);
    if(--barrier.count == 0){
        int i;
        for(i = 0; i < barrier.n; i++){
            sem_post(&barrier.turnstile2);
        }
    }
    sem_post(&barrier.mutex);
    sem_wait(&barrier.turnstile2);
}
