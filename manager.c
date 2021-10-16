#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpmss.h"
#include "lpr.h"
#include "shm.h"
#include "simulator.h"

void managerMain(  ) {
    shared_memory_t shm;

    get_shared_object(&shm, SHARE_NAME);
    printf("Hello! I am the manager.\n");
    printf("%s\n", shm.data->entrances[0].sensor.plate);
    return;
}
