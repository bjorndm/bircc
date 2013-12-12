#ifndef BIRCC_MEMORY_H
#define  BIRCC_MEMORY_H


/** Wrappers around stdlib.h */
void * bircc_calloc(size_t size, size_t amount);
void * bircc_malloc(size_t size);
void * bircc_realloc(void * old, size_t size);
void bircc_free(void * old);

/** struct allocation helper macro. */
#define BIRCC_STRUCT_ALLOC(TYPE) (bircc_calloc(sizeof(TYPE), 1))

#endif