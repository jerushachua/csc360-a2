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
 * count1 is the number of threads that have arrived at the barrier
 * mf is MEET_FIRST (1) or MEET_LAST (0)
 * codeword is the word to be shared to all threads
 * update is to let the barrier know that codeword is set
 */

typedef struct {
  int n;
  int count1;
  int mf;
  sem_t mutex1;
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
     barrier.n = n;
     barrier.count1 = 0;
     barrier.mf = mf;

     sem_init(&barrier.mutex1, 0, 1);
     sem_init(&barrier.turnstile1, 0, 0);
     sem_init(&barrier.turnstile2, 0, barrier.n);

}

/* Join Meetup
 *
 * calls write_resource() to copy the value to the barrier struct
 *  - if meetlast, only the last thread to arrive will copy
 *  - if meetfirst, only the first thread to arrive will copy
 *
 * turnstile2 only lets n threads in the room at a time.
 * starting value of turnstile2 is n.
 * starting value of turnstile1 is 0.
 *
 * the first mutex ensures that only one thread can update the thread counter at time
 *  - update the counter
 *  - if the thread is the first to arrive, write_resource() to save the codeword
 *  - if the thread is the last to arrive, also write_resource() to save the codeword
 *  - if the thread is the last to arrive, sem_post() to turnstile1 n times to make
 *    n threads allowed to go through the turnstile1
 *
 * return the first mutex before waiting at turnstile1.
 *
 * after the first turnstile1 is opened for the n threads, all threads copy over the
 *   codeword using read_resource().
 *
 * finally, sem_post() for turnstile2 to let this group of threads leave the room
 *   allows the next group to start filtering in.
 */
void join_meetup(char *value, int len) {

    sem_wait(&barrier.turnstile2);
    sem_wait(&barrier.mutex1);
    if( (barrier.count1 == 0) && (barrier.mf == MEET_FIRST) ){
        write_resource(&codeword, value, len);
        barrier.count1++;

    } else if( (barrier.count1+1) == barrier.n){
        if(barrier.mf == MEET_LAST) write_resource(&codeword, value, len);
        int i;
        for(i = 0; i < barrier.n; i++){
            printf("releasing thread\n");
            sem_post(&barrier.turnstile1);
        }
        barrier.count1 = 0;
    } else {
        barrier.count1++;
    }
    sem_post(&barrier.mutex1);
    sem_wait(&barrier.turnstile1); 

    printf("in exit room: copying value \n");
    read_resource(&codeword, value, len);

    sem_post(&barrier.turnstile2);

}
