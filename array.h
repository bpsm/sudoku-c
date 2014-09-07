#include <stdlib.h>

typedef struct {
  size_t capacity;
  size_t mem_size;
  char *data;
} array;

array * array_alloc(void);
void array_open(array *this, size_t capacity, size_t mem_size);
void array_close(array *this);
void array_free(array *this);

void array_get(array *this, size_t index, void *member);
void array_put(array *this, size_t index, void *member);
void array_move(array *this, size_t dst_index, size_t src_index, size_t count);

size_t array_length(array *this);

typedef struct {
  array array;
  size_t gap;    /* position of gap */
  size_t length; /* length of gap is capacity - length */
} buffer;

buffer * buffer_alloc(void);
void buffer_open(buffer *this, size_t capacity, size_t mem_size);
void buffer_close(buffer *this);
void buffer_free(buffer *this);

void buffer_get(buffer* this, size_t index, void *member);
void buffer_put(buffer* this, size_t index, void *member);
void buffer_del(buffer* this, size_t index);
void buffer_ins(buffer* this, size_t index, void *member);

void buffer_push(buffer* this, void *member);
void buffer_pop(buffer* this, void *member);
void buffer_top(buffer* this, void *member);

size_t buffer_length(buffer* this);

