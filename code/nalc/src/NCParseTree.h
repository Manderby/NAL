
#ifndef NC_PARSE_TREE_INCLUDED
#define NC_PARSE_TREE_INCLUDED

#include <NALib.h>
#include "main.h"
NC_PROTOTYPE(NCParseEntity);



typedef struct NCParseTree NCParseTree;



NCParseTree* ncAllocParseTree(
  NCParseTree* parent);

void ncDeallocParseTree(
  NCParseTree* tree);



void ncAddParseTreeEntity(
  NCParseTree* tree,
  NCParseEntity* entity);

NCParseTree* ncGetParseTreeParent(
  NCParseTree* tree);

NAList* ncGetParseTreeEntities(
  NCParseTree* tree);

#endif // NC_PARSE_TREE_INCLUDED
