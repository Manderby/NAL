
#include "NCParseEntity.h"
#include "NCParseTree.h"

struct NCParseEntity{
  NCParseTree* tree;
  NAString* string;
  NCParseEntityType type;
};



NCParseEntity* ncAllocParseEntity(
  NCParseTree* tree,
  NCParseEntityType type,
  NAString* string)
{
  NCParseEntity* entity = naAlloc(NCParseEntity);
  entity->tree = tree;
  entity->type = type;
  entity->string = string;
  return entity;
}



void ncDeallocParseEntity(NCParseEntity* entity){
  naDelete(entity->string);
  naFree(entity);
}
