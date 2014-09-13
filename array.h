#include <stdlib.h>

typedef struct {
  size_t capacity;
  size_t mem_size;
  char *data;
} array;

typedef struct {
  array array;
  size_t gap;    /* position of gap */
  size_t length; /* length of gap is capacity - length */
} *buffer;


buffer buffer_alloc(void);
buffer buffer_open(buffer this, size_t capacity, size_t mem_size);
buffer buffer_close(buffer this);
buffer buffer_free(buffer this);

void buffer_get(buffer this, size_t index, void *member);
void buffer_put(buffer this, size_t index, void *member);
void buffer_del(buffer this, size_t index, void *member);
void buffer_ins(buffer this, size_t index, void *member);

void buffer_push(buffer this, void *member);
void buffer_pop(buffer this, void *member);
void buffer_top(buffer this, void *member);

size_t buffer_length(buffer this);
size_t buffer_capacity(buffer this);


/*
TODO
- array "apis" should be private
 */
