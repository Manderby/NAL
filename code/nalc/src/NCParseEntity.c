
#include "NCParseEntity.h"
#include "NCParseTree.h"
#include "NCGlobalSymbol.h"

struct NCParseEntity{
  NCParseEntityType type;
  void* data;
};



NCParseEntity* ncAllocParseEntity(
  NCParseEntityType type,
  void* data)
{
  NCParseEntity* entity = naAlloc(NCParseEntity);
  entity->type = type;
  entity->data = data;
  return entity;
}



void nc_FreeParseEntityData(NCParseEntity* entity){
  switch(entity->type){
  case NC_ENTITY_TYPE_SCOPE:
    ncDeallocParseTree(entity->data);
    break;
  case NC_ENTITY_TYPE_GLOBAL_SYMBOL:
    ncDeallocGlobalSymbol(entity->data);
    break;
  default:
    if(entity->data){naDelete(entity->data);}
    break;
  }
}



void ncDeallocParseEntity(NCParseEntity* entity){
  nc_FreeParseEntityData(entity);
  naFree(entity);
}



void ncReplaceParseEntityData(
  NCParseEntity* entity,
  NCParseEntityType type,
  void* data)
{
  nc_FreeParseEntityData(entity);
  entity->type = type;
  entity->data = data;
}



NCParseEntityType ncGetParseEntityType(const NCParseEntity* entity){
  return entity->type;
}



void* ncGetParseEntityData(NCParseEntity* entity){
  return entity->data;
}

