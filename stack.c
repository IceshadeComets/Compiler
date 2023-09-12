#include "stack.h"

#include <assert.h>
#include <err.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

// Hashmap Function Provided by Tutorial Section from Jared Pon, which allows Stack Structures to be created in C Code.

void stackInit(struct Stack *stack, size_t capacity) {
  stack->length = 0;
  stack->capacity = capacity;
  stack->buffer = malloc(sizeof(void *) * capacity);

  if (stack->buffer == NULL)
    err(EXIT_FAILURE, "error in `stackInit` memory allocation failed.");
}

void stackPush(struct Stack *stack, void *elem) {
  if (stack->length == stack->capacity) {
    size_t newCapacity = 2 * stack->capacity + 1;

    stack->buffer = realloc(stack->buffer, sizeof(void *) * newCapacity);
    stack->capacity = newCapacity;
    if (stack->buffer == NULL)
      err(EXIT_FAILURE, "error in `stackPush` memory reallocation failed.");
  }

  stack->buffer[(stack->length)++] = elem;
}

void *stackPop(struct Stack *stack) {
  assert(stack->length > 0);

  return stack->buffer[--stack->length];
}

void *stackTop(struct Stack *stack) { return stack->buffer[stack->length - 1]; }

void *stackRead(struct Stack *stack, int indexFromTop) { return stack->buffer[stack->length - 1 - indexFromTop]; }

void stackPrint(struct Stack *stack) {
  fprintf(stderr, "Stack:\n");
  fprintf(stderr, "\tLength: %zu\n", stack->length);
  fprintf(stderr, "\tCapacity: %zu\n", stack->capacity);
  fprintf(stderr, "\tElements (highest element is the top of the stack):\n");
  // iterate through the stack backwards to the top appears at the top.
  for (size_t i = stack->length; i--;)
    fprintf(stderr, "\t\t%zu: %p\n", i, stack->buffer[i]);
}
