#include "cpmss.h"
#include "shm.h"
#include "plates.h"
#include "cars.h"

shared_memory_t shm;

// The main thread function for the manager.c file
void *managerMain()
{
    // Obtain the shared object created in cpmss.c
    get_shared_object(&shm, SHARE_NAME);
    while (1)
    {
        fflush(0);
        // Printing out the manager console
        if (flagPlateFound) 
        {
            // Status display: entrances, levels, exits, total revenue
            printf("------------------ENTRANCES------------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
                printf("| %d - PLATE: %s BOOM GATE: %c INFO: %c    |\n", i+1, shm.data->entrances[i].sensor.plate, shm.data->entrances[i].gate.status, shm.data->entrances[i].sign.status);
                pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
            }

            printf("-------------------LEVELS--------------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->levels[i].sensor.lock);
                printf("| %d - PLATE: %s CAPACITY: %d TEMP: %hu°C  |\n", i+1, shm.data->levels[i].sensor.plate, level[i], shm.data->levels[i].temperature_sensor);
                pthread_mutex_unlock(&shm.data->levels[i].sensor.lock);
            }

            printf("--------------------EXIT---------------------\n");
            for (int i = 0; i < 5; i++)
            {
                pthread_mutex_lock(&shm.data->exits[i].sensor.lock);
                printf("| %d PLATE: %s BOOM GATE: %c              |\n", i+1, shm.data->exits[i].sensor.plate, shm.data->exits[i].gate.status);
                pthread_mutex_unlock(&shm.data->exits[i].sensor.lock);
            }

            printf("-----------------TOTAL BILL-----------------\n");
            sleep(msSleep(20));
            printf("                 $%.2f                    \n", carBill);

            // Sleeping the status display for 50ms as specified
            sleep(msSleep(50)); 
            // Clearing the console to redisplay updated information 
            system("clear");
        }
    }
    return 0;
}