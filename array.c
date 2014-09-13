#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "array.h"


/* 
 * Array
 * =====
 *
 * Randomly addresable storage for a fixed number of
 * identically sized values.
 */

static array *array_alloc(void)
{
  return calloc(1, sizeof(array));
}

static array *array_open(array *this, size_t capacity, size_t mem_size)
{
  assert(this); assert(this->data == NULL);
  this->capacity = capacity;
  this->mem_size = mem_size;
  this->data = calloc(capacity, mem_size);
  return this;
}

static array *array_close(array *this)
{
  assert(this); assert(this->data);
  free(this->data);
  return this;
}

static array *array_free(array *this)
{
  free(this);
  return NULL;
}

static void array_get(array *this, size_t index, void *member)
{
  assert(this); assert(index < this->capacity);
  memmove(member, this->data + this->mem_size * index, this->mem_size);
}

static void array_put(array *this, size_t index, void *member)
{
  assert(this); assert(index < this->capacity);
  memmove(this->data + this->mem_size * index, member, this->mem_size);
}

static void array_move(array *this, size_t dst_index, size_t src_index, size_t count)
{
  assert(this);
  assert(dst_index < this->capacity);
  assert((dst_index + count) <= this->capacity);
  assert(src_index < this->capacity);
  assert((src_index + count) <= this->capacity);
  memmove(this->data + dst_index * this->mem_size,
          this->data + src_index * this->mem_size,
          count * this->mem_size);
}

static size_t array_length(array *this)
{
  return this->capacity;
}

/*
 * Buffer
 * ======
 *
 * Randomly addressable storage for a fixed maximum number of
 * identically sized values with amortized efficient insertion and
 * deletion.
 */

buffer buffer_alloc(void)
{
  return calloc(1, sizeof(buffer));
}

buffer buffer_open(buffer this, size_t capacity, size_t mem_size)
{
  assert(this);
  array_open(&this->array, capacity, mem_size);
  this->gap = 0;
  this->length = 0;
  return this;
}

buffer buffer_close(buffer this)
{
  assert(this);
  array_close(&this->array);
  return this;
}

buffer buffer_free(buffer this)
{
  assert(this);
  free(this);
  return NULL;
}

#define BUFLEN(this) (this->length)
#define BUFCAP(this) (this->array.capacity)
#define GAPLEN(this) (BUFCAP(this) - BUFLEN(this))

void buffer_get(buffer this, size_t index, void *member)
{
  assert(this);
  assert(BUFLEN(this) > index);
  array_get(&this->array,
            index + ((index < this->gap) ? 0 : GAPLEN(this)),
            member);
}

static void buffer_move_gap(buffer this, size_t gap)
{
  int n;
  assert(this); 
  assert(gap <= BUFLEN(this));
  if (this->gap < gap) {
    /* move gap to right (away from zero) */
    n = gap - this->gap;
    array_move(&this->array, 
               this->gap,
               this->gap + GAPLEN(this),
               n);
    this->gap = gap;
  } else if (gap < this->gap) {
    /* move gap to left (towards zero) */
    n = this->gap - gap;
    array_move(&this->array,
               this->gap + GAPLEN(this) - n,
               this->gap - n,
               n);
    this->gap = gap;
  }
}

void buffer_put(buffer this, size_t index, void *member)
{
  assert(this);
  assert(index <= BUFLEN(this));
  if (index < BUFLEN(this)) {
    /* Overwrite an existing element */
    array_put(&this->array, 
              index + ((index < this->gap) ? 0 : GAPLEN(this)),
              member);
  } else {
    /* Append an element to the end, which may require moving the gap
       (to the end). */
    assert(index == BUFLEN(this));
    assert(BUFLEN(this) < BUFCAP(this));
    if (this->gap != index)
      buffer_move_gap(this, index);
    array_put(&this->array, index, member);
    this->length++;
    this->gap++;
  }
}

void buffer_del(buffer this, size_t index, void *member)
{
  assert(this);
  assert(index < BUFLEN(this));
  if (member)
    buffer_get(this, index, member);
  if (this->gap != index)
    buffer_move_gap(this, index);
  this->length--;
}

void buffer_ins(buffer this, size_t index, void *member)
{
  assert(this);
  assert(index <= BUFLEN(this));
  assert(BUFLEN(this) < BUFCAP(this));
  if (this->gap != index)
    buffer_move_gap(this, index);
  array_put(&this->array, index, member);
  this->length++;
  this->gap++;
}

void buffer_push(buffer this, void *member)
{
  buffer_ins(this, BUFLEN(this), member);
}

void buffer_pop(buffer this, void *member)
{
  buffer_del(this, BUFLEN(this) - 1, member);
}

void buffer_top(buffer this, void *member)
{
  buffer_get(this, BUFLEN(this) - 1, member);
}

size_t buffer_length(buffer this)
{
  return BUFLEN(this);
}

size_t buffer_capacity(buffer this)
{
  return BUFCAP(this);
}

