#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   

#include "cpmss.h"
#include "shm.h"
#include "plates.h"


void *managerMain(  ) {
    while(1) {
        if (flagPlateFound) {
            shared_memory_t shm;
            get_shared_object(&shm, SHARE_NAME);
            printf("\nHello! I am the manager.\n");
            // printf("FILE DESCRIPTOR: %d\n",shm.fd);
            printf("LEVEL 1 -> ENTRANCE 1 -> LPR -> PLATE:\n");
            printf("%s\n",shm.data->entrances[0].sensor.plate);
            printf("LEVEL 2 -> ENTRANCE 2 -> LPR -> PLATE:\n");
            printf("%s\n",shm.data->entrances[1].sensor.plate);
            printf("LEVEL 3 -> ENTRANCE 3 -> LPR -> PLATE:\n");
            printf("%s\n",shm.data->entrances[2].sensor.plate);
            printf("LEVEL 4 -> ENTRANCE 4 -> LPR -> PLATE:\n");
            printf("%s\n",shm.data->entrances[3].sensor.plate);
            printf("LEVEL 5 -> ENTRANCE 5 -> LPR -> PLATE:\n");
            printf("%s\n",shm.data->entrances[4].sensor.plate);

            for (int i = 0; i < 5; i++) {
                printf("LEVEL %d CAR CAPACITY: %d\n", i + 1, level[i]);
            }
            
            sleep(1); // TODO: MUTEX
        }
    }
    return 0;
}
