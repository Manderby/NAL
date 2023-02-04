
#ifndef NC_PARSE_TREE_INCLUDED
#define NC_PARSE_TREE_INCLUDED

#include <NALib.h>
#include "main.h"
NC_PROTOTYPE(NCParseEntity);



typedef struct NCParseTree NCParseTree;



NCParseTree* ncAllocParseTree(void);

void ncDeallocParseTree(
  NCParseTree* tree);

void ncAddParseTreeEntity(
  NCParseTree* tree,
  NCParseEntity* entity);

#endif // NC_PARSE_TREE_INCLUDED
