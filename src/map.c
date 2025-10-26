#include "map.h"

int init_hashmap(HashMap* map, const uint32_t element_size) {
    map->size = 0;
    map->element_size = element_size;
    map->capacity = 128;
    map->entries = (HashMapEntry*)calloc(map->capacity, sizeof(HashMapEntry));
    return map->entries ? HASHMAP_SUCCESS : HASHMAP_FAIL;
}

int push_hashmap(HashMap* map, const char* key, const void* data) {

    if ((map->size + 1) * 100 / map->capacity > 70) {
        grow_hashmap(map);
    }

    uint32_t mask = map->capacity - 1;
    uint32_t idx = hash(key) & mask;

    for (int i = 0; i < map->capacity; i++) {
        
        HashMapEntry* e = &map->entries[idx];

        if (e->key == NULL || e->key == TOMBSTONE) {
            e->key = strdup(key);
            e->value = malloc(map->element_size);
            memcpy(e->value, data, map->element_size);
            map->size++;
            return HASHMAP_SUCCESS;
        }
        if (strcmp(e->key, key) == 0) {
            memcpy(e->value, data, map->element_size);
            return HASHMAP_SUCCESS;
        }
        
        idx = (idx + 1) & mask;
    }

    return HASHMAP_FAIL;
}

void* get_hashmap(HashMap* map, const char* key) {

    uint32_t mask = map->capacity - 1;
    uint32_t idx = hash(key) & mask;

    for (int i = 0; i < map->capacity; i++) {
        
        HashMapEntry* e = &map->entries[idx];

        if (e->key == NULL) {
            return NULL;
        }

        if (e->key != TOMBSTONE && strcmp(e->key, key) == 0) {
            return e->value;
        }

        idx = (idx + 1) & mask;
    }

    return NULL; 
}

int remove_hashmap(HashMap* map, const char* key) {

    uint32_t mask = map->capacity - 1;
    uint32_t idx = hash(key) & mask;

    for (int i = 0; i < map->capacity; i++) {
        
        HashMapEntry* e = &map->entries[idx];

        if (e->key == NULL) {
            return HASHMAP_FAIL;
        }

        if (strcmp(e->key, key) == 0) {
            free(e->key);
            free(e->value);
            e->key = TOMBSTONE;
            e->value = NULL;
            map->size--;
            return HASHMAP_SUCCESS;
        }

        idx = (idx + 1) & mask;
    }

    return HASHMAP_FAIL;
}


// grow and re-hash old entries
void grow_hashmap(HashMap *map) {
    uint32_t old_capacity = map->capacity;
    HashMapEntry *old_entries = map->entries;

    map->capacity *= 2;
    map->entries = calloc(map->capacity, sizeof(HashMapEntry));
    map->size = 0;

    for (uint32_t i = 0; i < old_capacity; i++) {
        HashMapEntry *entry = &old_entries[i];
        if (entry->key != NULL && entry->key != TOMBSTONE) {

            uint32_t mask = map->capacity - 1;
            uint32_t idx = hash(entry->key) & mask;
            while (map->entries[idx].key != NULL) {
                idx = (idx + 1) & mask;
            }
            map->entries[idx].key   = entry->key;
            map->entries[idx].value = entry->value;
            map->size++;
        }
    }

    free(old_entries);
}