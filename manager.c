#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpmss.h"
#include "lpr.h"
#include "simulator.h"

void managerMain(  ) {
    int shm_fd;
    const char *key;
    carpark_t *shm;

    /*
     * We need to get the segment named
     * "SHM_TEST", created by the server.
     */
    key = SHARE_NAME;

    /*
     * Locate the segment.
     */
    if ((shm_fd = shm_open(key, O_RDWR, 0)) < 0)
    {
        perror("shm_open");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    shm = mmap(0, PARKING_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == (carpark_t *)-1)
    {
        perror("mmap");
        exit(1);
    }

    printf("Hello! I am the manager.\n");
    printf("%s\n", shm->entrances[0].sensor.plate);
    return;
}
