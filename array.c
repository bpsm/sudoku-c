#include <stdio.h>

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

#define REF(array, index) ((void*) &(array)->data[(index) * (array)->mem_size])

void *array_ref(const array this, size_t index)
{
  assert(index < this->length);
  return REF(this, index);
}

void  array_get(const array this, size_t index, void *mem)
{
  assert(index < this->length);
  memmove(mem, REF(this, index), this->mem_size);
}

void  array_put(array this, size_t index, void const *mem)
{
  assert(index <= this->length && index < this->capacity);
  if (this->del_mem && index < this->length)
    this->del_mem(REF(this, index));
  memmove(REF(this, index), mem, this->mem_size);
  if (index == this->length)
    this->length++;
}

void  array_ins(array this, size_t index, void const *mem)
{
  assert(this->length < this->capacity);
  assert(index <= this->length);
  assert(index < this->capacity);
  if (index != this->length)
    memmove(REF(this, index),
            REF(this, index+1),
            (this->length - index) * this->mem_size);
  memmove(REF(this, index), mem, this->mem_size);
  this->length++;
}

void  array_del(array this, size_t index)
{
  void *ref = array_ref(this, index);
  if (this->del_mem)
    this->del_mem(ref);
  size_t size = (this->length - index - 1);
  if (size)
    memmove(ref, ref + this->mem_size, size);
  this->length--;
}

void  array_push(array this, void const *mem)
{
  assert(this->length < this->capacity);
  memmove(REF(this, this->length), mem, this->mem_size);
  this->length++;
}

void  array_pop(array this, void *mem)
{
  assert(this->length > 0);
  this->length--;
  if (mem)
    memmove(mem, REF(this, this->length), this->mem_size);
  else if (this->del_mem)
    this->del_mem(REF(this, this->length));
}

void  *array_top(array this)
{
  assert(this->length > 0);
  return REF(this, this->length-1);
}

/*
int main(int n, char **args) {
  int tmp = 1;
  array a = array_new(3, sizeof(int), NULL);
  array_push(a, &tmp);
  tmp = 2;
  array_push(a, &tmp);
  tmp = 3;
  array_push(a, &tmp);
  printf("%d\n", (int)a->length);
  for (int i = 0; i < 3; i++) {
    printf("%d\n", *((int*)array_top(a)));
    array_pop(a, NULL);
  }
  printf("%d\n", (int)a->length);
}  
*/
