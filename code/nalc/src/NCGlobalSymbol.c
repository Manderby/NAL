
#include "NCGlobalSymbol.h"

struct NCGlobalSymbol{
  NAString* name;
};



NCGlobalSymbol* ncAllocGlobalSymbol(NAString* string){
  NCGlobalSymbol* symbol = naAlloc(NCGlobalSymbol);
  symbol->name = naNewStringExtraction(string, 0, -1);
  return symbol;
}
  
NCGlobalSymbol* ncParseGlobalSymbol(NAString* string){
  NCGlobalSymbol* symbol = naAlloc(NCGlobalSymbol);
  symbol->name = naNewStringExtraction(string, 0, -1);
  return symbol;
}

void ncDeallocGlobalSymbol(NCGlobalSymbol* symbol){
  naDelete(symbol->name);
  naFree(symbol);
}
