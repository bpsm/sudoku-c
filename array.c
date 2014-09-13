#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "array.h"

static void array__get(array this, size_t index, void *member)
{
  assert(this); assert(index < this->capacity);
  memmove(member, this->data + this->mem_size * index, this->mem_size);
}

static void array__put(array this, size_t index, void *member)
{
  assert(this); assert(index < this->capacity);
  memmove(this->data + this->mem_size * index, member, this->mem_size);
}

static void array__move(array this, size_t dst_index, size_t src_index, size_t count)
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

/*
 * Buffer
 * ======
 *
 * Randomly addressable storage for a fixed maximum number of
 * identically sized values with amortized efficient insertion and
 * deletion.
 */

array array_alloc(void)
{
  return calloc(1, sizeof(struct array));
}

array array_open(array this, size_t capacity, size_t mem_size)
{
  assert(this);
  this->capacity = capacity;
  this->mem_size = mem_size;
  this->gap = 0;
  this->length = 0;
  this->data = calloc(capacity, mem_size);
  return this;
}

array array_close(array this)
{
  assert(this);
  assert(this->data);
  free(this->data);
  this->data = NULL;
  return this;
}

array array_free(array this)
{
  assert(this);
  free(this);
  return NULL;
}

#define BUFLEN(this) (this->length)
#define BUFCAP(this) (this->capacity)
#define GAPLEN(this) (BUFCAP(this) - BUFLEN(this))

void array_get(array this, size_t index, void *member)
{
  assert(this);
  assert(BUFLEN(this) > index);
  array__get(this,
            index + ((index < this->gap) ? 0 : GAPLEN(this)),
            member);
}

static void array_move_gap(array this, size_t gap)
{
  int n;
  assert(this); 
  assert(gap <= BUFLEN(this));
  if (this->gap < gap) {
    /* move gap to right (away from zero) */
    n = gap - this->gap;
    array__move(this,
               this->gap,
               this->gap + GAPLEN(this),
               n);
    this->gap = gap;
  } else if (gap < this->gap) {
    /* move gap to left (towards zero) */
    n = this->gap - gap;
    array__move(this,
               this->gap + GAPLEN(this) - n,
               this->gap - n,
               n);
    this->gap = gap;
  }
}

void array_put(array this, size_t index, void *member)
{
  assert(this);
  assert(index <= BUFLEN(this));
  if (index < BUFLEN(this)) {
    /* Overwrite an existing element */
    array__put(this,
              index + ((index < this->gap) ? 0 : GAPLEN(this)),
              member);
  } else {
    /* Append an element to the end, which may require moving the gap
       (to the end). */
    assert(index == BUFLEN(this));
    assert(BUFLEN(this) < BUFCAP(this));
    if (this->gap != index)
      array_move_gap(this, index);
    array__put(this, index, member);
    this->length++;
    this->gap++;
  }
}

void array_del(array this, size_t index, void *member)
{
  assert(this);
  assert(index < BUFLEN(this));
  if (member)
    array_get(this, index, member);
  if (this->gap != index)
    array_move_gap(this, index);
  this->length--;
}

void array_ins(array this, size_t index, void *member)
{
  assert(this);
  assert(index <= BUFLEN(this));
  assert(BUFLEN(this) < BUFCAP(this));
  if (this->gap != index)
    array_move_gap(this, index);
  array__put(this, index, member);
  this->length++;
  this->gap++;
}

void array_push(array this, void *member)
{
  array_ins(this, BUFLEN(this), member);
}

void array_pop(array this, void *member)
{
  array_del(this, BUFLEN(this) - 1, member);
}

void array_top(array this, void *member)
{
  array_get(this, BUFLEN(this) - 1, member);
}

size_t array_length(array this)
{
  return BUFLEN(this);
}

size_t array_capacity(array this)
{
  return BUFCAP(this);
}

