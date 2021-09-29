#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpmss.h"
#include "lpr.h"

// Setting number of buckets to 100
#define BUCKETS_SIZE 100
size_t buckets = BUCKETS_SIZE;
htab_t h;

void managerTest(  ) {
    printf("Hello! I am the manager.");
    return;
}

int platesInit(  ) {
    if (!htab_init(&h, buckets))
    {
        printf("failed to initialise hash table\n");
        return EXIT_FAILURE;
    }
    return 0;
} 

// https://stackoverflow.com/questions/3501338/c-read-file-line-by-line

void readPlates( const char * filename, const char * mode ) {
    platesInit();
    FILE* fp;
    int count = 0;
    fp = fopen(filename, mode);
    if (fp == NULL) {
        perror("Failed: ");
        return;
    }

    char buffer[256];

    while (fgets(buffer, sizeof buffer, fp)) {
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n", buffer);
        char *plate = (char *)malloc(sizeof(char)*100);  
        strcpy(plate, buffer);
        htab_add(&h, plate, count);
        
        // printf("%s\n HASH: ", plate);
        // fgets(plate, 100 - 1, fp);
        
        count++;
    }

    htab_print(&h);
    item_print(htab_find(&h, "480GML"));

    fclose(fp);
    return;
}
