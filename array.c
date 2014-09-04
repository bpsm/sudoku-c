#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "array.h"

array array_new(size_t capacity, size_t mem_size, del_fn del_mem)
{
  array this = malloc(sizeof(struct array_desc) + mem_size * capacity);
  this->capacity = capacity;
  this->mem_size = mem_size;
  this->del_mem = del_mem;
  this->length = 0;
  return (array) this;
}

void array_free(array this)
{
  size_t i; void *p;
  if (this->del_mem)
    for (i = 0, p = &this->data[0]; i < this->length; i++, p += this->mem_size)
      this->del_mem(p);
  free(this);
}

void *array_ref(const array this, size_t index)
{
  assert(index < this->length);
  return &this->data[index * this->mem_size];
}

void  array_get(const array this, size_t index, void *mem)
{
  memmove(mem, array_ref(this, index), this->mem_size);
}

void  array_put(array this, size_t index, void const *mem)
{
  memmove(array_ref(this, index), mem, this->mem_size);
}

void  array_ins(array this, size_t index, void const *mem)
{
  
}

void  array_del(array this, size_t index)
{

}

void  array_push(array this, void const *mem)
{

}

void  array_pop(array this)
{

}

void  *array_top(array this)
{
  return NULL;
}

  
