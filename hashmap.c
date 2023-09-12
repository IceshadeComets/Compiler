#include "hashmap.h"

#include <assert.h>
#include <err.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Hashmap Function Provided by Tutorial Section from Jared Pon, which allows Hashmap Structures to be created in C.

static size_t hash(char *str) {
  size_t acc = 0;
  int c;

  while ((c = *str++)) {
    acc = c + 420 * acc;
  }

  return acc;
}

static size_t probe(struct HashMap *hashmap, char *key) {
  size_t i = hash(key) % hashmap->capacity;

  while (hashmap->buffer[i].key != NULL &&
         strcmp(hashmap->buffer[i].key, key) != 0)
    i = (i + 1) % hashmap->capacity;

  return i;
}

void hashMapInit(struct HashMap *hashmap, size_t capacity) {
  assert(capacity > 0);

  hashmap->length = 0;
  hashmap->capacity = capacity;
  hashmap->buffer = malloc(sizeof(struct HashMapElem) * capacity);
  for (size_t i = 0; i < capacity; ++i) {
    hashmap->buffer[i].key = NULL;
    hashmap->buffer[i].value = NULL;
  }

  if (hashmap->buffer == NULL)
    err(EXIT_FAILURE, "error `hashMapInit` memory allocation failed");

  return;
}

void *hashMapFind(struct HashMap *hashmap, char *key) {
  return hashmap->buffer[probe(hashmap, key)].value;
}

void hashMapInsert(struct HashMap *hashmap, char *key, void *value) {
  assert(key != NULL);
  assert(value != NULL);

  if (2 * (hashmap->length + 1) >= hashmap->capacity) {
    struct HashMap newHashMap;
    hashMapInit(&newHashMap, 2 * (hashmap->capacity + 1));

    // reinsert everything in the new hashmap
    for (size_t i = 0; i < hashmap->capacity; ++i) {
      if (hashmap->buffer[i].value == NULL)
        continue;

      size_t slot = probe(&newHashMap, hashmap->buffer[i].key);
      newHashMap.buffer[slot] = hashmap->buffer[i];
      ++newHashMap.length;
    }

    free(hashmap->buffer);
    memcpy(hashmap, &newHashMap, sizeof newHashMap);
  }

  size_t slot = probe(hashmap, key);
  if (hashmap->buffer[slot].key == NULL)
    ++hashmap->length;
  hashmap->buffer[slot].key = key;
  hashmap->buffer[slot].value = value;
}

void hashMapPrint(struct HashMap *hashmap) {
  fprintf(stderr, "Hashmap:\n");
  fprintf(stderr, "\tLength: %zu\n", hashmap->length);
  fprintf(stderr, "\tCapacity: %zu\n", hashmap->capacity);
  fprintf(stderr, "\tBuffer:\n");
  for (size_t i = 0; i < hashmap->capacity; ++i) {
    fprintf(stderr, "\t\t%zu:\n", i);
    fprintf(stderr, "\t\t\tKey: %s\n",
            hashmap->buffer[i].key ? hashmap->buffer[i].key : "NULL");
    fprintf(stderr, "\t\t\tValue:%p\n", hashmap->buffer[i].value);
  }
}
