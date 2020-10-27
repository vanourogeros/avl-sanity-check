# avl-sanity-check

This is an AVL Sanity Check function integrated into the avltree.hpp header file implementing AVL trees in C++. (Respective owners are mentioned in the comments)

Specifically, this function checks the following:
* <b>Parent-child relations:</b> Whether each node has in their parent parameter a pointer pointing at the parent node (with the root having `nullptr` as a parent)
* <b>Binary Search Tree property:</b> For each node with value k, the values in the left subtree are smaller than k, and the values in the right subtree are larger than k. 
* <b>AVL Tree property:</b> For each node, the height difference of the left and right sub-tree is no more than one.
* <b>Balance info consistency:</b> The `balance` parameter for each node has the correct value LH, RH, or EH, according to whether the node is Left-High, Right-High, or Equal-Height respectively.
* <b>Tree size info consistency:</b> The `the_size` parameter has the correct number of nodes assigned to it.

All done in <b>O(n)</b> complexity, with <b>only one</b> recursion of the tree!
