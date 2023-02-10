
#ifndef NC_PARSE_ENTITY_INCLUDED
#define NC_PARSE_ENTITY_INCLUDED

#include <NALib.h>
#include "main.h"
NC_PROTOTYPE(NCParseTree);



typedef struct NCParseEntity NCParseEntity;

typedef enum NCParseEntityType{
  NC_ENTITY_TYPE_UNPARSED,
  NC_ENTITY_TYPE_GLOBAL_SYMBOL,
  NC_ENTITY_TYPE_GLOBAL_LHS,
  NC_ENTITY_TYPE_LINE_COMMENT,
  NC_ENTITY_TYPE_MULTI_LINE_COMMENT,
  NC_ENTITY_TYPE_SINGLE_QUOTE_CONTENT,
  NC_ENTITY_TYPE_DOUBLE_QUOTE_CONTENT,
  NC_ENTITY_TYPE_SCOPE,
} NCParseEntityType;



NCParseEntity* ncAllocParseEntity(
  NCParseEntityType type,
  void* data);
  
void ncDeallocParseEntity(
  NCParseEntity* entity);



void ncReplaceParseEntityData(
  NCParseEntity* entity,
  NCParseEntityType type,
  void* data);

NCParseEntityType ncGetParseEntityType(
  const NCParseEntity* entity);

void* ncGetParseEntityData(
  NCParseEntity* entity);

#endif // NC_PARSE_ENTITY_INCLUDED
