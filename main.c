#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"

int main() {
//   mem_init();
//   mem_alloc(200);
//   assert(mem_alloc(200) == NULL);

//   mem_init();
//   mem_alloc(42);
//   mem_alloc(42);
//   mem_alloc(42);
//   mem_alloc(42);
//   mem_alloc(42);
//   assert(mem_alloc(42) != NULL);

//   mem_init();
//   mem_alloc(42);
//   mem_alloc(42);
//   mem_alloc(42);
//   mem_alloc(42);
//   mem_alloc(42);
//   mem_alloc(42);
//   assert(mem_alloc(42) == NULL);

//   mem_init();
//   mem_show_heap();
//   mem_alloc(42);
//   mem_alloc(42);
//   int64_t *p = mem_alloc(42);
//   mem_show_heap();
//   mem_release(p);
//   mem_show_heap();

//   mem_init();
//   p = mem_alloc(200);
//   mem_release(p);
//   assert(mem_alloc(300) != NULL);

  mem_init();
  mem_show_heap();
  mem_alloc(42);
  mem_show_heap();
  mem_alloc(42);
  mem_alloc(42);
  mem_alloc(42);
  mem_alloc(42);
  mem_alloc(42);
  mem_show_heap();
  mem_alloc(42);
  mem_show_heap();
}
