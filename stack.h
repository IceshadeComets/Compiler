#pragma once

#include <stddef.h>

// `Stack` implements a stack data structure for `void*` types which
// automatically resizes itself (and `err`s if memory allocation fails)
struct Stack {
  // `buffer` is a pointer to your `void*` elements that were pushed
  // on the stack.
  void **buffer;
  // `length` is the number of elements currently stored
  // in the stack
  size_t length;
  // `capacity` is size of the buffer `buffer` for the stack
  size_t capacity;
};

// `stackInit` initializes an uninitialized stack containing no elements with a
// buffer of the given `capacity`.
//
// Example usage
// ```
// #define DEFAULT_STACK_SIZE 16
// struct Stack myStack;
// stackInit(&myStack, DEFAULT_STACK_SIZE);
// ```
void stackInit(struct Stack *stack, size_t capacity);

// `stackPush` pushes `elem` to the top of `stack`.
void stackPush(struct Stack *stack, void *elem);

// `stackPop` returns and removes the top element from the stack.
//
// Preconditions:
//
//  - `stack` must have length > 0.
void *stackPop(struct Stack *stack);

// `stackTop` returns the top element from the stack.
void *stackTop(struct Stack *stack);

void *stackRead(struct Stack *stack, int indexFromTop);

// `stackPrint` prints the pointer addresses of all elements in the stack and
// other related information to stderr
void stackPrint(struct Stack *stack);
