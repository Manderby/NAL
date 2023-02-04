
#include "NCParseEntity.h"
#include "NCParseTree.h"

struct NCParseEntity{
  NCParseEntityType type;
  NAString* string;
  NCParseTree* tree;
};



NCParseEntity* ncAllocParseEntityString(
  NCParseEntityType type,
  NAString* string)
{
  NCParseEntity* entity = naAlloc(NCParseEntity);
  entity->type = type;
  entity->string = string;
  entity->tree = NA_NULL;
  return entity;
}



NCParseEntity* ncAllocParseEntityTree(
  NCParseEntityType type,
  NCParseTree* parentTree)
{
  NCParseEntity* entity = naAlloc(NCParseEntity);
  entity->type = type;
  entity->string = NA_NULL;
  entity->tree = ncAllocParseTree(parentTree);
  return entity;
}



void ncDeallocParseEntity(NCParseEntity* entity){
  if(entity->string){naDelete(entity->string);}
  if(entity->tree){ncDeallocParseTree(entity->tree);}
  naFree(entity);
}



NCParseTree* ncGetParseEntityTree(NCParseEntity* entity){
  return entity->tree;
}

