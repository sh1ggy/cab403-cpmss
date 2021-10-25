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
            // printf("ENTRANCE 1 -> PLATE: %s\n", shm.data->entrances[0].sensor.plate);
            // printf("ENTRANCE 2 -> PLATE: %s\n", shm.data->entrances[1].sensor.plate);
            // printf("ENTRANCE 3 -> PLATE: %s\n", shm.data->entrances[2].sensor.plate);
            // printf("ENTRANCE 4 -> PLATE: %s\n", shm.data->entrances[3].sensor.plate);
            // printf("ENTRANCE 5 -> PLATE: %s\n", shm.data->entrances[4].sensor.plate);

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

            printf("----------------ENTRANCES----------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
                printf("%d - PLATE: %s, BOOM GATE: %c\n", i+1, shm.data->entrances[i].sensor.plate, shm.data->entrances[i].gate.status);
                pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
            }

            printf("----------------LEVELS----------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->levels[i].sensor.lock);
                printf("%d - PLATE: %s, CAPACITY: %d\n", i+1, shm.data->levels[i].sensor.plate, level[i]);
                pthread_mutex_unlock(&shm.data->levels[i].sensor.lock);
            }

            printf("----------------EXIT----------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->exits[i].sensor.lock);
                printf("%d - PLATE: %s, BOOM GATE: %c\n", i+1, shm.data->levels[i].sensor.plate, shm.data->exits[i].gate.status);
                pthread_mutex_unlock(&shm.data->exits[i].sensor.lock);
            }

            // for (int i = 0; i < 5; i++)
            // {
            //     printf("LEVEL %d CAR CAPACITY: %d\n", i + 1, level[i]);
            // }

            sleep(msSleep(200)); // TODO: MUTEX
            system("clear");
        }
    }
    return 0;
}

            // char *entrances[7];
            // char *boomGates[2];
            // char *levels[7];
            // char *exits[7];

            // for (int i = 0; i < 5; i++)
            // {
            //     pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
            //     entrances[i] = malloc(sizeof(char) * 7);
            //     strcpy(entrances[i], shm.data->entrances[i].sensor.plate);
            //     boomGates[i] = malloc(sizeof(char) * 2);

            //     strcpy(boomGates[i], shm.data->entrances[i].gate.status);
            //     // printf("LEVEL %d -> ENTRANCE %d -> LPR -> PLATE: %s\n", i+1, i, plate);
            //     pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
            // }

            // for (int i = 0; i < 5; i++)
            // {
            //     pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
            //     levels[i] = malloc(sizeof(char) * 7);
            //     strcpy(levels[i], shm.data->levels[i].sensor.plate);
            //     // printf("LEVEL %d -> LPR -> PLATE: %s\n", i+1, shm.data->levels[i].sensor.plate);
            //     pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
            // }

            // for (int i = 0; i < 5; i++)
            // {
            //     pthread_mutex_lock(&shm.data->exits[i].sensor.lock);
            //     exits[i] = malloc(sizeof(char) * 7);
            //     strcpy(exits[i], shm.data->exits[i].sensor.plate);
            //     // printf("LEVEL %d -> EXIT %d -> LPR -> PLATE: %s\n", i+1, i, shm.data->exits[i].sensor.plate);
            //     pthread_mutex_unlock(&shm.data->exits[i].sensor.lock);
            // }

            // for (int i = 0; i < 5; i++)
            // {
            //     fflush(stdout);
            //     printf("ENTRANCE: %d, PLATE: %s, BOOM GATE: %s\n", i, entrances[i], boomGates[i]);
            // }

            // for (int i = 0; i < 5; i++) {
            //     printf("LEVEL: %d, PLATE: %s\n", i, levels[i]);
            // }
            // for (int i = 0; i < 5; i++) {
            //     printf("EXIT: %d, PLATE: %s\n", i, exits[i]);
            // }
