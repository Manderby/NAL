
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



NAList* ncGetParseTreeEntities(NCParseTree* tree){
  return &tree->entities;
}
