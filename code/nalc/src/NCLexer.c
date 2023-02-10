
#include "main.h"
#include "NCGlobalSymbol.h"
#include "NCLexer.h"
#include "NCParseEntity.h"
#include "NCParseTree.h"


typedef void(*NCReader)(NCLexer*, NAUTF8Char);

struct NCLexer{
  NCParseTree* parseTree;

  NABuffer* inBuffer;
  NABufferIterator bufIter;
  
  NAStack readerStack;
  NAInt startPos;
};

void nc_ReadSingleLineComment(NCLexer* lexer, NAUTF8Char c);
void nc_ReadPotentialMultiLineCommentEnd(NCLexer* lexer, NAUTF8Char c);
void nc_ReadMultiLineComment(NCLexer* lexer, NAUTF8Char c);
void nc_ReadPotentialCommentBegin(NCLexer* lexer, NAUTF8Char c);

void nc_ReadSingleQuoteContent(NCLexer* lexer, NAUTF8Char c);
void nc_ReadDoubleQuoteContent(NCLexer* lexer, NAUTF8Char c);
void nc_ReadEscapeCharacter(NCLexer* lexer, NAUTF8Char c);

void nc_ReadLHS(NCLexer* lexer, NAUTF8Char c);
void nc_ReadRHS(NCLexer* lexer, NAUTF8Char c);
void nc_ReadCommon(NCLexer* lexer, NAUTF8Char c);

NCParseEntityType nc_GetGlobalEntityType(NCLexer* lexer);
void nc_CreateParseEntity(NCLexer* lexer, NAInt backOffset, NCParseEntityType type);
NABool nc_IsLexerInGlobalScope(NCLexer* lexer);
void nc_SetLexerReader(NCLexer* lexer, NCReader newReader);
void nc_PushLexerReader(NCLexer* lexer, NCReader reader);
void nc_PopLexerReader(NCLexer* lexer);
void nc_CallLexerReader(NCLexer* lexer, NAUTF8Char c);



void nc_ReadSingleLineComment(NCLexer* lexer, NAUTF8Char c){
  if(c == '\n' || c == '\r'){
    nc_CreateParseEntity(lexer, 1, NC_ENTITY_TYPE_LINE_COMMENT);
    nc_PopLexerReader(lexer);
  }
  // potentially, \\n should be detected, but for the moment, that is not
  // the plan, as we do not allow for C style macros.
}



void nc_ReadPotentialMultiLineCommentEnd(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    nc_CreateParseEntity(lexer, 2, NC_ENTITY_TYPE_MULTI_LINE_COMMENT);
    nc_PopLexerReader(lexer);
  }else{
    nc_SetLexerReader(lexer, nc_ReadMultiLineComment);
    nc_CallLexerReader(lexer, c);
  }
}



void nc_ReadMultiLineComment(NCLexer* lexer, NAUTF8Char c){
  if(c == '*'){
    nc_SetLexerReader(lexer, nc_ReadPotentialMultiLineCommentEnd);
  }
}



void nc_ReadPotentialCommentBegin(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    nc_CreateParseEntity(lexer, 2, NC_ENTITY_TYPE_UNPARSED);
    nc_SetLexerReader(lexer, nc_ReadSingleLineComment);
  }else if(c == '*'){
    nc_CreateParseEntity(lexer, 2, NC_ENTITY_TYPE_UNPARSED);
    nc_SetLexerReader(lexer, nc_ReadMultiLineComment);
  }else{
    nc_PopLexerReader(lexer);
    nc_CallLexerReader(lexer, c);
  }
}



void nc_ReadSingleQuoteContent(NCLexer* lexer, NAUTF8Char c){
  if(c == '\''){
    nc_CreateParseEntity(lexer, 1, NC_ENTITY_TYPE_SINGLE_QUOTE_CONTENT);
    nc_PopLexerReader(lexer);
  }else if(c == '\\'){
    nc_PushLexerReader(lexer, nc_ReadEscapeCharacter);
  }
}



void nc_ReadDoubleQuoteContent(NCLexer* lexer, NAUTF8Char c){
  if(c == '\"'){
    nc_CreateParseEntity(lexer, 1, NC_ENTITY_TYPE_DOUBLE_QUOTE_CONTENT);
    nc_PopLexerReader(lexer);
  }else if(c == '\\'){
    nc_PushLexerReader(lexer, nc_ReadEscapeCharacter);
  }
}



void nc_ReadEscapeCharacter(NCLexer* lexer, NAUTF8Char c){
  // todo: nothing to do right now, as we only consider one single character.
  // in the future, allow for hexadecimal escape codes.
  nc_PopLexerReader(lexer);
}



void nc_ReadLHS(NCLexer* lexer, NAUTF8Char c){
  nc_ReadCommon(lexer, c);
}

void nc_ReadRHS(NCLexer* lexer, NAUTF8Char c){
  nc_ReadCommon(lexer, c);
}

void nc_ReadCommon(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    nc_PushLexerReader(lexer, nc_ReadPotentialCommentBegin);
  }else if(c == '\''){
    nc_CreateParseEntity(lexer, 1, NC_ENTITY_TYPE_UNPARSED);
    nc_PushLexerReader(lexer, nc_ReadSingleQuoteContent);
  }else if(c == '\"'){
    nc_CreateParseEntity(lexer, 1, NC_ENTITY_TYPE_UNPARSED);
    nc_PushLexerReader(lexer, nc_ReadDoubleQuoteContent);
  }else if(c == '='){
    if(nc_IsLexerInGlobalScope(lexer)){
      nc_CreateParseEntity(lexer, 1, nc_GetGlobalEntityType(lexer));
      nc_SetLexerReader(lexer, nc_ReadRHS);
    }
  }else if(c == ';'){
    if(nc_IsLexerInGlobalScope(lexer)){
      nc_CreateParseEntity(lexer, 1, nc_GetGlobalEntityType(lexer));
      nc_SetLexerReader(lexer, nc_ReadLHS);
    }
  }else if(c == '{'){
    nc_CreateParseEntity(lexer, 1, NC_ENTITY_TYPE_UNPARSED);
    NCParseEntity* scope = ncAllocParseEntity(
      NC_ENTITY_TYPE_SCOPE,
      ncAllocParseTree(lexer->parseTree));
    ncAddParseTreeEntity(lexer->parseTree, scope);
    lexer->parseTree = ncGetParseEntityData(scope);
    nc_PushLexerReader(lexer, nc_ReadCommon);
  }else if(c == '}'){
    nc_PopLexerReader(lexer);
    if(nc_IsLexerInGlobalScope(lexer)){
      nc_CreateParseEntity(lexer, 1, nc_GetGlobalEntityType(lexer));
      nc_SetLexerReader(lexer, nc_ReadLHS);
    }
    lexer->parseTree = ncGetParseTreeParent(lexer->parseTree);
  }
}



NCLexer* ncAllocLexer(void){
  NCLexer* lexer = naAlloc(NCLexer);
  
  lexer->parseTree = ncAllocParseTree(NA_NULL);
  lexer->inBuffer = NA_NULL;

  naInitStack(&lexer->readerStack, sizeof(NCReader), 0, 0);
  
  return lexer;
}



void ncDeallocLexer(NCLexer* lexer){
  #if NA_DEBUG
    if(lexer->inBuffer)
      naError("Input file still open");
  #endif
  naClearStack(&lexer->readerStack);
  ncDeallocParseTree(lexer->parseTree);
  naFree(lexer);
}



NCParseEntityType nc_GetGlobalEntityType(NCLexer* lexer){
  NCReader* reader = (NCReader*)naTopStack(&lexer->readerStack);
  return (nc_IsLexerInGlobalScope(lexer) && (*reader == nc_ReadLHS))
    ? NC_ENTITY_TYPE_GLOBAL_LHS
    : NC_ENTITY_TYPE_UNPARSED;
}

// The backOffset parameter is how many characters were used to "close" the
// token. For exampe a /**/ comment requires a backOffset of 2 because the
// token is closed with th */ string.
void nc_CreateParseEntity(NCLexer* lexer, NAInt backOffset, NCParseEntityType type){
  NAInt endPos = naGetBufferLocation(&lexer->bufIter) - backOffset;
  NAString* content = endPos == lexer->startPos
    ? naNewString()
    : naNewStringWithBufferExtraction(
      lexer->inBuffer,
      naMakeRangeiWithStartAndEnd(lexer->startPos, endPos));
  ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntity(
    type,
    content));
  lexer->startPos = naGetBufferLocation(&lexer->bufIter);
}

NABool nc_IsLexerInGlobalScope(NCLexer* lexer){
  return (naGetStackCount(&lexer->readerStack) == 1);
}

void nc_SetLexerReader(NCLexer* lexer, NCReader newReader){
  NCReader* reader = (NCReader*)naTopStack(&lexer->readerStack);
  *reader = newReader;
}

void nc_PushLexerReader(NCLexer* lexer, NCReader reader){
  *(NCReader*)naPushStack(&lexer->readerStack) = reader;
}

void nc_PopLexerReader(NCLexer* lexer){
  naPopStack(&lexer->readerStack);
}

void nc_CallLexerReader(NCLexer* lexer, NAUTF8Char c){
  NCReader* reader = (NCReader*)naTopStack(&lexer->readerStack);
  (*reader)(lexer, c);
}



void ncSetInputPath(NCLexer* lexer, const char* path){
  #if NA_DEBUG
    if(lexer->inBuffer)
      naError("Input file already set and active");
  #endif
  lexer->inBuffer = naCreateBufferWithInputPath(path);
  lexer->bufIter = naMakeBufferAccessor(lexer->inBuffer);

  // At the beginning of a file, we start with the default reader.
  nc_PushLexerReader(lexer, nc_ReadLHS);
  lexer->startPos = 0;
}



void ncHandleFile(NCLexer* lexer){
  // Read the whole file and create coarse lexer tokens.
  while(!naIsBufferAtEnd(&lexer->bufIter)){
    NAUTF8Char c = naReadBufferu8(&lexer->bufIter);
    nc_CallLexerReader(lexer, c);
  }

  // Go through all LHS entities and parse them.
  NAList* entities = ncGetParseTreeEntities(lexer->parseTree);
  NAListIterator listIter = naMakeListMutator(entities);
  while(naIterateList(&listIter)){
    NCParseEntity* entity = naGetListCurMutable(&listIter);
    if(ncGetParseEntityType(entity) == NC_ENTITY_TYPE_GLOBAL_LHS){
      NCGlobalSymbol* symbol = ncParseGlobalSymbol(ncGetParseEntityData(entity));
      ncReplaceParseEntityData(entity, NC_ENTITY_TYPE_GLOBAL_SYMBOL, symbol);
      int asdf = 1234;
    }
  }
  naClearListIterator(&listIter);
}



void ncCloseFile(NCLexer* lexer){
  naClearBufferIterator(&lexer->bufIter);
  naRelease(lexer->inBuffer);
  lexer->inBuffer = NA_NULL;
}

