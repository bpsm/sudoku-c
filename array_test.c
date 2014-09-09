#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "array.h"

typedef struct {
  int x, y;
} point;


point make_point(int x, int y)
{
  point p; p.x = x; p.y = y; return p;
}

void test_array(void)
{
  array *a = array_open(array_alloc(), 3, sizeof(point));

  assert(3 == array_length(a));

  point p;
  p = make_point(10, 11);
  array_put(a, 1, &p);

  p = make_point(0, 1);
  array_put(a, 0, &p);

  p = make_point(20, 21);
  array_put(a, 2, &p);

  array_get(a, 0, &p);
  assert(p.x == 0 && p.y == 1);

  array_get(a, 1, &p);
  assert(p.x == 10 && p.y == 11);

  array_get(a, 2, &p);
  assert(p.x == 20 && p.y == 21);

  array_move(a, 1, 0, 2);

  array_get(a, 0, &p);
  assert(p.x == 0 && p.y == 1);

  array_get(a, 1, &p);
  assert(p.x == 0 && p.y == 1);

  array_get(a, 2, &p);
  assert(p.x == 10 && p.y == 11);

  a = array_free(array_close(a));
  assert(!a);
}  

void test_buffer(void)
{
  point p;
  buffer *b = buffer_open(buffer_alloc(), 5, sizeof(point));

  assert(buffer_length(b) == 0);
  p = make_point(0, 1);
  buffer_push(b, &p);
  assert(buffer_length(b) == 1);
  p = make_point(2, 3);
  buffer_push(b, &p);
  assert(buffer_length(b) == 2);
  p = make_point(4, 5);
  buffer_push(b, &p);
  assert(buffer_length(b) == 3);

  buffer_get(b, 0, &p);
  assert(p.x == 0 && p.y == 1);
  buffer_get(b, 1, &p);
  assert(p.x == 2 && p.y == 3);
  buffer_get(b, 2, &p);
  assert(p.x == 4 && p.y == 5);

  buffer_del(b, 0, NULL);
  p = make_point(6, 7);
  buffer_ins(b, 1, &p);

  buffer_get(b, 2, &p);
  assert(p.x == 4 && p.y == 5);
  buffer_get(b, 1, &p);
  assert(p.x == 6 && p.y == 7);
  buffer_get(b, 0, &p);
  assert(p.x == 2 && p.y == 3);

  buffer_top(b, &p);
  assert(p.x == 4 && p.y == 5);
  buffer_pop(b, NULL);
  assert(buffer_length(b) == 2);
  buffer_pop(b, &p);
  assert(p.x == 6 && p.y == 7);
  assert(buffer_length(b) == 1);
  buffer_pop(b, &p);
  assert(p.x == 2 && p.y == 3);
  assert(buffer_length(b) == 0);

  b = buffer_free(buffer_close(b));
  assert(!b);
}

int main(int n, char **args) {
  test_array();
  test_buffer();
}
