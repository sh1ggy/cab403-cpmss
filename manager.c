#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpmss.h"
#include "shm.h"
#include "plates.h"
#include "cars.h"

shared_memory_t shm;

void *managerMain()
{
    get_shared_object(&shm, SHARE_NAME);
    while (1)
    {
        fflush(0);
        if (flagPlateFound)
        {
            printf("----------------ENTRANCES----------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
                // sleep(msSleep(20));
                printf("%d - PLATE: %s BOOM GATE: %c INFO: %c\n", i+1, shm.data->entrances[i].sensor.plate, shm.data->entrances[i].gate.status, shm.data->entrances[i].sign.status);
                pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
            }

            printf("----------------LEVELS----------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->levels[i].sensor.lock);
                // sleep(msSleep(20));
                printf("%d - PLATE: %s CAPACITY: %d\n", i+1, shm.data->levels[i].sensor.plate, level[i]);
                pthread_mutex_unlock(&shm.data->levels[i].sensor.lock);
            }

            printf("----------------EXIT----------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->exits[i].sensor.lock);
                // sleep(msSleep(20));
                printf("%d - PLATE: %s BOOM GATE: %c\n", i+1, shm.data->levels[i].sensor.plate, shm.data->exits[i].gate.status);
                pthread_mutex_unlock(&shm.data->exits[i].sensor.lock);
            }

            printf("----------------TOTAL BILL----------------\n");
            // sleep(msSleep(20));
            printf("$%.2f\n", carBill);

            sleep(msSleep(50)); 
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
