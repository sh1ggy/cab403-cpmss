#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpmss.h"
#include "shm.h"
#include "plates.h"

shared_memory_t shm;

void *managerMain()
{
    get_shared_object(&shm, SHARE_NAME);
    while (1)
    {
        if (flagPlateFound)
        {
            // printf("\nHello! I am the manager.\n");
            // printf("FILE DESCRIPTOR: %d\n",shm.fd);
            // printf("LEVEL 1 -> ENTRANCE 1 -> LPR -> PLATE: %s\n", shm.data->entrances[0].sensor.plate);
            // printf("LEVEL 2 -> ENTRANCE 2 -> LPR -> PLATE: %s\n", shm.data->entrances[1].sensor.plate);
            // printf("LEVEL 3 -> ENTRANCE 3 -> LPR -> PLATE: %s\n", shm.data->entrances[2].sensor.plate);
            // printf("LEVEL 4 -> ENTRANCE 4 -> LPR -> PLATE: %s\n", shm.data->entrances[3].sensor.plate);
            // printf("LEVEL 5 -> ENTRANCE 5 -> LPR -> PLATE: %s\n", shm.data->entrances[4].sensor.plate);

            // printf("LEVEL 1 -> LPR -> PLATE: %s\n", shm.data->levels[0].sensor.plate);
            // printf("LEVEL 2 -> LPR -> PLATE: %s\n", shm.data->levels[1].sensor.plate);
            // printf("LEVEL 3 -> LPR -> PLATE: %s\n", shm.data->levels[2].sensor.plate);
            // printf("LEVEL 4 -> LPR -> PLATE: %s\n", shm.data->levels[3].sensor.plate);
            // printf("LEVEL 5 -> LPR -> PLATE: %s\n", shm.data->levels[4].sensor.plate);

            // printf("LEVEL 1 -> EXIT 1 -> LPR -> PLATE: %s\n", shm.data->exits[0].sensor.plate);
            // printf("LEVEL 2 -> EXIT 2 -> LPR -> PLATE: %s\n", shm.data->exits[1].sensor.plate);
            // printf("LEVEL 3 -> EXIT 3 -> LPR -> PLATE: %s\n", shm.data->exits[2].sensor.plate);
            // printf("LEVEL 4 -> EXIT 4 -> LPR -> PLATE: %s\n", shm.data->exits[3].sensor.plate);
            // printf("LEVEL 5 -> EXIT 5 -> LPR -> PLATE: %s\n", shm.data->exits[4].sensor.plate);

            // char entrances[5][7];
            // char levels[5][7];
            // char exits[5][7];

            char *entrances[7];
            char *levels[7];
            char *exits[7];

            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
                entrances[i] = malloc(sizeof(char) * 7);
                strcpy(entrances[i], shm.data->entrances[i].sensor.plate);
                // printf("LEVEL %d -> ENTRANCE %d -> LPR -> PLATE: %s\n", i+1, i, plate);
                pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
            }

            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
                levels[i] = malloc(sizeof(char) * 7);
                strcpy(levels[i], shm.data->levels[i].sensor.plate);
                // printf("LEVEL %d -> LPR -> PLATE: %s\n", i+1, shm.data->levels[i].sensor.plate);
                pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
            }

            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->exits[i].sensor.lock);
                exits[i] = malloc(sizeof(char) * 7);
                strcpy(exits[i], shm.data->exits[i].sensor.plate);
                // printf("LEVEL %d -> EXIT %d -> LPR -> PLATE: %s\n", i+1, i, shm.data->exits[i].sensor.plate);
                pthread_mutex_unlock(&shm.data->exits[i].sensor.lock);
            }

            for (int i = 0; i < 5; i++)
            {
                fflush(stdout);
                printf("ENTRANCE: %d, PLATE: %s\n", i, entrances[i]);
            }

            for (int i = 0; i < 5; i++) {
                printf("LEVEL: %d, PLATE: %s\n", i, levels[i]);
            }
            for (int i = 0; i < 5; i++) {
                printf("EXIT: %d, PLATE: %s\n", i, exits[i]);
            }

            for (int i = 0; i < 5; i++)
            {
                printf("LEVEL %d CAR CAPACITY: %d\n", i + 1, level[i]);
            }

            sleep(msSleep(500)); // TODO: MUTEX
        }
    }
    return 0;
}
