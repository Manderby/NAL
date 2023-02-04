

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
  
  ncSetInputPath(lexer, inPath);
  ncHandleFile(lexer);
  ncCloseFile(lexer);

  ncDeallocLexer(lexer);

  naStopRuntime();

  return 0;
}
