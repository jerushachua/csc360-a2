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
 * count is the number of threads that have arrived at the barrier
 * mf is MEET_FIRST (1) or MEET_LAST (0)
 * codeword is the word to be shared to all threads
 */

typedef struct {
  int n;
  int count;
  int mf;
  char* codeword;
  sem_t mutex;
  sem_t turnstile1;
  sem_t turnstile2;
} barrier_t;

barrier_t barrier;
static resource_t codeword;

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
     barrier.n = n;
     barrier.count = 0;
     barrier.mf = mf;

}

/* Join Meetup
 *
 * calls write_resource() to copy the value to the barrier struct
 *  - if meetlast, only the last thread to arrive will copy
 *  - if meetfirst, only the first thread to arrive will copy
 *
 * if not enough threads have arrived (barrier.n) then wait until
 *   enough threads have arrived before copying to barrier.codeword
 *
 */
void join_meetup(char *value, int len) {

    sem_wait(&barrier.mutex);
    if( (barrier.count == 0) && (barrier.mf == MEET_FIRST) ){
        barrier.codeword = value;
        write_resource(&codeword, value, len);
        printf("stored value: %s\n", barrier.codeword);
    }
    if(++barrier.count == barrier.n){
        if(barrier.mf == MEET_LAST){
            barrier.codeword = value;
            write_resource(&codeword, value, len); 
            printf("stored value: %s\n", barrier.codeword);
        }
        int i;
        for(i = 0; i < barrier.n; i++){
            sem_post(&barrier.turnstile1);
        }
    }
    sem_post(&barrier.mutex);
    sem_wait(&barrier.turnstile1);


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
