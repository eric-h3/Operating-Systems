// ------------
// This code is provided solely for the personal and private use of
// students taking the CSC369H5 course at the University of Toronto.
// Copying for purposes other than this use is expressly prohibited.
// All forms of distribution of this code, whether as given or with
// any changes, are expressly prohibited.
//
// Authors: Bogdan Simion
//
// All of the files in this directory and all subdirectories are:
// Copyright (c) 2022 Bogdan Simion
// -------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "executor.h"

extern struct executor tassadar;


/**
 * Populate the job lists by parsing a file where each line has
 * the following structure:
 *
 * <id> <type> <num_resources> <resource_id_0> <resource_id_1> ...
 *
 * Each job is added to the queue that corresponds with its job type.
 */
void parse_jobs(char *file_name) {
    int id;
    struct job *cur_job;
    struct admission_queue *cur_queue;
    enum job_type jtype;
    int num_resources, i;
    FILE *f = fopen(file_name, "r");

    /* parse file */
    while (fscanf(f, "%d %d %d", &id, (int*) &jtype, (int*) &num_resources) == 3) {

        /* construct job */
        cur_job = malloc(sizeof(struct job));
        cur_job->id = id;
        cur_job->type = jtype;
        cur_job->num_resources = num_resources;
        cur_job->resources = malloc(num_resources * sizeof(int));

        int resource_id; 
				for(i = 0; i < num_resources; i++) {
				    fscanf(f, "%d ", &resource_id);
				    cur_job->resources[i] = resource_id;
				    tassadar.resource_utilization_check[resource_id]++;
				}
				
				assign_processor(cur_job);

        /* append new job to head of corresponding list */
        cur_queue = &tassadar.admission_queues[jtype];
        cur_job->next = cur_queue->pending_jobs;
        cur_queue->pending_jobs = cur_job;
        cur_queue->pending_admission++;
    }

    fclose(f);
}

/*
 * Magic algorithm to assign a processor to a job.
 */
void assign_processor(struct job* job) {
    int i, proc = job->resources[0];
    for(i = 1; i < job->num_resources; i++) {
        if(proc < job->resources[i]) {
            proc = job->resources[i];
        }
    }
    job->processor = proc % NUM_PROCESSORS;
}


void do_stuff(struct job *job) {
    /* Job prints its id, its type, and its assigned processor */
    printf("%d %d %d\n", job->id, job->type, job->processor);
}



/**
 * TODO: Fill in this function
 *
 * Do all of the work required to prepare the executor
 * before any jobs start coming
 * 
 */
void init_executor() {
    // initialize resource_utilization_check and resource_locks arrays
    for (int r = 0; r < NUM_RESOURCES; r++) {
        // initialize struct resource_utilization_check value
        tassadar.resource_utilization_check[r] = 0;
        // create lock for resource_lock r
        pthread_mutex_init(&tassadar.resource_locks[r], NULL);
    }
    // initialize processor_records array
    for (int p = 0; p < NUM_PROCESSORS; p++) {
        // initialize struct processor_records value
        tassadar.processor_records[p].num_completed = 0;
        // create lock for processor_record p
        pthread_mutex_init(&tassadar.processor_records[p].lock, NULL);
    }
    // initialize admission_queues array
    for (int q = 0; q < NUM_QUEUES; q++) {
        // initialize struct admission_queues values
        tassadar.admission_queues[q].pending_admission = 0;
        tassadar.admission_queues[q].admitted_jobs = malloc(QUEUE_LENGTH * sizeof(struct job*));
        tassadar.admission_queues[q].capacity = QUEUE_LENGTH;
        tassadar.admission_queues[q].num_admitted = 0;
        tassadar.admission_queues[q].head = 0;
        tassadar.admission_queues[q].tail = 0;
        // create lock for admission_queue q
        pthread_mutex_init(&tassadar.admission_queues[q].lock, NULL);
        // create conditional variables for admission and execution of jobs
        pthread_cond_init(&tassadar.admission_queues[q].execution_cv, NULL); // signal when we execute a job
        pthread_cond_init(&tassadar.admission_queues[q].admission_cv, NULL); // signal when we admit a job
    }
}

/**
 * TODO: Fill in this function
 *
 * Handles an admission queue passed in through the arg (see the executor.c file). 
 * Bring jobs into this admission queue as room becomes available in it. 
 * As new jobs are added to this admission queue (and are therefore ready to be taken
 * for execution), the corresponding execute thread must become aware of this.
 * 
 */
void *admit_jobs(void *arg) {
    struct admission_queue *q = arg;

    // ideas
    // iterate over admission_queue and keep admitting jobs until not possible
    // when we hit the maximum capacity, we wait for a conditional variable
    // execution_cv when we execute a job, we signal execution_cv to allow another
    // job through as we've created space

    // if there are jobs pending admission, keep running
    while (q->pending_admission >= 1) {
        // lock the admission queue
        pthread_mutex_lock(&q->lock);
        // did we hit the queue capacity?
        while (q->capacity == q->num_admitted) {
            // wait for a job to execute leaving space in the queue
            pthread_cond_wait(&q->execution_cv, &q->lock);
        }
        if (q->tail == q->capacity) { // check if we are out of index
            q->tail = q->tail % q->capacity;
        }
        q->admitted_jobs[q->tail] = q->pending_jobs;
        q->pending_jobs = q->pending_jobs->next;
        // increment q->tail
        q->tail += 1;
        // increment num_admitted
        q->num_admitted += 1;
        // decrement pending_admission
        q->pending_admission -= 1;
        // signal that we admitted a job
        pthread_cond_signal(&q->admission_cv);
        // unlock admission queue
        pthread_mutex_unlock(&q->lock);
    }
    return NULL;
}

/**
 * TODO: Fill in this function
 *
 * Moves jobs from a single admission queue of the executor. 
 * Jobs must acquire the required resource locks before being able to execute. 
 *
 * Note: You do not need to spawn any new threads in here to simulate the processors.
 * When a job acquires all its required resources, it will execute do_stuff.
 * When do_stuff is finished, the job is considered to have completed.
 *
 * Once a job has completed, the admission thread must be notified since room
 * just became available in the queue. Be careful to record the job's completion
 * on its assigned processor and keep track of resources utilized. 
 *
 * Note: No printf statements are allowed in your final jobs.c code, 
 * other than the one from do_stuff!
 */
void *execute_jobs(void *arg) {
    struct admission_queue *q = arg;
    
    // ideas
    // keep iterating until there aren't any jobs pending admission
    // or in the queue. If we don't have any jobs to execute, we wait
    // until a job is admitted into the queue by waiting for a admission_cv
    // signal. We call this signal when we admit a job from pending admission.
    // We execute a job by first allocating the resources needed for it
    // and then calling do_stuff. We then increment completed_jobs
    // and add the job we just finished to the processor_record array

    while ( q->pending_admission + q->num_admitted >= 1) {
        // lock admission queue
        pthread_mutex_lock(&q->lock);
        // if we don't have jobs to execute
        while (q->num_admitted == 0) {
            // wait for another job to be admitted
            pthread_cond_wait(&q->admission_cv, &q->lock);
        }
        struct job* job;
        struct processor_record* processor_record;
        if (q->head == q->capacity) { // check if we are out of index
            q->head = q->head % q->capacity;
        }
        // we have a job to execute at q->head of admitted_jobs
        job = q->admitted_jobs[q->head];
        q->head += 1; // increment head index
        for (int r = 0; r < job->num_resources; r++) {
            // lock resource needed
            pthread_mutex_lock(&tassadar.resource_locks[job->resources[r]]);
            // decrement to show that it has been acquired
            tassadar.resource_utilization_check[job->resources[r]] -= 1;
        }
        q->num_admitted -= 1;
        do_stuff(job);
        // execution done
        // unlock every resource
        for (int r = 0; r < job->num_resources; r++) {
            pthread_mutex_unlock(&tassadar.resource_locks[job->resources[r]]);
        }
        processor_record = &tassadar.processor_records[job->processor];
        pthread_mutex_lock(&processor_record->lock);
        processor_record->num_completed += 1;
        // sets the head of completed_jobs to the job we just executed
        job->next = processor_record->completed_jobs;
        processor_record->completed_jobs = job;
        // unlock processor_record
        pthread_mutex_unlock(&processor_record->lock);
        // signal that we executed a job
        pthread_cond_signal(&q->execution_cv);
        // unlock admission queue
        pthread_mutex_unlock(&q->lock);
    }
    return NULL;
}
