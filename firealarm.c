#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include "shm.h"
#include "simulator.h"
#include "cpmss.h"
#include "plates.h"
// int shm_fd;
// volatile void *shm;

int alarm_active = 0;
pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t alarm_condvar = PTHREAD_COND_INITIALIZER;

#define LEVELS 5
#define ENTRANCES 5
#define EXITS 5

#define MEDIAN_WINDOW 5
#define TEMPCHANGE_WINDOW 30

bool globalAlarmActive = false;

struct tempnode {
	int temperature;
	struct tempnode *next;
};

struct tempnode *deletenodes(struct tempnode *templist, int after)
{
	if (templist->next) {
		templist->next = deletenodes(templist->next, after - 1);
	}
	if (after <= 0) {
		free(templist);
		return NULL;
	}
	return templist;
}
int compare(const void *first, const void *second)
{
	return *((const int *)first) - *((const int *)second);
}

void *tempmonitor(void *level)
{
	sleep(msSleep(250));

	struct tempnode *templist = NULL, *newtemp, *medianlist = NULL, *oldesttemp;
	int count, mediantemp, hightemps; //addr
	int *tempLevel = (int *)level;

	if (shm.data->levels[*tempLevel].temperature_sensor == 0)
	{
		return 0;
	}
	for (;;) {
		sleep(msSleep(50));
		// Add temperature to beginning of linked list
		newtemp = malloc(sizeof(struct tempnode));
		newtemp->temperature = shm.data->levels[*tempLevel].temperature_sensor; 
		newtemp->next = templist;
		templist = newtemp;
		
		// Delete nodes after 5th
		deletenodes(templist, MEDIAN_WINDOW);
		
		// Count nodes
		count = 0;
		for (struct tempnode *t = templist; t != NULL; t = t->next) {
			count++;
		}
		if (count == MEDIAN_WINDOW) { // Temperatures are only counted once we have 5 samples
			int *sorttemp = malloc(sizeof(int) * MEDIAN_WINDOW);
			count = 0;
			for (struct tempnode *t = templist; t != NULL; t = t->next) {
				sorttemp[count++] = t->temperature;
			}
			qsort(sorttemp, MEDIAN_WINDOW, sizeof(int), compare);
			mediantemp = sorttemp[(MEDIAN_WINDOW - 1) / 2];
			
			// Add median temp to linked list
			newtemp = malloc(sizeof(struct tempnode));
			newtemp->temperature = mediantemp;
			newtemp->next = medianlist; 
			medianlist = newtemp;
			
			// Delete nodes after 30th
			deletenodes(medianlist, TEMPCHANGE_WINDOW);
			
			// Count nodes
			count = 0;
			hightemps = 0;
			
			for (struct tempnode *t = medianlist; t != NULL; t = t->next) { 
				// Temperatures of 58 degrees and higher are a concern
				if (t->temperature >= 58) hightemps++;
				// Store the oldest temperature for rate-of-rise detection
				oldesttemp = t; 
				count++;
			}
			
			if (count == TEMPCHANGE_WINDOW) {
				// If 90% of the last 30 temperatures are >= 58 degrees,
				// this is considered a high temperature. Raise the alarm
				if (hightemps >= TEMPCHANGE_WINDOW * 0.9){
					alarm_active = 1;
					globalAlarmActive = true;
				}
				
				// If the newest temp is >= 8 degrees higher than the oldest
				// temp (out of the last 30), this is a high rate-of-rise.
				// Raise the alarm
				if (templist->temperature - oldesttemp->temperature >= 8)
				{
					alarm_active = 1;
					globalAlarmActive = true;
				}
			}
		}
		usleep(2000);
	}
	return 0;
}

void *openboomgate(void *arg)
{
	int *entrance = (int *) arg;
	pthread_mutex_lock(&shm.data->entrances[*entrance].gate.lock);
	pthread_mutex_lock(&shm.data->exits[*entrance].gate.lock);
	for (;;) {
		shm.data->entrances[0].gate.status = 'O';
		shm.data->entrances[1].gate.status = 'O';
		shm.data->entrances[2].gate.status = 'O';
		shm.data->entrances[3].gate.status = 'O';
		shm.data->entrances[4].gate.status = 'O';
		
		shm.data->exits[0].gate.status = 'O';
		shm.data->exits[1].gate.status = 'O';
		shm.data->exits[2].gate.status = 'O';
		shm.data->exits[3].gate.status = 'O';
		shm.data->exits[4].gate.status = 'O';

		level[0] = 0;
		level[1] = 0; 
		level[2] = 0;
		level[3] = 0; 
		level[4] = 0;

	}

	pthread_mutex_unlock(&shm.data->entrances[*entrance].gate.lock);
	pthread_mutex_unlock(&shm.data->exits[*entrance].gate.lock);

}

void *fireAlarmMain()
{
	pthread_t *threads = malloc(sizeof(pthread_t) * LEVELS);
	
	for (int i = 0; i < 4; i++) {
			pthread_create(threads + i, NULL, tempmonitor, &i);
	}
	for (;;) {
		if (alarm_active) {

			// Handle the alarm system and open boom gates
			// Activate alarms on all levels
			for (int i = 0; i < LEVELS; i++) {
				shm.data->levels[i].alarm = true;
				strcpy(shm.data->exits[i].sensor.plate, shm.data->levels[i].sensor.plate);
			}
			
			// Open up all boom gates
			pthread_t *boomgatethreads = malloc(sizeof(pthread_t) * (ENTRANCES + EXITS));
			for (int i = 0; i < ENTRANCES; i++) {
				pthread_create(boomgatethreads + i, NULL, openboomgate, &i);
			}
			for (int i = 0; i < EXITS; i++) {
				pthread_create(boomgatethreads + ENTRANCES + i, NULL, openboomgate, &i);
			}
			
			// Show evacuation message on an endless loop
			for (;;) {
				sleep(msSleep(100));
				char *evacmessage = "EVACUATE ";
				shm.data->entrances[0].gate.status = 'O';
				shm.data->entrances[1].gate.status = 'O';
				shm.data->entrances[2].gate.status = 'O';
				shm.data->entrances[3].gate.status = 'O';
				shm.data->entrances[4].gate.status = 'O';
				
				shm.data->exits[0].gate.status = 'O';
				shm.data->exits[1].gate.status = 'O';
				shm.data->exits[2].gate.status = 'O';
				shm.data->exits[3].gate.status = 'O';
				shm.data->exits[4].gate.status = 'O';

				level[0] = 0;
				level[1] = 0; 
				level[2] = 0;
				level[3] = 0; 
				level[4] = 0;
				for (char *p = evacmessage; *p != '\0'; p++) {
					for (int i = 0; i < ENTRANCES; i++) {
						information_sign_t *sign = &shm.data->entrances[i].sign;
						pthread_mutex_lock(&sign->lock);
						sign->status = *p;
						pthread_mutex_unlock(&sign->lock);
					}
					usleep(20000);
				}
			}
			
			for (int i = 0; i < LEVELS; i++) {
				pthread_join(threads[i], NULL);
			}
		}
		usleep(1000);
	}
}
