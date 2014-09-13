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

void test_buffer(void)
{
  point p;
  array b = array_open(array_alloc(), 5, sizeof(point));

  assert(array_length(b) == 0);
  p = make_point(0, 1);
  array_push(b, &p);
  assert(array_length(b) == 1);
  p = make_point(2, 3);
  array_push(b, &p);
  assert(array_length(b) == 2);
  p = make_point(4, 5);
  array_push(b, &p);
  assert(array_length(b) == 3);

  array_get(b, 0, &p);
  assert(p.x == 0 && p.y == 1);
  array_get(b, 1, &p);
  assert(p.x == 2 && p.y == 3);
  array_get(b, 2, &p);
  assert(p.x == 4 && p.y == 5);

  array_del(b, 0, NULL);
  p = make_point(6, 7);
  array_ins(b, 1, &p);

  array_get(b, 2, &p);
  assert(p.x == 4 && p.y == 5);
  array_get(b, 1, &p);
  assert(p.x == 6 && p.y == 7);
  array_get(b, 0, &p);
  assert(p.x == 2 && p.y == 3);

  array_top(b, &p);
  assert(p.x == 4 && p.y == 5);
  array_pop(b, NULL);
  assert(array_length(b) == 2);
  array_pop(b, &p);
  assert(p.x == 6 && p.y == 7);
  assert(array_length(b) == 1);
  array_pop(b, &p);
  assert(p.x == 2 && p.y == 3);
  assert(array_length(b) == 0);

  b = array_free(array_close(b));
  assert(!b);
}

int main(int n, char **args) {
  test_buffer();
}
