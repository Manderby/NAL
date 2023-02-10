
#ifndef NC_GLOBAL_SYMBOL_INCLUDED
#define NC_GLOBAL_SYMBOL_INCLUDED

#include <NALib.h>



typedef struct NCGlobalSymbol NCGlobalSymbol;



NCGlobalSymbol* ncAllocGlobalSymbol(
  NAString* string);
  
NCGlobalSymbol* ncParseGlobalSymbol(
  NAString* string);

void ncDeallocGlobalSymbol(
  NCGlobalSymbol* symbol);


#endif // NC_GLOBAL_SYMBOL_INCLUDED
