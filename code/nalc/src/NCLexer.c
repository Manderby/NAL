
#include "main.h"
#include "NCLexer.h"
#include "NCParseEntity.h"
#include "NCParseTree.h"


typedef void(*NCReader)(NCLexer*, NAUTF8Char);

struct NCLexer{
  NCParseTree* parseTree;

  NABuffer* inBuffer;
  NABufferIterator bufIter;
  
  NCReader reader;      // current state function pointer
  NCReader prevReader;  // previous state function pointer
  NAInt startPos;
};

void nc_ReadSingleLineComment(NCLexer* lexer, NAUTF8Char c);
void nc_ReadPotentialMultiLineCommentEnd(NCLexer* lexer, NAUTF8Char c);
void nc_ReadMultiLineComment(NCLexer* lexer, NAUTF8Char c);
void nc_ReadPotentialCommentBegin(NCLexer* lexer, NAUTF8Char c);

void nc_ReadSingleQuoteContent(NCLexer* lexer, NAUTF8Char c);
void nc_ReadDoubleQuoteContent(NCLexer* lexer, NAUTF8Char c);
void nc_ReadEscapeCharacter(NCLexer* lexer, NAUTF8Char c);

void nc_ReadDefault(NCLexer* lexer, NAUTF8Char c);



void nc_ReadSingleLineComment(NCLexer* lexer, NAUTF8Char c){
  if(c == '\n' || c == '\r'){
    NAInt endPos = naGetBufferLocation(&lexer->bufIter) - 1;
    NAString* content = endPos == lexer->startPos
      ? naNewString()
      : naNewStringWithBufferExtraction(
        lexer->inBuffer,
        naMakeRangeiWithStartAndEnd(lexer->startPos, endPos));
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntityString(
      NC_ENTITY_TYPE_LINE_COMMENT,
      content));
    lexer->reader = nc_ReadDefault;
  }
  // potentially, \\n should be detected, but for the moment, that is not
  // the plan, as we do not allow for C style macros.
}



void nc_ReadPotentialMultiLineCommentEnd(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    NAInt endPos = naGetBufferLocation(&lexer->bufIter) - 2;
    NAString* content = endPos == lexer->startPos
      ? naNewString()
      : naNewStringWithBufferExtraction(
        lexer->inBuffer,
        naMakeRangeiWithStartAndEnd(lexer->startPos, endPos));
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntityString(
      NC_ENTITY_TYPE_MULTI_LINE_COMMENT,
      content));
    lexer->reader = nc_ReadDefault;
  }else{
    lexer->reader = nc_ReadMultiLineComment;
    nc_ReadMultiLineComment(lexer, c);
  }
}



void nc_ReadMultiLineComment(NCLexer* lexer, NAUTF8Char c){
  if(c == '*'){
    lexer->reader = nc_ReadPotentialMultiLineCommentEnd;
  }
}



void nc_ReadPotentialCommentBegin(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    lexer->reader = nc_ReadSingleLineComment;
    lexer->startPos = naGetBufferLocation(&lexer->bufIter);
  }else if(c == '*'){
    lexer->reader = nc_ReadMultiLineComment;
    lexer->startPos = naGetBufferLocation(&lexer->bufIter);
  }else{
    lexer->reader = nc_ReadDefault;
    lexer->reader(lexer, c);
  }
}



void nc_ReadSingleQuoteContent(NCLexer* lexer, NAUTF8Char c){
  if(c == '\''){
    NAInt endPos = naGetBufferLocation(&lexer->bufIter) - 1;
    NAString* content = endPos == lexer->startPos
      ? naNewString()
      : naNewStringWithBufferExtraction(
        lexer->inBuffer,
        naMakeRangeiWithStartAndEnd(lexer->startPos, endPos));
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntityString(
      NC_ENTITY_TYPE_SINGLE_QUOTE_CONTENT,
      content));
    lexer->reader = nc_ReadDefault;
  }else if(c == '\\'){
    lexer->reader = nc_ReadEscapeCharacter;
    lexer->prevReader = nc_ReadSingleQuoteContent;
  }
}



void nc_ReadDoubleQuoteContent(NCLexer* lexer, NAUTF8Char c){
  if(c == '\"'){
    NAInt endPos = naGetBufferLocation(&lexer->bufIter) - 1;
    NAString* content = endPos == lexer->startPos
      ? naNewString()
      : naNewStringWithBufferExtraction(
        lexer->inBuffer,
        naMakeRangeiWithStartAndEnd(lexer->startPos, endPos));
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntityString(
      NC_ENTITY_TYPE_DOUBLE_QUOTE_CONTENT,
      content));
    lexer->reader = nc_ReadDefault;
  }else if(c == '\\'){
    lexer->reader = nc_ReadEscapeCharacter;
    lexer->prevReader = nc_ReadDoubleQuoteContent;
  }
}



void nc_ReadEscapeCharacter(NCLexer* lexer, NAUTF8Char c){
  // todo: nothing to do right now, as we only consider one single character.
  // in the future, allow for hexadecimal escape codes.
  lexer->reader = lexer->prevReader;
}



void nc_ReadDefault(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    lexer->reader = nc_ReadPotentialCommentBegin;
  }else if(c == '\''){
    lexer->reader = nc_ReadSingleQuoteContent;
    lexer->startPos = naGetBufferLocation(&lexer->bufIter);
  }else if(c == '\"'){
    lexer->reader = nc_ReadDoubleQuoteContent;
    lexer->startPos = naGetBufferLocation(&lexer->bufIter);
  }else if(c == '{'){
    NCParseEntity* scope = ncAllocParseEntityTree(
      NC_ENTITY_TYPE_SCOPE,
      lexer->parseTree);
    ncAddParseTreeEntity(lexer->parseTree, scope);
    lexer->parseTree = ncGetParseEntityTree(scope);
  }else if(c == '}'){
    lexer->parseTree = ncGetParseTreeParent(lexer->parseTree);
  }
}



NCLexer* ncAllocLexer(void){
  NCLexer* lexer = naAlloc(NCLexer);
  
  lexer->parseTree = ncAllocParseTree(NA_NULL);
  lexer->inBuffer = NA_NULL;

  lexer->reader = nc_ReadDefault;
  
  return lexer;
}



void ncDeallocLexer(NCLexer* lexer){
  #if NA_DEBUG
    if(lexer->inBuffer)
      naError("Input file still open");
  #endif
  ncDeallocParseTree(lexer->parseTree);
  naFree(lexer);
}



void ncSetInputPath(NCLexer* lexer, const char* path){
  #if NA_DEBUG
    if(lexer->inBuffer)
      naError("Input file already set and active");
  #endif
  lexer->inBuffer = naCreateBufferWithInputPath(path);
  lexer->bufIter = naMakeBufferAccessor(lexer->inBuffer);

  // At the beginning of a file, we start with the default reader.
  lexer->reader = nc_ReadDefault;
}



void ncHandleFile(NCLexer* lexer){
  while(!naIsBufferAtEnd(&lexer->bufIter)){
    NAUTF8Char c = naReadBufferu8(&lexer->bufIter);
    lexer->reader(lexer, c);
  }
}



void ncCloseFile(NCLexer* lexer){
  naClearBufferIterator(&lexer->bufIter);
  naRelease(lexer->inBuffer);
  lexer->inBuffer = NA_NULL;
}

