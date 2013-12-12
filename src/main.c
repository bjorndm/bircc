#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "bircc/cpp.h"




int main(int argc, char * argv[]) {  
  int res;
  bircc_sourcefile * src = NULL;
  printf("bircc - under development :p !\n");
  src = bircc_sourcefile_read("/home/bjorn/src/bircc/test/test_cpp_1.h");
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



