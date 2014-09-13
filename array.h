#include <stdlib.h>

typedef struct array {
  size_t capacity; /* maximum number of members */
  size_t mem_size; /* size of member in bytes */
  size_t gap;      /* position of gap */
  size_t length;   /* length of gap is capacity - length */
  char  *data;     /* pointer to member data */
} *array;

array array_alloc(void);
array array_open(array this, size_t capacity, size_t mem_size);
array array_close(array this);
array array_free(array this);

void array_get(array this, size_t index, void *member);
void array_put(array this, size_t index, void *member);
void array_del(array this, size_t index, void *member);
void array_ins(array this, size_t index, void *member);

void array_push(array this, void *member);
void array_pop(array this, void *member);
void array_top(array this, void *member);

size_t array_length(array this);
size_t array_capacity(array this);

