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

void test_array_get_and_put(void)
{
  array *a = array_alloc();
  array_open(a, 3, sizeof(point));

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

  array_close(a);
  array_free(a);
}  


int main(int n, char **args) {
  test_array_get_and_put();
}
