
#include <stdlib.h>
#include "bircc/memory.h"


void * bircc_calloc(size_t size, size_t amount) {
  return calloc(size, amount);  
}

void * bircc_malloc(size_t size) {
  return malloc(size);  
}


void * bircc_realloc(void * old, size_t size) {
  return realloc(old, size);  
}

void bircc_free(void * old) {
  free(old);
}



