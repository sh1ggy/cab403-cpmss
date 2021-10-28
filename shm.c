#include "shm.h"

// Create the shared memory object and allocate memory
bool create_shared_object( shared_memory_t* shm, const char* share_name ) {
    // Remove previous instances of shared memory
    shm_unlink(share_name);
    
    // shm name set to params
    shm -> name = share_name;

    // Shared memory file descriptor initialisation & error check
    int shm_fd = shm_open(share_name, O_CREAT | O_RDWR, 0666);
    shm -> fd = shm_fd; 
    // Error check
    if (shm_fd < 0) {
        shm -> data = NULL;
        return false;
    }

    // ftruncate sets capacity of shm object + error check
    if (ftruncate(shm->fd, PARKING_SIZE) != 0){
        shm->data = NULL;
        return false;
    }

    // Shared memory carpark, data initialisation
    carpark_t *carpark = mmap(NULL, PARKING_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = carpark;
    if (shm->data == MAP_FAILED)
    {
        return false;
    }

    return true;
}

// Get shared object based on the share_name
bool get_shared_object( shared_memory_t* shm, const char* share_name ) {
    // Shared memory file descriptor initialisation & error check
    int shm_fd = shm_open(share_name, O_RDWR, 0);
    shm -> fd = shm_fd;
    if (shm_fd < 0) 
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    // Shared memory carpark, data initialisation
    carpark_t *map = mmap(0, PARKING_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = map;

    // Error check 
    if (map == MAP_FAILED)
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    return true;
}
    
// Remove the shared memory object and free memory
void destroy_shared_object( shared_memory_t* shm ) {
    munmap(&shm, 48);
    shm_unlink(shm -> name);
    shm -> fd = -1;
    shm -> data = NULL; 
}