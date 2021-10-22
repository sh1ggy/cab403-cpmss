#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   

#include "cpmss.h"
#include "shm.h"
#include "plates.h"

shared_memory_t shm;

void *managerMain(  ) {
    get_shared_object(&shm, SHARE_NAME);
    while(1) {
        if (flagPlateFound) {
            printf("\nHello! I am the manager.\n");
            // printf("FILE DESCRIPTOR: %d\n",shm.fd);
            printf("LEVEL 1 -> ENTRANCE 1 -> LPR -> PLATE: %s\n", shm.data->entrances[0].sensor.plate);
            printf("LEVEL 2 -> ENTRANCE 2 -> LPR -> PLATE: %s\n", shm.data->entrances[1].sensor.plate);
            printf("LEVEL 3 -> ENTRANCE 3 -> LPR -> PLATE: %s\n", shm.data->entrances[2].sensor.plate);
            printf("LEVEL 4 -> ENTRANCE 4 -> LPR -> PLATE: %s\n", shm.data->entrances[3].sensor.plate);
            printf("LEVEL 5 -> ENTRANCE 5 -> LPR -> PLATE: %s\n", shm.data->entrances[4].sensor.plate);

            printf("LEVEL 1 -> LPR -> PLATE: %s\n", shm.data->levels[0].sensor.plate);
            printf("LEVEL 2 -> LPR -> PLATE: %s\n", shm.data->levels[1].sensor.plate);
            printf("LEVEL 3 -> LPR -> PLATE: %s\n", shm.data->levels[2].sensor.plate);
            printf("LEVEL 4 -> LPR -> PLATE: %s\n", shm.data->levels[3].sensor.plate);
            printf("LEVEL 5 -> LPR -> PLATE: %s\n", shm.data->levels[4].sensor.plate);

            printf("LEVEL 1 -> EXIT 1 -> LPR -> PLATE: %s\n", shm.data->exits[0].sensor.plate);
            printf("LEVEL 2 -> EXIT 2 -> LPR -> PLATE: %s\n", shm.data->exits[1].sensor.plate);
            printf("LEVEL 3 -> EXIT 3 -> LPR -> PLATE: %s\n", shm.data->exits[2].sensor.plate);
            printf("LEVEL 4 -> EXIT 4 -> LPR -> PLATE: %s\n", shm.data->exits[3].sensor.plate);
            printf("LEVEL 5 -> EXIT 5 -> LPR -> PLATE: %s\n", shm.data->exits[4].sensor.plate);

            

            for (int i = 0; i < 5; i++) {
                printf("LEVEL %d CAR CAPACITY: %d\n", i + 1, level[i]);
            }
            
            sleep(msSleep(50)); // TODO: MUTEX
        }
    }
    return 0;
}
