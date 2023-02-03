
#include <stdio.h>
#include "NALib.h"

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
  NABuffer* inBuffer = naCreateBufferWithInputPath(inPath);
  NABufferIterator bufIt = naMakeBufferAccessor(inBuffer);
  
  size_t lineNum = 1;
  while(!naIsBufferAtEnd(&bufIt)){
    NAString* line = naParseBufferLine(&bufIt, NA_FALSE);
    NABufferIterator lineIt = naMakeBufferAccessor(naGetStringBufferConst(line));
    
    // Overjump empty lines
    naSkipBufferWhitespaces(&lineIt);
    if(!naIsBufferAtEnd(&lineIt)){
    
      // Overjump lines which start with //
      NABool commentedOut = NA_FALSE;
      if(naGetBufferu8(&lineIt) == '/'){
        naIterateBuffer(&lineIt, 1);
        if(naGetBufferu8(&lineIt) == '/'){
          commentedOut = NA_TRUE;
        }else{
          naIterateBuffer(&lineIt, -1);
        }
      }
      if(!commentedOut){
      
        NAString* lhs = naParseBufferTokenWithDelimiter(&lineIt, '=', NA_TRUE);
        NAString* rhs = naParseBufferRemainder(&lineIt);
        
        int asdf = 1234;
      }
    }

    naClearBufferIterator(&lineIt);
    
    naDelete(line);
    ++lineNum;
  }

  naClearBufferIterator(&bufIt);
  naRelease(inBuffer);
  naStopRuntime();

  return 0;
}
