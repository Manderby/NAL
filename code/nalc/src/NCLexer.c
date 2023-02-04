
#include "main.h"
#include "NCLexer.h"
#include "NCParseEntity.h"
#include "NCParseTree.h"


typedef void(*NCReader)(NCLexer*, NAUTF8Char);

struct NCLexer{
  NCReader reader;
  
  NCParseTree* parseTree;

  NABuffer* inBuffer;
  NABufferIterator bufIter;
  
  NAInt startPos;
};

void nc_ReadPotentialMultiLineCommentEnd(NCLexer* lexer, NAUTF8Char c);
void nc_ReadSingleLineComment(NCLexer* lexer, NAUTF8Char c);
void nc_ReadMultiLineComment(NCLexer* lexer, NAUTF8Char c);
void nc_ReadPotentialCommentEnd(NCLexer* lexer, NAUTF8Char c);

void nc_ReadSingleQuoteContent(NCLexer* lexer, NAUTF8Char c);
void nc_ReadSingleQuoteEscapeCharacter(NCLexer* lexer, NAUTF8Char c);
void nc_ReadDoubleQuoteContent(NCLexer* lexer, NAUTF8Char c);
void nc_ReadDoubleQuoteEscapeCharacter(NCLexer* lexer, NAUTF8Char c);

void nc_ReadDefault(NCLexer* lexer, NAUTF8Char c);



void nc_ReadPotentialMultiLineCommentEnd(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    NAInt endPos = naGetBufferLocation(&lexer->bufIter) - 2;
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntityString(
      NC_ENTITY_TYPE_MULTI_LINE_COMMENT,
      naNewStringWithBufferExtraction(
        lexer->inBuffer,
        naMakeRangeiWithStartAndEnd(lexer->startPos, endPos))));
    lexer->reader = nc_ReadDefault;
  }else{
    lexer->reader = nc_ReadMultiLineComment;
  }
}



void nc_ReadSingleLineComment(NCLexer* lexer, NAUTF8Char c){
  if(c == '\n' || c == '\r'){
    NAInt endPos = naGetBufferLocation(&lexer->bufIter) - 1;
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntityString(
      NC_ENTITY_TYPE_LINE_COMMENT,
      naNewStringWithBufferExtraction(
        lexer->inBuffer,
        naMakeRangeiWithStartAndEnd(lexer->startPos, endPos))));
    lexer->reader = nc_ReadDefault;
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
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntityString(
      NC_ENTITY_TYPE_SINGLE_QUOTE_CONTENT,
      naNewStringWithBufferExtraction(
        lexer->inBuffer,
        naMakeRangeiWithStartAndEnd(lexer->startPos, endPos))));
    lexer->reader = nc_ReadDefault;
  }else if(c == '\\'){
    lexer->reader = nc_ReadSingleQuoteEscapeCharacter;
  }
}



void nc_ReadSingleQuoteEscapeCharacter(NCLexer* lexer, NAUTF8Char c){
  // todo: nothing to do right now, as we only consider one single character.
  // in the future, allow for hexadecimal escape codes.
  lexer->reader = nc_ReadSingleQuoteContent;
}



void nc_ReadDoubleQuoteContent(NCLexer* lexer, NAUTF8Char c){
  if(c == '\"'){
    NAInt endPos = naGetBufferLocation(&lexer->bufIter) - 1;
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntityString(
      NC_ENTITY_TYPE_DOUBLE_QUOTE_CONTENT,
      naNewStringWithBufferExtraction(
        lexer->inBuffer,
        naMakeRangeiWithStartAndEnd(lexer->startPos, endPos))));
    lexer->reader = nc_ReadDefault;
  }else if(c == '\\'){
    lexer->reader = nc_ReadDoubleQuoteEscapeCharacter;
  }
}



void nc_ReadDoubleQuoteEscapeCharacter(NCLexer* lexer, NAUTF8Char c){
  // todo: nothing to do right now, as we only consider one double character.
  // in the future, allow for hexadecimal escape codes.
  lexer->reader = nc_ReadDoubleQuoteContent;
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
    NAInt testPos = naGetBufferLocation(&lexer->bufIter) - 1;
    NCParseEntity* scope = ncAllocParseEntityTree(
      NC_ENTITY_TYPE_SCOPE,
      lexer->parseTree);
    ncAddParseTreeEntity(lexer->parseTree, scope);
    lexer->parseTree = ncGetParseEntityTree(scope);
  }else if(c == '}'){
    NAInt testPos = naGetBufferLocation(&lexer->bufIter) - 1;
    lexer->parseTree = ncGetParseTreeParent(lexer->parseTree);
  }
}



NCLexer* ncAllocLexer(void){
  NCLexer* lexer = naAlloc(NCLexer);
  
  lexer->reader = nc_ReadDefault;
  lexer->parseTree = ncAllocParseTree(NA_NULL);
  lexer->inBuffer = NA_NULL;
  
  return lexer;
}



void ncDeallocLexer(NCLexer* lexer){
  if(lexer->inBuffer){naRelease(lexer->inBuffer);}
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

