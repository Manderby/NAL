

#include <stdio.h>
#include "NALib.h"
#include "main.h"

#include "NCParseEntity.h"
#include "NCLexer.h"
#include "NCParseTree.h"

int main(int argc, const char * argv[]) {

  if(argc < 3){
    printf("Usage: nalc inputfile outputfile\n");
    return 1;
  }
  
  const NAUTF8Char* inPath = argv[1];
  const NAUTF8Char* outPath = argv[2];

  printf("Compiling input file %s\n", inPath);
  printf("Output file: %s\n", outPath);



  naStartRuntime();
  
  // Create the lexer.
  NCLexer* lexer = ncAllocLexer();
  
  NADateTime time0 = naMakeDateTimeNow();

  for(int i = 0; i < 1000; i++){
    ncSetInputPath(lexer, inPath);
    ncHandleFile(lexer);
    ncCloseFile(lexer);
  }

  NADateTime time1 = naMakeDateTimeNow();
  printf("Time difference: %f\n", naGetDateTimeDifference(&time1, &time0));

  ncDeallocLexer(lexer);

  naStopRuntime();

  return 0;
}
