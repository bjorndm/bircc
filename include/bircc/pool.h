#ifndef BIRCC_POOL_H_INCLUDED
#define BIRCC_POOL_H_INCLUDED

struct bircc_pool;

/** Allocates a new memory pool and returns it or NULL if out of memory. */
struct bircc_pool * bircc_pool_make();
/** Allocates a block of memory in the pool. Returns NULL if out of memory.
Please include stdlib.h to provide size_t.
*/
void * bircc_pool_calloc(struct bircc_pool * me, size_t size);
/** Frees the pool and all allocated block in it. */
void * bircc_pool_free(struct bircc_pool * me);



#endif
