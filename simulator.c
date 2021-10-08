#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "cpmss.h"
#include "simulator.h"

// shared memory libs
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <sys/shm.h>       
#include <fcntl.h>   
#include <stddef.h>       


pthread_mutex_t plateGenerationMutex;
#define SHARE_NAME "PARKING"
#define PARKING_SIZE 2920
#define NUM_LEVELS 5

void simulatorMain() {
    shared_memory_t shm;
    create_shared_object( &shm, SHARE_NAME );
    
}

bool create_shared_object( shared_memory_t* shm, const char* share_name ) {
    printf("CREATING SHARED MEMORY\n");
    // Remove any previous instance of the shared memory object, if it exists.
    shm_unlink(share_name);
    
    shm -> name = share_name;

    // Create the shared memory object, allowing read-write access, and saving the
    // resulting file descriptor in shm->fd. If creation failed, ensure 
    // that shm->data is NULL and return false.
    int shm_fd = shm_open(share_name, O_CREAT | O_RDWR, 0666);
    shm -> fd = shm_fd; 
    if (shm_fd < 0) {
        shm -> data = NULL;
        return false;
    }

    // Set the capacity of the shared memory object via ftruncate. If the 
    // operation fails, ensure that shm->data is NULL and return false. 
    
    if (ftruncate(shm->fd, PARKING_SIZE) != 0){
        shm->data = NULL;
        return false;
    }

    // Otherwise, attempt to map the shared memory via mmap, and save the address
    // in shm->data. If mapping fails, return false.
    // shm -> data = mmap(0, PARKING_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); 

    // if (shm->data == MAP_FAILED)
    // {
    //     return false;
    // }


    lpr_sensor_t *lpr_sensor = mmap(NULL, sizeof(lpr_sensor_t), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
 
    int position = sizeof(lpr_sensor_t);

    // NULL address ensures this is inserted appropriately into shared memory - the address is chosen by the OS
    boom_gate_t *boom_gate= mmap(NULL, sizeof(boom_gate_t), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    // NULL TERMINATED STRING
    printf("LPR SENSOR:\n");
    char* plate = "123XXX";
    strcpy(lpr_sensor->plate, plate);
    printf("%s\n", lpr_sensor->plate);

    printf("BOOM GATE:\n");
    char* status = "O";
    strcpy(boom_gate->status, status);
    printf("%s\n", boom_gate->status);



    printf("lock offset = %ld byte in address structure.\n",
    offsetof(lpr_sensor_t, lock));
    printf("cond offset = %ld byte in address structure.\n",
    offsetof(lpr_sensor_t, cond));
    printf("plate offset = %ld byte in address structure.\n",
    offsetof(lpr_sensor_t, plate));
    printf("padding offset = %ld byte in address structure.\n",
    offsetof(lpr_sensor_t, padding));
    

    int size_lpr = sizeof(lpr_sensor_t);
    printf("LPR SENSOR: %d\n", size_lpr);
    int boom_gate_s = sizeof(boom_gate_t);
    printf("LPR SENSOR: %d\n", boom_gate_s);
    int info_sign = sizeof(information_sign_t);
    printf("LPR SENSOR: %d\n", info_sign);
    int entrance = sizeof(entrance_t);
    printf("LPR SENSOR: %d\n", entrance);
    int exit = sizeof(exit_t);
    printf("LPR SENSOR: %d\n", exit);
    int level = sizeof(level_t);
    printf("LPR SENSOR: %d\n", level);

    // int memory_offset = 0;
    // for (int i = 0; i < NUM_LEVELS; i++) {
    //     // ATTACH MEMORY

        
    //     memory_offset + sizeof(lpr_sensor_t);
    // }


    // char* shm_addr;
    // shm_addr = shmat(shm_fd, NULL, 0);
    // if (!shm_addr) { /* operation failed. */
    //     perror("shmat: ");
    //     exit(1);
    // }
    // printf("ATTACHED\n");

    // struct lpr_sensor {
    //     char* name;
    //     int number;
    // };

    // struct info_sign {
    //     pthread_mutex_t info_lock
    // }

    // sizeof(56) = 56;

    // int* sensors_num;
    // struct lpr_sensor* sensors;
    
    // sensors_num = (int*) shm_addr;
    // sensors_num = 0;
    // printf("ADDRESSED\n");
    // sensors = (struct lpr_sensor*) ((void*)shm_addr + sizeof(int));
    // printf("SIZED\n");

    // strcpy(sensors[0].name, "hello");
    // printf("HELLER\n");
    // sensors->number = 0;

    // (*sensors_num)++;

    // sensors[1].name = "world";
    // sensors->number = 1;
    // (*sensors_num)++;

    // for (int i = 0; i < (*sensors_num); i++) {
    //     printf("SENSOR %d: \n", i+1);
    //     printf("    -> NAME %s: \n", sensors[i].name);
    //     printf("    -> NUMBER %d: \n", sensors[i].number);
    // }




    // int* shm_id = shmat(shm_fd, NULL, 0);
    // lpr_sensor* pData = (lpr_sensor *) shm_id;
    // pData -> a = shmat(share_name, 0, 0);
    // pData -> a = shmat(shm_fd, 0, 0);
    // int * aInt = pData -> a;
    // printf("%ls", aInt);

    printf("DONE CREATING SHARED MEMORY\n");
    return true;
}

bool get_shared_object( shared_memory_t* shm, const char* share_name ) {
    // Get a file descriptor connected to shared memory object and save in 
    // shm->fd. If the operation fails, ensure that shm->data is 
    // NULL and return false.
    int shm_fd = shm_open(share_name, O_RDWR, 0);
    shm -> fd = shm_fd;
    if (shm_fd < 0) 
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    // Otherwise, attempt to map the shared memory via mmap, and save the address
    // in shm->data. If mapping fails, return false.
    // INSERT SOLUTION HERE
    shared_data_t *map = mmap(0, 48, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = map;

    if (map == MAP_FAILED)
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    // Modify the remaining stub only if necessary.
    return true;
}

void simulatorTest(  ) {
    printf("Hello! I am the simulator");
    return;
}

char *generatePlate(  ) {
    // Mutex locks to protect the global random
    pthread_mutex_lock(&plateGenerationMutex);

    // Uses current time as the random seed
    char *plate = (char *)malloc(6);
    for (int i = 0; i < 3; i++) {
        int randIntNumber = rand();
        int randIntLetter = rand();

        // Generate random integers as a part of the plate
        randIntNumber %= 10;
        randIntNumber += 48;
        char randNumberConverted = randIntNumber;
        plate[i] = randNumberConverted;

        // Generate random letters as a part of the plate
        randIntLetter %= 26;
        randIntLetter += 65;
        char randLetterConverted = randIntLetter;
        plate[i+3] = randLetterConverted;

    }
    pthread_mutex_unlock(&plateGenerationMutex);
    return plate;

}

