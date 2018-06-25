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
  int count2;
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
     barrier.count2 = 0;
     barrier.mf = mf;

     sem_init(&barrier.mutex1, 0, 1);
     sem_init(&barrier.turnstile1, barrier.count1, 0);
     sem_init(&barrier.turnstile2, barrier.count2, 0);

}

/* Join Meetup
 *
 * calls write_resource() to copy the value to the barrier struct
 *  - if meetlast, only the last thread to arrive will copy
 *  - if meetfirst, only the first thread to arrive will copy
 *
 * if not enough threads have arrived (barrier.n) then wait until
 *   enough threads have arrived before copying to codeword
 *
 */
void join_meetup(char *value, int len) {

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
    } else if (barrier.mf == MEET_FIRST) {
        barrier.count1++;
        read_resource(&codeword, value, len);
    } else {
        barrier.count1++; 
    }
    printf("number of threads so far for turnstile1: %d\n", barrier.count1);
    sem_post(&barrier.mutex1);
    sem_wait(&barrier.turnstile1); // once we have n threads in a group, release the n threads

    // critical section between turnstiles
    printf("between turnstile1 and turnstile2\n");
    // read_resource(&codeword, value, len);
    barrier.count2 = barrier.n;

    /*
    sem_wait(&barrier.mutex1);
    barrier.count2--;
    printf("number of threads left for turnstile2: %d\n", barrier.count2);

    if( (barrier.count2-1) <= 0){
        printf("hallo release threads for the second time\n");
        int i;
        for(i = 0; i < barrier.n; i++){
            sem_post(&barrier.turnstile2);
        }
    }
    sem_post(&barrier.mutex1);
    sem_wait(&barrier.turnstile2);
    */
}
