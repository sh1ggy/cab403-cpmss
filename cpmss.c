#include "cpmss.h"
#include "lpr.h"
#include "simulator.h"
#include "shm.h"

int main( int argc, char **argv )
{     
    shared_memory_t shm;
    create_shared_object( &shm, SHARE_NAME );
    
    pthread_mutexattr_t attrmutex;
    pthread_mutexattr_init(&attrmutex);
    
    pthread_condattr_t attrcond;
    pthread_condattr_init(&attrcond);
    

    for (int i = 0; i < 5; i++) {
        pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&shm.data->entrances[i].sensor.lock, &attrmutex);
        pthread_mutex_init(&shm.data->levels[i].sensor.lock, &attrmutex);
        pthread_mutex_init(&shm.data->exits[i].sensor.lock, &attrmutex);

        pthread_condattr_setpshared(&attrcond, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(&shm.data->entrances[i].sensor.cond, &attrcond);
        pthread_cond_init(&shm.data->levels[i].sensor.cond, &attrcond);
        pthread_cond_init(&shm.data->exits[i].sensor.cond, &attrcond);
    }
    
    
    
    htab_destroy(&h);

    sleep(1);    

    pthread_t simulatorThread, managerThread;

    pthread_create(&simulatorThread, NULL, simulatorMain, NULL );
    pthread_create(&managerThread, NULL, managerMain, NULL );
        
    // Join runs the thread(function) after it exits once (simulator never exits so we dont have to join, manager exits so we join it)
    pthread_join( simulatorThread, NULL );
    pthread_join( managerThread, NULL );

    // simulatorMain();
    // managerMain();
    destroy_shared_object(&shm);

}