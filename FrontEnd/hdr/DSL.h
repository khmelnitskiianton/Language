#ifndef DSL_H
#define DSL_H

#define L CurrentNode->Left
#define R CurrentNode->Right
#define P CurrentNode->Parent
#define C CurrentNode
#define T CurrentToken
#define PT PtrCurrentToken

#define DIF(Node) RecDiff(Node, myTree)
#define CPY(Node) RecDiffCreateNode(Node->Type, Node->Value, (Node->Left), (Node->Right))
#define OPR(OPR, Left, Right) DiffCreateNode(OPERATOR, {.Number = -1, .Index = OPR}, (Left), (Right))
#define NUM(Num) DiffCreateNode(NUMBER,   {.Number = Num}, NULL, NULL)
#define VAR(Var) DiffCreateNode(VARIABLE, {.Index = Var}, NULL, NULL)

#endif