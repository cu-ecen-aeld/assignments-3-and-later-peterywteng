#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
//#define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    int ret = 1;
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    
    ret = usleep(thread_func_args->wait_to_obtain_ms * 1000);
    if ( ret != 0 ) {
        ERROR_LOG( "usleep failed with %d\n", ret);
        //return (void*)ret;
        return thread_param;
    }

    ret = pthread_mutex_lock(thread_func_args->mutex);
    if ( ret != 0 ) {
        ERROR_LOG( "pthread_mutex_lock failed with %d\n", ret);
        //return (void*)ret;
        return thread_param;
    }

    ret = usleep(thread_func_args->wait_to_release_ms * 1000);
    if ( ret != 0 ) {
        ERROR_LOG( "usleep failed with %d\n", ret);
        //return (void*)ret;
        return thread_param;
    }

    ret = pthread_mutex_unlock(thread_func_args->mutex);
    if ( ret != 0 ) {
        ERROR_LOG( "pthread_mutex_unlock failed with %d\n", ret);
        //return (void*)ret;
        return thread_param;
    }

    /* remember to set thread_complete_success as TRUE! */
    thread_func_args->thread_complete_success = true;

    DEBUG_LOG( "threadfunc Passed!\n");
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    int ret = 1;
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    struct thread_data* thread_data_ptr = (struct thread_data*) malloc(sizeof(struct thread_data));
    if (thread_data_ptr == NULL) {
        ERROR_LOG( "malloc failed with NULL\n");
        return false;
    }

    /* fill in the arguments into the entity */
    thread_data_ptr->mutex = mutex;
    thread_data_ptr->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_data_ptr->wait_to_release_ms = wait_to_release_ms;
    thread_data_ptr->thread_complete_success = false;

    ret = pthread_create(thread, NULL, threadfunc, thread_data_ptr);
    if ( ret != 0 ) {
        ERROR_LOG( "pthread_create failed with %d\n", ret);
        return false;
    }

    DEBUG_LOG( "start_thread_obtaining_mutex Passed!\n");
    //free(thread_data_ptr); -> it seems that thread_data_ptr will be freed during another function
    return true;
}


