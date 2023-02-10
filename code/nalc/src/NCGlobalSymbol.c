
#include "NCGlobalSymbol.h"
#include "NCString.h"

struct NCGlobalSymbol{
//  NCString* name;
  NAString* name;
};



//NCGlobalSymbol* ncAllocGlobalSymbol(NAString* string){
//  NCGlobalSymbol* symbol = naAlloc(NCGlobalSymbol);
//  symbol->name = naNewStringExtraction(string, 0, -1);
//  return symbol;
//}
  
//NCGlobalSymbol* ncParseGlobalSymbol(NCString* string){
//  NCGlobalSymbol* symbol = naAlloc(NCGlobalSymbol);
//  symbol->name = ncAllocString(ncGetStringPtr(string), ncGetStringLen(string));
//  return symbol;
//}
NCGlobalSymbol* ncParseGlobalSymbol(NAString* string){
  NCGlobalSymbol* symbol = naAlloc(NCGlobalSymbol);
  symbol->name = naNewStringExtraction(string, 0, -1);
  return symbol;
}

void ncDeallocGlobalSymbol(NCGlobalSymbol* symbol){
//  ncDeallocString(symbol->name);
  naDelete(symbol->name);
  naFree(symbol);
}
