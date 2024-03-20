#ifndef OPTIMIZIER_H
#define OPTIMIZIER_H

#define ISNUM(side, value) ((side->Type == NUMBER) && (Compare(side->Value.Number, value)))
#define TREE_CHANGED (myTree->ChangeOptimize = 1)

EnumOfErrors TreeOptimize(BinaryTree_t* myTree);

#endif