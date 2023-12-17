#ifndef DSL_H
#define DSL_H

#define L CurrentNode->Left
#define R CurrentNode->Right
#define P CurrentNode->Parent
#define C CurrentNode

#define DIF(Node) RecDiff(Node, myTree)
#define CPY(Node) RecDiffCreateNode(Node->Type, Node->Value, (Node->Left), (Node->Right))
#define OPR(OPR, Left, Right) DiffCreateNode(OPERATOR, {.Index = OPR}, (Left), (Right))
#define NUM(Num) DiffCreateNode(NUMBER, {.Number = Num}, NULL, NULL)

#endif