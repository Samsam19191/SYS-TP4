#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "mem.h"

void *heap_base = NULL; // first address of the heap
void *heap_end = NULL;  // first address beyond the heap

int mem_initialized = 0;

// initialize the memory manager
void mem_init(void) {
  // request memory from the kernel
  heap_base = mmap(NULL, 800, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert(heap_base != MAP_FAILED);

  heap_end = heap_base + 800;

  // create some free blocks: five of 80 bytes and one of 400 bytes
  *((int64_t *)heap_base) = 800;

  *((int64_t *)(heap_end - 1)) = 800;

  mem_initialized = 1;
}

void *mem_alloc(int64_t length) {
  assert(mem_initialized == 1);

  // compute actual size of block
  length = (length + 7) / 8 * 8; // round up to nearest multiple of 8
  length += 16;                  // add space for the header

  // heap traversal
  void *block_ptr;
  int64_t header;
  int64_t size;
  char flags;

  block_ptr = heap_base;
  while (block_ptr < heap_end) {
    header = *((int64_t *)block_ptr);
    flags = header & 0b111; // keep only three least significant bits
    size = header & ~0b111; // discard the three least significant bits

    if ((flags == 0) &&   // if current block is free, and
        (size >= length)) // is also large enough, then we have a winner
      break;

    // otherwise we move on to the next block
    block_ptr += size;
  }

  // if the heap  traversal reached this far, then it  means we have
  // found no suitable block, so we should return NULL
  if (block_ptr >= heap_end) {
    return NULL;
  }

  if (size > length) {
    int64_t new_size = size - length;
    int64_t new_header = new_size | 0b000;
    size = length;
    *(int64_t *)(block_ptr + size) = new_header; // header of the new block
    *(int64_t *)(block_ptr + size + new_size - 1) =
        new_header; // footer of the new block
  }

  flags = 0b001; // mark block as taken
  header = size | flags;
  *((int64_t *)block_ptr) = header; // write header back into the block
  *((int64_t *)(block_ptr + size - 1)) =
      header; // write footer back into the block

  return block_ptr + 8; // skip header
}

void mem_release(void *ptr) {
  assert(mem_initialized == 1);
  assert(((int64_t)ptr % 8) == 0); // sanity check
  ptr -= 8;
  printf("%p\n", ptr);
  const char flagsForFree = 0b000;
  const int64_t size = *(int64_t *)ptr & ~0b111;
  int64_t header = size | flagsForFree;

  *(int64_t *)ptr = header;
  *(int64_t *)(ptr + size - 1) = header;

  if (!(*(int64_t *)(ptr - 8) &
        0b111)) // if the footer of the block before is "free"
  {
    int64_t sizeOfBefore = *(int64_t *)(ptr - 8);
    ptr -= sizeOfBefore;
    const int64_t new_size = size + sizeOfBefore;
    header = size | flagsForFree;
    *(int64_t *)(ptr) = header;
    *(int64_t *)(ptr + new_size - 1) = header;
  }

//   if (!(*(int64_t *)(ptr - 8) &
//         0b111)) // if the footer of the block before is "free"
//   {
//     int64_t sizeOfBefore = *(int64_t *)(ptr - 8);
//     ptr -= sizeOfBefore;
//     const int64_t new_size = size + sizeOfBefore;
//     header = size | flagsForFree;
//     *(int64_t *)(ptr) = header;
//     *(int64_t *)(ptr + new_size - 1) = header;
//   }
}

void mem_show_heap(void) {
  assert(mem_initialized == 1);

  void *block_ptr = heap_base;

  printf("heap_base = %p\n", heap_base);
  while (block_ptr < heap_end) {
    int64_t header = *((int64_t *)block_ptr);
    char flags = header & 0b111;    //   keep only three least significant bits
    int64_t size = header & ~0b111; // discard the three least significant bits
    if ((size < 8) || (size % 8 != 0)) {
      printf("error: block at %p has incorrect size %ld\n", block_ptr, size);
      exit(1);
    }

    printf("  block at %p: header=0x%08lx size=%ld flags=%d (%s)\n", block_ptr,
           header, size, flags, (flags ? "taken" : "free"));

    block_ptr += size; // move on to next block
  }

  printf("heap_end = %p\n", heap_end);

  // sanity check: a full heap traversal should reach *exactly* the end
  assert(block_ptr == heap_end);
}
