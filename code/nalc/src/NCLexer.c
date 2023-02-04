
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

void nc_ReadPotentialCommentBegin(NCLexer* lexer, NAUTF8Char c);
void nc_ReadMultiLineComment(NCLexer* lexer, NAUTF8Char c);
void nc_ReadPotentialCommentEnd(NCLexer* lexer, NAUTF8Char c);
void nc_ReadDefault(NCLexer* lexer, NAUTF8Char c);



void nc_ReadPotentialCommentEnd(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    NAInt endPos = naGetBufferLocation(&lexer->bufIter) - 2;
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntity(
      lexer->parseTree,
      NC_ENTITY_TYPE_MULTI_LINE_COMMENT,
      naNewStringWithBufferExtraction(
      lexer->inBuffer, naMakeRangeiWithStartAndEnd(lexer->startPos, endPos))));
    lexer->reader = nc_ReadDefault;
  }else{
    lexer->reader = nc_ReadMultiLineComment;
  }
}



void nc_ReadMultiLineComment(NCLexer* lexer, NAUTF8Char c){
  if(c == '*'){
    lexer->reader = nc_ReadPotentialCommentEnd;
  }
}



void nc_ReadPotentialCommentBegin(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    // We found a double slash comment.
    ncAddParseTreeEntity(lexer->parseTree, ncAllocParseEntity(
      lexer->parseTree,
      NC_ENTITY_TYPE_LINE_COMMENT,
      naParseBufferRemainder(&lexer->bufIter)));
    lexer->reader = nc_ReadDefault;
  }else if(c == '*'){
    lexer->reader = nc_ReadMultiLineComment;
    lexer->startPos = naGetBufferLocation(&lexer->bufIter);
  }else{
    lexer->reader = nc_ReadDefault;
    lexer->reader(lexer, c);
  }
}


void nc_ReadDefault(NCLexer* lexer, NAUTF8Char c){
  if(c == '/'){
    lexer->reader = nc_ReadPotentialCommentBegin;
  }
}



NCLexer* ncAllocLexer(void){
  NCLexer* lexer = naAlloc(NCLexer);
  
  lexer->reader = nc_ReadDefault;
  lexer->parseTree = ncAllocParseTree();
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

