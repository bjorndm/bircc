#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "bircc/pool.h"

#include "bircc/cpp.h"




int main(int argc, char * argv[]) {  
  int res;
  int index;
  void * ptr[10];
  bircc_sourcefile * src = NULL;
  struct bircc_pool * pool = NULL;
  printf("bircc - under development :p !\n");
  
  pool = bircc_pool_make();
  for(index = 0; index < 10 ; index ++) {
    ptr[index] = bircc_pool_calloc(pool, index * 10);
  }
  bircc_pool_free(pool);
  
  src = bircc_sourcefile_read("test/data/test_cpp_1.h");
  if(!src) {
    puts("Out of memory");
    return 1;
  }
  
  printf("Before expand trigraphs\n");
  bircc_sourcefile_debugprint(src);
  res = bircc_sourcefile_expand_trigraphs(src);
  printf("Expand trigraphs: %d\n", res);
  bircc_sourcefile_debugprint(src);  
  bircc_sourcefile_free(src);  
  
  return 0;
}



