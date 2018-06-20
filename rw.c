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
int writers = 0;
int readers = 0;

/*
 * Initialize the shared structures, including those used for
 * synchronization.
 */
void initialize_readers_writer() {
    pthread_mutex_init(&m, NULL);

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
    pthread_once(&m, initialize_readers_writer);
    pthread_mutex_lock(&m);

    /*
    while( !(writers == 0)){
        pthread_cond_wait()
        // work here
        pthread_mutex_unlock(&m);
    }
    */

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
    printf("NOTHING IMPLEMENTED YET FOR rw_write\n");
}
