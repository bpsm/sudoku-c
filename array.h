#include <stdlib.h>

typedef void (*del_fn)(void *mem);

typedef struct array_desc {
  size_t capacity;
  size_t mem_size;
  del_fn del_mem;
  size_t length;
  char data[];
} *array;

array array_new(size_t capacity, size_t mem_size, del_fn del_mem);
void  array_free(array this);

void *array_ref(const array this, size_t index);
void  array_get(const array this, size_t index, void *mem);
void  array_put(array this, size_t index, void const *mem);
void  array_ins(array this, size_t index, void const *mem);
void  array_del(array this, size_t index);

void  array_push(array this, void const *mem);
void  array_pop(array this);
void  *array_top(array this);


