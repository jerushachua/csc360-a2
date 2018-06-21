/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "rw.h"
#include "resource.h"

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must START here.
 */

static resource_t data;
static pthread_mutex_t m;
static pthread_cond_t readerQ;
static pthread_cond_t writerQ;
int readers = 0;
int writers = 0;

/*
 * Initialize the shared structures, including those used for
 * synchronization.
 */
void initialize_readers_writer() {
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&readerQ, NULL);
    pthread_cond_init(&writerQ, NULL);

}

/* Reader
 *
 *    as long as there is no thread writing the resource, this function will
 *      read from data and copy it to value using read_resource().
 *
 *    if there are threads writing, then calling rw_read() is blocked until
 *      the writers are finished.
 */

void rw_read(char *value, int len) {
    pthread_mutex_lock(&m);

    while( !(writers == 0)){
        pthread_cond_wait(&readerQ, &m);
    }
    readers++;
    pthread_mutex_unlock(&m);

    // read here
    printf("reading! \n");

    if(--readers == 0){
        pthread_cond_signal(&writerQ);
    }
    pthread_mutex_unlock(&m);

}

/* Writer
 *
 *    as long as there is no thread reading the resource, this function will
 *      write the value into data using write_resource().
 *
 *    if there are threads reading, then calling rw_write() is blocked until
 *      the readers are finished.
 */
void rw_write(char *value, int len) {
    pthread_mutex_lock(&m);

    while(!(readers == 0) & (writers ==0)){
        pthread_cond_wait(&writerQ, &m);
    }
    writers++;
    pthread_mutex_unlock(&m);

    // write here
    printf("writing! \n");

    pthread_mutex_lock(&m);
    writers--;
    pthread_cond_signal(&writerQ);
    pthread_broadcast_signal(&readerQ);
    pthread_mutex_unlock(&m);

}
