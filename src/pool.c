
#include <stdlib.h>

#include "bircc/pool.h"
#include "bircc/memory.h"


struct bircc_poolblock;

struct bircc_poolblock {
  /* Linked stack in reverse order of allocation. */
  struct bircc_poolblock * previous;
  /* Allocated pointer. */
  void * ptr;
};


struct bircc_pool {
  struct bircc_poolblock * poolblock;
};


struct bircc_pool * bircc_pool_alloc() {
  return bircc_calloc(sizeof(struct bircc_pool), 1);
}

/** Allocates a new memory pool and returns it or NULL if out of memory. */
struct bircc_pool * bircc_pool_make() {
  struct bircc_pool * me;
  me = bircc_pool_alloc();
  if (me) {
    me->poolblock = NULL;
  }
  return me;
}

/** Allocates a block of memory in the pool. Returns NULL if out of memory. */
void * bircc_pool_calloc(struct bircc_pool * me, size_t size) {
  struct bircc_poolblock * newblock;
  if (!me)             return NULL;
  newblock           = bircc_calloc(sizeof(struct bircc_poolblock), 1);
  if (!newblock)       return NULL;
  newblock->ptr      = bircc_calloc(size, 1);
  if (!newblock->ptr) {
    free(newblock);
    return NULL;
  }
  newblock->previous  = me->poolblock;
  me->poolblock       = newblock;
  return me->poolblock->ptr;
}


/** Deallocates the last pool allocated in the block. Returns 0 if no 
more allocations in the pool are left, 1 if any are left, -1 on error. */
int bircc_pool_freelast(struct bircc_pool * me) {
  struct bircc_poolblock * nowblock, * prevblock;
  if (!me)                  return -1;
  nowblock                = me->poolblock;
  if (!nowblock)            return 0;
  prevblock               = nowblock->previous;
  /* Free the pointer and the wrapper block. */
  bircc_free(nowblock->ptr);
  bircc_free(nowblock);
  /* And oldblock now is the me->poolblock to use */
  me->poolblock           = prevblock;
  if(me->poolblock)         return 1;
  return 0;
}



/** Frees the pool and all allocated block in it. */
void * bircc_pool_free(struct bircc_pool * me) {
  /* Keep freeing last until nothing to free or error. */
  while(bircc_pool_freelast(me) > 0);
  /* Free the pool itself; */
  bircc_free(me);
  return NULL;
}







