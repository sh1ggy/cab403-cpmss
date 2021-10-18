#include <stdio.h>
#include <stdlib.h>
#include "lpr.h"

// Setting number of buckets to 100
#define BUCKETS_SIZE 2626
size_t buckets = BUCKETS_SIZE;

void item_print(item_t *i)
{
    printf("key=%s value=%d", i->key, i->value);
}

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
        // printf("%s\n", buffer);
        char *plate = (char *)malloc(sizeof(char)*100);  
        strcpy(plate, buffer);
        htab_add(&h, plate, count);
        
        // printf("%s\n HASH: ", plate);
        // fgets(plate, 100 - 1, fp);
        
        count++;
    }

    //---------- PRINTING HASHTABLE
    // htab_print(&h);
    // item_print(htab_find(&h, "480GML"));

    fclose(fp);
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

// clean up hash table
// htab_destroy(&h);