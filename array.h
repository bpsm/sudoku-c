#include <stdlib.h>

typedef struct {
  size_t capacity; /* maximum number of members */
  size_t mem_size; /* size of member in bytes */
  size_t gap;      /* position of gap */
  size_t length;   /* length of gap is capacity - length */
  char  *data;     /* pointer to member data */
} buffer, *buffer_ref;

buffer_ref buffer_alloc(void);
buffer_ref buffer_open(buffer_ref this, size_t capacity, size_t mem_size);
buffer_ref buffer_close(buffer_ref this);
buffer_ref buffer_free(buffer_ref this);

void buffer_get(buffer_ref this, size_t index, void *member);
void buffer_put(buffer_ref this, size_t index, void *member);
void buffer_del(buffer_ref this, size_t index, void *member);
void buffer_ins(buffer_ref this, size_t index, void *member);

void buffer_push(buffer_ref this, void *member);
void buffer_pop(buffer_ref this, void *member);
void buffer_top(buffer_ref this, void *member);

size_t buffer_length(buffer_ref this);
size_t buffer_capacity(buffer_ref this);


/*
TODO
- array "apis" should be private
 */
