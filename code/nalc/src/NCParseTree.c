
#include "NCParseEntity.h"

struct NCParseTree{
  NAList entities;
};



NCParseTree* ncAllocParseTree(void)
{
  NCParseTree* tree = naAlloc(NCParseTree);
  naInitList(&(tree->entities));
  
  return tree;
}



void ncDeallocParseTree(NCParseTree* tree){
  naForeachListMutable(&(tree->entities), (NAMutator)ncDeallocParseEntity);
  naClearList(&(tree->entities));
  naFree(tree);
}



void ncAddParseTreeEntity(
  NCParseTree* tree,
  NCParseEntity* entity)
{
  naAddListLastMutable(&(tree->entities), entity);
}
