#pragma once

#include <stddef.h>

// Hashmap Function Provided by Tutorial Section from Jared Pon, and Modified a bit to work for this assignment but most of the code here imitates a hashmap for C.

struct HashMap;
struct HashMapElem;

// `HashMap` implements a hashmap that provides an efficient mapping from
// `char*` NULL terminated strings (that are not NULL pointer) to non NULL
// `void*` pointers.
//
// We support
//
//      - initialization
//
//      - insertion
//
//      - find
//
// We use an open addressing scheme with linear probing.
//
// Note: this automatically resizes itself (and `err`s if memory allocation
// fails)
struct HashMap {
  // `buffer` is an array of `HashMapElem` which associate keys to your `void*`
  // pointers you put in the hashmap.
  struct HashMapElem *buffer;
  // `length` is the number of elements currently stored
  // in the hashmap
  size_t length;
  // `capacity` is size of the buffer `buffer` for the hashmap
  size_t capacity;
};

// Invariants:
//  - capacity > length

// `HashMapElem` is a struct to contain the key (a `char*`) and value (a
// `void*`) of an element stored in the hashmap.
struct HashMapElem {
  char *key;
  void *value;
};

// `hashMapInit` initializes an uninitialized hashmap containing no elements
// with a buffer of the given `capacity`.
//
// Preconditions:
//  - `capacity` > 0
//
// Example usage
// ```
// #define DEFAULT_HASH_MAP_SIZE 16
// struct HashMap myHashMap;
// hashMapInit(&myHashMap, DEFAULT_HASH_MAP_SIZE);
// ```
void hashMapInit(struct HashMap *hashmap, size_t capacity);

// `hashMapFind` searches the hashmap for an element corresponding to the given
// `key`.
//
//  - If there is an element with the given `key`, then this returns that
//  element
//
//  - Otherwise, this returns NULL.
void *hashMapFind(struct HashMap *hashmap, char *key);

// `hashMapInsert` inserts the given `key` and `value` pair in the hashmap.
//
//  - if a mapping from `key` already exists, then we overwrite the old value
//  with the new `value`.
//
//  - otherwise, we create a new mapping from `key` to `value`.
//
// Preconditions:
//  - both `key` and `value` are not NULL.
//
// Notes: if this reallocates, this frees the old buffer maintained by the
// hashmap
void hashMapInsert(struct HashMap *hashmap, char *key, void *value);

// `hashMapPrint` prints the pointer addresses of all elements in the hashmap
// and other related information to stderr
void hashMapPrint(struct HashMap *hashmap);
