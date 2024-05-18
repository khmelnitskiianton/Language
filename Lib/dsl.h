#ifndef DSL_H
#define DSL_H
//DSL FOR WRITING ADDRESSING FOR NODES SIMPLER

#define L CurrentNode->Left
#define R CurrentNode->Right
#define P CurrentNode->Parent
#define C CurrentNode

#define DIF(Node) RecDiff(Node, myTree)
#define CPY(Node) RecDiffCreateNode(Node->Type, Node->Value, (Node->Left), (Node->Right))
#define OPR(TREE, OPR, Left, Right) DiffCreateNode(TREE, OPERATOR, {.Number = -1, .Index = OPR}, (Left), (Right))
#define NUM(TREE, Num) DiffCreateNode(TREE, NUMBER,   {.Number = Num}, NULL, NULL)
#define VAR(TREE, Var) DiffCreateNode(TREE, VARIABLE, {.Index = Var}, NULL, NULL)

#endif