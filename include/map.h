#ifndef MAP_H
#define MAP_H

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define HASHMAP_SUCCESS 1
#define HASHMAP_FAIL 0

#define TOMBSTONE ((char*)-1)

typedef struct {
    char* key;
    void* value;
} HashMapEntry;

typedef struct {
    HashMapEntry* entries;
    uint32_t element_size;
    uint32_t size;
    uint32_t capacity;
} HashMap;

extern int init_hashmap(HashMap* map, const uint32_t element_size);
extern int push_hashmap(HashMap* map, const char* key, const void* data);
extern int remove_hashmap(HashMap* map, const char* key);

extern void* get_hashmap(HashMap* map, const char* key);
extern void grow_hashmap(HashMap* map);

// hashing function
static inline uint32_t hash(const char *key) {
    uint32_t h = 2166136261u;
    for (const unsigned char *p = (const unsigned char*)key; *p; p++) {
        h ^= *p;
        h *= 16777619u;
    }
    return h;
}

static inline void free_hashmap(HashMap* map) {
    for (uint32_t i = 0; i < map->capacity; i++) {
        HashMapEntry* entry = &map->entries[i];
        if (entry->key != NULL) {
            free(entry->key);
        }
        if (entry->value != NULL) {
            free(entry->value);
        }
    }

    free(map->entries);
}

#endif 