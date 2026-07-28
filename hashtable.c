#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hashtable.h"

#define PRIME_NUMBER1 143
#define PRIME_NUMBER2 151

static ht_item HT_DELETED_ITEM = {NULL, NULL};

// Create new item
static ht_item* create_new_item(const char *key, const char *value)
{
    if(!key || !value) return NULL;

    ht_item *new_item = malloc(sizeof(ht_item));
    if(!new_item) return NULL;

    new_item->key = strdup(key);
    new_item->value = strdup(value);

    return new_item;
}

// Delete existing item
static void ht_del_item(ht_item *item)
{
    if(!item) return;
    free(item->key);
    free(item->value);
    free(item);
}

// Create Hash Table
static ht_table* create_hash_table(int number_of_elements)
{
    if(!number_of_elements) return NULL;

    ht_table *new_table = malloc(sizeof(ht_table));
    if(!new_table) return NULL;

    new_table->count = 0;
    new_table->size = number_of_elements;
    new_table->items = calloc((size_t)number_of_elements, sizeof(ht_item*));

    return new_table;
}

// Delete Table
static void del_hash_table(ht_table *table)
{
    if(!table) return;

    for(int i = 0; i < table->size; i++)
    {
        ht_item *item = table->items[i];
        if(item != NULL && item != &HT_DELETED_ITEM)
        {
            ht_del_item(item);
        }
    }

    free(table->items);
    free(table);
}

// Hashing
static unsigned int ht_hash(const char *s, const int prime_number_larger_than_size, int number_of_buckets)
{
    if(!s) return -1;
    const int len_s = strlen(s);
    long hash = 0;

    for(int i = 0; i < len_s; i++)
    {
        hash += (long)pow(prime_number_larger_than_size, len_s - (i+1)) * s[i];
    }

    hash = hash % number_of_buckets;

    return hash;
}

// Double Hashing Open Ended
static unsigned int ht_get_hash(const char *s, int number_of_buckets, const int attempt)
{
    if(!s || !number_of_buckets) return -1;
    const unsigned int hash_a = ht_hash(s, PRIME_NUMBER1, number_of_buckets);
    const unsigned int hash_b = ht_hash(s, PRIME_NUMBER2, number_of_buckets);

    printf("Hash a = %u Hash b = %u\n", hash_a, hash_b);

    int return_value =  ((hash_a + (attempt * (hash_b + 1))) % number_of_buckets);

    //if(return_value < 0) return_value += number_of_buckets;
    return return_value;
}

// Insert key,value
void ht_insert(ht_table *table, const char *key, const char *value)
{
    if(!table || !key || !value) return;

    ht_item *new_kv = create_new_item(key, value);
    if(!new_kv) return;

    unsigned int index = ht_get_hash(new_kv->key, table->size, 0);
    printf("Index = %d\n", index);
    ht_item *current = table->items[index];

    int i = 1;
    while(current != NULL && current != &HT_DELETED_ITEM)
    {
        index = ht_get_hash(new_kv->key, table->size, i);
        current = table->items[index];
        i++;
    }

    table->items[index] = new_kv;
    table->count++;
}

static char *ht_search(ht_table *table, const char *key)
{
    if(!table || !key) return NULL;

    int index = ht_get_hash(key, table->size ,0);

    //printf("Index = %d for search key = %s\n", index, key);

    ht_item *kv = table->items[index];
    int i = 1;
    while (kv != NULL)
    {
        if(kv != &HT_DELETED_ITEM)
        {
            if(strcmp(kv->key, key) == 0) 
            {
                return kv->value;
            }
        }

        index = ht_get_hash(key,table->size, i);
        kv = table->items[index];
        i++;
    }

    return NULL;
}

void ht_delete(ht_table *table, const char *key)
{
    if(!table || !key) return;

    int index = ht_get_hash(key, table->size, 0);
    ht_item *kv = table->items[index];
    int i = 1;

    while(kv != NULL)
    {
        if(kv != &HT_DELETED_ITEM)
        {
            if(strcmp(kv->key, key) == 0)
            {
                ht_del_item(kv);
                table->items[index] = &HT_DELETED_ITEM;
            }
        }

        index = ht_get_hash(key, table->size, i);
        kv = table->items[index];
        i++;
    }

    table->count--;

}

void print_table(ht_table *table)
{
    if(!table) 
    {
        printf("Hashtable empty\n");
        return;
    }

    printf("<-----------------Hashtable----------------------->\n");
    for(int i = 0; i < table->size; i++)
    {
        if(table->items[i] && table->items[i] != &HT_DELETED_ITEM)
            printf("Index = %d Key = %s , Value = %s\n", i, table->items[i]->key, table->items[i]->value);
    }
    printf("<---------------------------------------->\n");

}

int main()
{

    ht_table *table = create_hash_table(10);

    ht_item items[10] = {
        {"ID_01", "Apple"},
        {"ID_02", "Banana"},
        {"ID_03", "Cherry"},
        {"ID_04", "Date"},
        {"ID_05", "Elderberry"},
        {"ID_06", "Fig"},
        {"ID_07", "Grape"},
        {"ID_08", "Honeydew"},
        {"ID_09", "Kiwi"},
        {"ID_10", "Lemon"}
    };

    for(int i = 0; i < 10; i++)
    {
        ht_insert(table, items[i].key, items[i].value);
    }

    //printf("Value for key %s\n",ht_search(table, "Keerthana"));

    print_table(table);
    del_hash_table(table);
    return 0;
}