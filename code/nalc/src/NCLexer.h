
#ifndef NC_LEXER_INCLUDED
#define NC_LEXER_INCLUDED

#include <NALib.h>



typedef struct NCLexer NCLexer;



NCLexer* ncAllocLexer(void);
void ncDeallocLexer(NCLexer* lexer);

void ncSetInputPath(NCLexer* lexer, const char* path);
void ncHandleFile(NCLexer* lexer);
void ncCloseFile(NCLexer* lexer);

void printDebugInfo(NCLexer* lexer);

#endif // NC_LEXER_INCLUDED
