
#ifndef NC_GLOBAL_SYMBOL_INCLUDED
#define NC_GLOBAL_SYMBOL_INCLUDED

#include <NALib.h>
#include "main.h"
NC_PROTOTYPE(NCString);



typedef struct NCGlobalSymbol NCGlobalSymbol;



//NCGlobalSymbol* ncAllocGlobalSymbol(
//  NAString* string);
  
//NCGlobalSymbol* ncParseGlobalSymbol(
//  NCString* string);
NCGlobalSymbol* ncParseGlobalSymbol(
  NAString* string);

void ncDeallocGlobalSymbol(
  NCGlobalSymbol* symbol);


#endif // NC_GLOBAL_SYMBOL_INCLUDED
