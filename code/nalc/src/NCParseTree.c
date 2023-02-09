
#include "NCParseEntity.h"

struct NCParseTree{
  NCParseTree* parent;
  NAList entities;
};



NCParseTree* ncAllocParseTree(NCParseTree* parent)
{
  NCParseTree* tree = naAlloc(NCParseTree);
  tree->parent = parent;
  naInitList(&tree->entities);
  
  return tree;
}



void ncDeallocParseTree(NCParseTree* tree){

  NAListIterator listIter = naMakeListAccessor(&tree->entities);
  while(naIterateList(&listIter)){
    NCParseEntity* entity = naGetListCurConst(&listIter);
    if(ncGetParseEntityType(entity) == NC_ENTITY_TYPE_GLOBAL_LHS){
      int asdf = 1234;
    }
  }
  naClearListIterator(&listIter);

  naForeachListMutable(&tree->entities, (NAMutator)ncDeallocParseEntity);
  naClearList(&tree->entities);
  naFree(tree);
}



void ncAddParseTreeEntity(
  NCParseTree* tree,
  NCParseEntity* entity)
{
  naAddListLastMutable(&tree->entities, entity);
}



NCParseTree* ncGetParseTreeParent(NCParseTree* tree){
    return tree->parent;
}
