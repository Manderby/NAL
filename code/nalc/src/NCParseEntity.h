
#ifndef NC_PARSE_ENTITY_INCLUDED
#define NC_PARSE_ENTITY_INCLUDED

#include <NALib.h>
#include "main.h"
NC_PROTOTYPE(NCParseTree);



typedef struct NCParseEntity NCParseEntity;

typedef enum NCParseEntityType{
  NC_ENTITY_TYPE_UNPARSED,
  NC_ENTITY_TYPE_GLOBAL_LHS,
  NC_ENTITY_TYPE_LINE_COMMENT,
  NC_ENTITY_TYPE_MULTI_LINE_COMMENT,
  NC_ENTITY_TYPE_SINGLE_QUOTE_CONTENT,
  NC_ENTITY_TYPE_DOUBLE_QUOTE_CONTENT,
  NC_ENTITY_TYPE_SCOPE,
} NCParseEntityType;



NCParseEntity* ncAllocParseEntityString(
  NCParseEntityType type,
  NAString* string);
  
NCParseEntity* ncAllocParseEntityTree(
  NCParseEntityType type,
  NCParseTree* parentTree);

void ncDeallocParseEntity(
  NCParseEntity* entity);



NCParseEntityType ncGetParseEntityType(
  const NCParseEntity* entity);

NCParseTree* ncGetParseEntityTree(
  NCParseEntity* entity);

#endif // NC_PARSE_ENTITY_INCLUDED
