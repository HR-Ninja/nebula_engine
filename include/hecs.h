#pragma once

#include "assert.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#define HAS_COMPONENT(mask, id) (((mask) & (1ULL << (id))) != 0)

#define MAX_ENTITIES 20000
#define MAX_COMPONENTS 64

typedef uint32_t Entity;
typedef uint32_t Component;
typedef uint64_t Mask;

static Entity free_list[MAX_ENTITIES];
static size_t free_count = 0;
static Mask entity_mask[MAX_ENTITIES];

typedef struct {
    void* data;
    size_t size;
} ComponentArray;

static ComponentArray components[MAX_COMPONENTS];

Entity create_entity();
void destroy_entity(Entity e);

Component register_component(const size_t size);
void attach_component(const Entity e, const Component c, void* data);
void remove_component(const Entity e, const Component c);
void* get_component(const Entity e, const Component c);
void free_components();


Entity create_entity() {
    if (free_count > 0) {
        return free_list[--free_count];
    }

    static Entity count = 0;
    assert(count < MAX_ENTITIES);

    return count++;
}

void destroy_entity(Entity e) {
    assert(e < MAX_ENTITIES);

    // Remove all components by clearing mask
    Mask mask = entity_mask[e];
    for (Component c = 0; c < MAX_COMPONENTS; c++) {
        if (mask & (1ULL << c)) {
            remove_component(e, c);
        }
    }

    entity_mask[e] = 0;
    assert(free_count < MAX_ENTITIES);
    free_list[free_count++] = e;
}

Component register_component(const size_t size) {
    static Component id = 0;
    assert(id < MAX_COMPONENTS);  // Check BEFORE use

    components[id].data = NULL;
    components[id].size = size;
    return id++;
}

void attach_component(const Entity e, const Component c, void* data) {
    assert(e < MAX_ENTITIES && c < MAX_COMPONENTS);

    if (components[c].data == NULL) {
        components[c].data = calloc(MAX_ENTITIES, components[c].size);
    }

    assert(data != NULL);
    void* dest = (char*)components[c].data + e * components[c].size;
    memcpy(dest, data, components[c].size);
    entity_mask[e] |= 1ULL << c;
}

void remove_component(const Entity e, const Component c) {
    assert(e < MAX_ENTITIES && c < MAX_COMPONENTS);
    
    if (components[c].data) {
        void* dest = (char*)components[c].data + e * components[c].size;
        memset(dest, 0, components[c].size);
    }

    entity_mask[e] &= ~(1ULL << c);
}

void* get_component(const Entity e, const Component c) {
    assert(e < MAX_ENTITIES && c < MAX_COMPONENTS);
    assert(HAS_COMPONENT(entity_mask[e], c));
    assert(components[c].data != NULL);
    return (char*)components[c].data + e * components[c].size;
}

void free_components() {
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (components[i].data) {
            free(components[i].data);
            components[i].data = NULL;
        }
        components[i].size = 0;
    }
}
