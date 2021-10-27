#include "lpr.h"
#include "simulator.h"

// Setting number of buckets to 100 according to the plates.txt file
#define BUCKETS_SIZE 100
size_t buckets = BUCKETS_SIZE;

void item_print(item_t *i)
{
    printf("key=%s value=%d", i->key, i->value);
}

// Initialising the hash table to be created
bool htab_init(htab_t *h, size_t n)
{
    h->size = n;
    h->buckets = (item_t **)calloc(n, sizeof(item_t *));
    return h->buckets != 0;
}

size_t djb_hash(char *s)
{
    size_t hash = 5381;
    int c;
    while ((c = *s++) != '\0')
    {
        // hash = hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

size_t htab_index(htab_t *h, char *key)
{
    return djb_hash(key) % h->size;
}

item_t *htab_bucket(htab_t *h, char *key)
{
    return h->buckets[htab_index(h, key)];
}

// Searching the hash table to find a particular key value
item_t *htab_find(htab_t *h, char *key)
{
    for (item_t *i = htab_bucket(h, key); i != NULL; i = i->next)
    {
        if (strcmp(i->key, key) == 0)
        { // found the key
            return i;
        }
    }
    return NULL;
}

// Adding values to the hash table
bool htab_add(htab_t *h, char *key, int value)
{
    // allocate new item
    item_t *newhead = (item_t *)malloc(sizeof(item_t));
    if (newhead == NULL)
    {
        return false;
    }
    newhead->key = key;
    newhead->value = value;

    // hash key and place item in appropriate bucket
    size_t bucket = htab_index(h, key);
    newhead->next = h->buckets[bucket];
    h->buckets[bucket] = newhead;
    return true;
}

// Printing the hash table created and its values stored
void htab_print(htab_t *h)
{
    printf("hash table with %ld buckets\n", h->size);
    for (size_t i = 0; i < h->size; ++i)
    {
        printf("bucket %ld: ", i);
        if (h->buckets[i] == NULL)
        {
            printf("empty\n");
        }
        else
        {
            for (item_t *j = h->buckets[i]; j != NULL; j = j->next)
            {
                item_print(j);
                if (j->next != NULL)
                {
                    printf(" -> ");
                }
            }
            printf("\n");
        }
    }
}

// Destroying the hash table created
void htab_destroy(htab_t *h)
{
    // free linked lists
    for (size_t i = 0; i < h->size; ++i)
    {
        item_t *bucket = h->buckets[i];
        while (bucket != NULL)
        {
            item_t *next = bucket->next;
            free(bucket);
            bucket = next;
        }
    }
    // free buckets array
    free(h->buckets);
    h->buckets = NULL;
    h->size = 0;
}

// Initialise the hash table for plates to be stored
int platesInit(  ) {
    if (!htab_init(&h, buckets))
    {
        printf("failed to initialise hash table\n");
        return EXIT_FAILURE;
    }
    return 0;
} 

// Read the file and add each line to the hash table
void readPlates( const char * filename, const char * mode ) {
    platesInit();
    
    // Opening file
    FILE* fp;
    int count = 0;
    fp = fopen(filename, mode);
    if (fp == NULL) {
        perror("Failed: ");
        return;
    }

    // Going through each line of the file 
    char buffer[256];
    while (fgets(buffer, sizeof buffer, fp)) {
        buffer[strcspn(buffer, "\n")] = 0;
        char *plate = (char *)malloc(sizeof(char)*100);  
        strcpy(plate, buffer);
        htab_add(&h, plate, count);
        count++;
    }

    // Closing file
    fclose(fp);
    return;
}

// Reading the plates.txt file to retrieve a license plate to be used
bool randLine ( char *plate ) {
    // Generate random line to get a plate from 
    int line;
    line = generateInRange( 1, 100 );
    
    // Opening file
    FILE *fp = fopen("plates.txt", "r");
    int count = 0;
    if ( fp == NULL) {
        return false;
    }

    // Read through each line of file
    char buffer[256]; 
    while (fgets(buffer, sizeof buffer, fp) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = 0;
        if (count == line)
        {
            // Close file
            fclose(fp);
            strcpy(plate, buffer);

            return true;
        }
        else
        {
            count++;
        }
    }
    return false;
}