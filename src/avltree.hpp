#include "container.hpp"

/* Reference implementation of AVL trees in C++, used in the course
 * "Programming Techniques" at the School of Electrical and Computer
 * Engineering of the National Technical University of Athens.
 *
 * This implementation is based on the C implementation written by
 * Eric Biggers <ebiggers3@gmail.com>, available here:
 * https://github.com/ebiggers/avl_tree
 * Some comments in the code below are taken verbatim from Eric's sources.
 *
 * Written in 2020 by Nikolaos Papaspyrou <nickie@softlab.ntua.gr>,
 * maintained and available here:
 * https://git.softlab.ntua.gr/pub/avl-tree
 *
 * sanity() function written by Ioannis Protogeros (see line 47)
 */

template <typename T>
class avltree : public Container<T>, public Iterable<T> {
public:
  // Constructor: empty tree.
  avltree() : root(nullptr), the_size(0) {}
  // Copy constructor.
  avltree(const avltree &t) : root(copy(t.root)), the_size(t.the_size) {}
  // Destructor.
  virtual ~avltree() override { purge(root); }

  // Assignment operator.
  avltree &operator=(const avltree &t) {
    purge(root);
    root = copy(t.root);
    the_size = t.the_size;
    return *this;
  }

  // Returns the number of nodes.
  virtual int size() const override { return the_size; }

  // Clears the tree, removing all nodes.
  virtual void clear() override {
    purge(root);
    root = nullptr;
    the_size = 0;
  }

  bool sanity() const {
	  int n = 0;
	  if (root == nullptr) return n == the_size;
	  return insanity(root, n) >= 0 && n == the_size; // see private (line 77)
  }

private:
  // Balance type for each node (left-high, equal-high, right-high).
  // Notice that -2 and +2 may also appear, before rebalancing.
  enum balance_type : signed char { LH = -1, EH = 0, RH = +1 };

  static balance_type adjust_balance(balance_type b, signed char sign) {
    return static_cast<balance_type>(b + sign);
  }

  static balance_type negate_balance(balance_type b) {
    return static_cast<balance_type>(-b);
  }

  // The type of the tree's node.
  // It contains a pointer to the parent, which is nullptr for the tree's root.
  struct node {
    T data;
    balance_type balance;
    node *left, *right, *parent;

    node(const T &x, node *p = nullptr)
        : data(x), balance(EH), left(nullptr), right(nullptr), parent(p) {}
  };

  int insanity(node* t, int& n, int depth = 0, node* p = nullptr, T min = 0, T max = 0, bool isleft = 0) const { 
	  if (t == nullptr) return depth - 1;
	  bool isBST, isParent, imbCheck, lnewleft; // conditions for each subtree
	  T lnewmin, lnewmax, rnewmin, rnewmax; // recursion parameters 
	  n++;
	  isParent = t->parent == p; // parent - child connections
	  
	  if (t == root) { // BST check
		  isBST = 1;
		  lnewmin = lnewmax = rnewmin = rnewmax = t->data;
	  }
	  else if (min == max) {
		  if (isleft) {
			  isBST = t->data < t->parent->data;
			  lnewmin = lnewmax = rnewmin = t->data;
			  rnewmax = max;
		  }
		  else {
			  isBST = t->data > t->parent->data;
			  rnewmin = rnewmax = lnewmax = t->data;
			  lnewmin = min;
		  }
	  }
	  else {
		  isBST = t->data > min&& t->data < max;
		  lnewmin = min; lnewmax = t->data;
		  rnewmin = t->data; rnewmax = max;
	  }

	  // imbalance (AVL) check
	  int l = insanity(t->left, n, depth + 1, t, lnewmin, lnewmax, 1);
	  int r = insanity(t->right, n, depth + 1, t, rnewmin, rnewmax, 0);
	  int imb = r - l;
	  imbCheck = imb == t->balance;
	  if (imb <= 1 && imb >= -1 && l >= 0 && r >= 0 && isBST && isParent && imbCheck) // requirements for each subtree
		  if (l >= r) return l;
		  else return r;
	  else return -1; // if even one condition is false, function returns -1 instead of height
  }

  // Returns a node's left child (if sign < 0) or right child (otherwise).
  // A reference to the child's pointer is returned, so that this function
  // can be used in the LHS of an assignment, e.g.,
  //
  //   child(t, +1) = r;
  //
  // which makes r the right child of node t.
  static node *&child(node *t, signed char sign) {
    return sign < 0 ? t->left : t->right;
  }

  // Replaces old_child with new_child in node t.
  // This assumes that old_child is indeed a child of t.
  // If t is nullptr, it sets the tree's root pointer.
  void replace_child(node *t, node *old_child, node *new_child) {
    if (t == nullptr)
      root = new_child;
    else if (old_child == t->left)
      t->left = new_child;
    else
      t->right = new_child;
  }

  // The tree's fields.
  node *root;
  int the_size;

  // Recursively copies the subtree pointed to by t and returns an
  // identical subtree.  The root of the copy will have p as its parent.
  static node *copy(node *t, node *p = nullptr) {
    if (t == nullptr) return nullptr;
    node *n = new node(t->data, p);
    n->left = copy(t->left, n);
    n->right = copy(t->right, n);
    n->balance = t->balance;
    return n;
  }

  // Recursively deletes the subtree pointed to by t.
  static void purge(node *t) {
    if (t != nullptr) {
      purge(t->left);
      purge(t->right);
      delete t;
    }
  }

  // Returns the node with the minimum value in the subtree pointed to by t,
  // i.e., it goes down and to the left until that's not possible.
  static node *leftdown(node *t) {
    if (t == nullptr) return nullptr;
    while (t->left != nullptr) t = t->left;
    return t;
  }

  // Returns the next larger node than those contained in the subtree pointed
  // to by t, i.e., it goes up until it reaches a parent from its left child.
  static node *leftup(node *t) {
    while (t->parent != nullptr && t->parent->left != t)
      t = t->parent;
    return t->parent;
  }

public:
  // Insert x in the tree.
  void insert(const T &x) {
    if (root == nullptr) {
      root = new node(x);
      ++the_size;
    } else {
      node *p = insert(root, x);
      if (p != nullptr) {
        ++the_size;
        rebalance_after_insert(p);
      }
    }
  }

private:
  // Insert x in the subtree pointed to by t.
  // Returns the new node, if it was inserted, otherwise nullptr.
  static node *insert(node *t, const T &x) {
    while (true) {
      if (x < t->data) {
        if (t->left == nullptr)
          return (t->left = new node(x, t));
        else
          t = t->left;
      } else if (x > t->data) {
        if (t->right == nullptr)
          return (t->right = new node(x, t));
        else
          t = t->right;
      } else
        return nullptr;
    }
  }

  // Rebalance the tree after insertion of the specified node.
  void rebalance_after_insert(node *t) {
    // Adjust balance factor of new node's parent.
    // No rotation will need to be done at this level.
    node *p = t->parent;
    if (p == nullptr) return;
    p->balance = adjust_balance(p->balance, t == p->left ? -1 : +1);
    // If parent did not change in height, nothing more to do.
    if (p->balance == EH) return;
    // The subtree rooted at parent increased in height by 1.
    bool done;
    do {
      // Adjust balance factor of next ancestor.
      t = p;
      p = p->parent;
      if (p == nullptr) return;
      // The subtree rooted at t has increased in height by 1.
      done = handle_subtree_growth(t, p, t == p->left ? -1 : +1);
    } while (!done);
  }

  /*
   * This function handles the growth of a subtree due to an insertion.
   *
   * t: a subtree that has increased in height by 1 due to an insertion.
   * p: parent of t; must not be nullptr.
   * sign:
   *	-1 if t is the left child of p;
   *	+1 if t is the right child of p.
   *
   * This function will adjust p's balance factor, then do a (single
   * or double) rotation if necessary.  The return value will be true if
   * the full AVL tree is now adequately balanced, or false if the subtree
   * rooted at p is now adequately balanced but has increased in
   * height by 1, so the caller should continue up the tree.
   *
   * Note that if false is returned, no rotation will have been done.
   * Indeed, a single node insertion cannot require that more than one
   * (single or double) rotation be done.
   */
  bool handle_subtree_growth(node *t, node *p, signed char sign) {
    balance_type old_balance_factor = p->balance;
    balance_type new_balance_factor = adjust_balance(old_balance_factor, sign);

    if (old_balance_factor == EH) {
      p->balance = new_balance_factor;
      // Parent is still sufficiently balanced (-1 or +1 balance factor),
      // but must have increased in height.
      // Continue up the tree.
      return false;
    }

    if (new_balance_factor == EH) {
      p->balance = new_balance_factor;
      // Parent is now perfectly balanced (0 balance factor).
      // It cannot have increased in height, so there is nothing more to do.
      return true;
    }

    // Parent is too left-heavy (new_balance_factor == -2) or
    // too right-heavy (new_balance_factor == +2).

    /* Test whether t is left-heavy (-1 balance factor) or
     * right-heavy (+1 balance factor).
     * Note that it cannot be perfectly balanced (0 balance factor)
     * because here we are under the invariant that t has
     * increased in height due to the insertion.  */
    if (sign * t->balance > 0) {

      /* t (B below) is heavy in the same direction p (A below) is heavy.
       *
       * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
       * The comment, diagram, and equations below assume sign < 0.
       * The other case is symmetric!
       * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
       *
       * Do a clockwise rotation rooted at @parent (A below):
       *
       *           A              B
       *          / \           /   \
       *         B   C?  =>    D     A
       *        / \           / \   / \
       *       D   E?        F?  G?E?  C?
       *      / \
       *     F?  G?
       *
       * Before the rotation:
       *	balance(A) = -2
       *	balance(B) = -1
       * Let x = height(C).  Then:
       *	height(B) = x + 2
       *	height(D) = x + 1
       *	height(E) = x
       *	max(height(F), height(G)) = x.
       *
       * After the rotation:
       *	height(D) = max(height(F), height(G)) + 1
       *		  = x + 1
       *	height(A) = max(height(E), height(C)) + 1
       *		  = max(x, x) + 1 = x + 1
       *	balance(B) = 0
       *	balance(A) = 0
       */
      rotate(p, -sign);

      // Equivalent to setting parent's balance factor to 0.
      p->balance = adjust_balance(p->balance, -sign); // A

      // Equivalent to setting t's balance factor to 0.
      t->balance = adjust_balance(t->balance, -sign); // B
    } else {
      /* t (B below) is heavy in the direction opposite
       * from the direction p (A below) is heavy.
       *
       * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
       * The comment, diagram, and equations below assume sign < 0.
       * The other case is symmetric!
       * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
       *
       * Do a counterblockwise rotation rooted at @node (B below),
       * then a clockwise rotation rooted at @parent (A below):
       *
       *           A             A           E
       *          / \           / \        /   \
       *         B   C?  =>    E   C? =>  B     A
       *        / \           / \        / \   / \
       *       D?  E         B   G?     D?  F?G?  C?
       *          / \       / \
       *         F?  G?    D?  F?
       *
       * Before the rotation:
       *	balance(A) = -2
       *	balance(B) = +1
       * Let x = height(C).  Then:
       *	height(B) = x + 2
       *	height(E) = x + 1
       *	height(D) = x
       *	max(height(F), height(G)) = x
       *
       * After both rotations:
       *	height(A) = max(height(G), height(C)) + 1
       *		  = x + 1
       *	balance(A) = balance(E{orig}) >= 0 ? 0 : -balance(E{orig})
       *	height(B) = max(height(D), height(F)) + 1
       *		  = x + 1
       *	balance(B) = balance(E{orig} <= 0) ? 0 : -balance(E{orig})
       *
       *	height(E) = x + 2
       *	balance(E) = 0
       */
      double_rotate(t, p, -sign);
    }

    // Height after rotation is unchanged; nothing more to do.
    return true;
  }

  /*
   * Template for performing a single rotation ---
   *
   * sign > 0:  Rotate clockwise (right) rooted at A:
   *
   *           P?            P?
   *           |             |
   *           A             B
   *          / \           / \
   *         B   C?  =>    D?  A
   *        / \               / \
   *       D?  E?            E?  C?
   *
   * (nodes marked with ? may not exist)
   *
   * sign < 0:  Rotate counterclockwise (left) rooted at A:
   *
   *           P?            P?
   *           |             |
   *           A             B
   *          / \           / \
   *         C?  B   =>    A   D?
   *            / \       / \
   *           E?  D?    C?  E?
   *
   * This updates pointers but not balance factors!
   */
  void rotate(node *A, signed char sign) {
    node *B = child(A, -sign);
    node *E = child(B, +sign);
    node *P = A->parent;

    child(A, -sign) = E;
    A->parent = B;

    child(B, +sign) = A;
    B->parent = P;

    if (E) E->parent = A;
    replace_child(P, A, B);
  }

  /*
   * Template for performing a double rotation ---
   *
   * sign > 0:  Rotate counterclockwise (left) rooted at B, then
   *		     clockwise (right) rooted at A:
   *
   *           P?            P?          P?
   *           |             |           |
   *           A             A           E
   *          / \           / \        /   \
   *         B   C?  =>    E   C? =>  B     A
   *        / \           / \        / \   / \
   *       D?  E         B   G?     D?  F?G?  C?
   *          / \       / \
   *         F?  G?    D?  F?
   *
   * (nodes marked with ? may not exist)
   *
   * sign < 0:  Rotate clockwise (right) rooted at B, then
   *		     counterclockwise (left) rooted at A:
   *
   *         P?          P?              P?
   *         |           |               |
   *         A           A               E
   *        / \         / \            /   \
   *       C?  B   =>  C?  E    =>    A     B
   *          / \         / \        / \   / \
   *         E   D?      G?  B      C?  G?F?  D?
   *        / \             / \
   *       G?  F?          F?  D?
   *
   * Returns a pointer to E and updates balance factors.  Except for those
   * two things, this function is equivalent to:
   *	avl_rotate(root_ptr, B, -sign);
   *	avl_rotate(root_ptr, A, +sign);
   *
   * See comment in handle_subtree_growth() for explanation of balance
   * factor updates.
   */
  node *double_rotate(node *B, node *A, signed char sign) {
    node *E = child(B, +sign);
    node *F = child(E, -sign);
    node *G = child(E, +sign);
    node *P = A->parent;
    balance_type e = E->balance;

    child(A, -sign) = G;
    A->parent = E;
    A->balance = sign * e >= 0 ? EH : negate_balance(e);

    child(B, +sign) = F;
    B->parent = E;
    B->balance = sign * e <= 0 ? EH : negate_balance(e);

    child(E, +sign) = A;
    child(E, -sign) = B;
    E->parent = P;
    E->balance = EH;

    if (G != nullptr) G->parent = A;
    if (F != nullptr) F->parent = B;
    replace_child(P, A, E);
    return E;
  }

private:
  // Implementation of iterators for in-order tree traversal.
  class TreeIteratorImpl : public Iterator<T>::Impl {
  private:
    typedef typename Iterator<T>::Impl Impl;

  public:
    Impl *clone() const override { return new TreeIteratorImpl(ptr); }
    T &access() const override { return ptr->data; }
    void advance() override {
      if (this->ptr == nullptr) return;
      if (this->ptr->right != nullptr)
        this->ptr = leftdown(this->ptr->right);
      else
        this->ptr = leftup(this->ptr);
    }
    bool equal(const Impl &i) const override {
      return ptr == ((TreeIteratorImpl *)&i)->ptr;
    }

    TreeIteratorImpl(node *p) : ptr(p) {}

  protected:
    node *ptr;
    friend class avltree<T>;
  };

public:
  Iterator<T> begin() override {
    return Iterator<T>(new TreeIteratorImpl(leftdown(root)));
  }
  Iterator<T> end() override {
    return Iterator<T>(new TreeIteratorImpl(nullptr));
  }

  // Searches the tree for key x.  If found, it returns an iterator
  // pointing to it, otherwise it returns end().
  Iterator<T> lookup(const T &x) {
    return Iterator<T>(new TreeIteratorImpl(lookup(root, x)));
  }

private:
  // Searches the subtree pointed to by t for key x.  If found, it
  // returns the node, otherwise, it returns nullptr.
  static node *lookup(node *t, const T &x) {
    while (t != nullptr)
      if (x < t->data)
        t = t->left;
      else if (x > t->data)
        t = t->right;
      else
        break;
    return t;
  }

public:
  // Removes key x from the tree, if it exists, and returns true.
  // If it does not exist, it does nothing and returns false.
  bool remove(const T& x) {
    node *t = lookup(root, x);
    if (t == nullptr) return false;
    remove(t);
    delete t;
    --the_size;
    return true;
  }

  // Removes the element pointed to by iterator i.
  void remove(Iterator<T> i) {
    node *t = dynamic_cast<const TreeIteratorImpl *>(i.getImpl())->ptr;
    remove(t);
    delete t;
    --the_size;
  }

private:
  // Removes the node pointed to by t.
  void remove(node *t) {
  	node *p;
  	bool left_deleted = false;

  	if (t->left != nullptr && t->right != nullptr) {
  		/* node is fully internal, with two children.  Swap it
  		 * with its in-order successor (which must exist in the
  		 * right subtree of node and can have, at most, a right
  		 * child), then unlink node.  */
  		p = swap_with_successor(t, left_deleted);
  		/* p now points to the parent of what was node's in-order
  		 * successor.  It cannot be nullptr, since the node itself was
  		 * an ancestor of its in-order successor.
  		 * left_deleted has been set to true if the node's
  		 * in-order successor was the left child of parent,
  		 * otherwise false.  */
  	} else {
  		/* node is missing at least one child.  Unlink it.  Set
  		 * parent to node's parent, and set left_deleted to
  		 * reflect which child of parent node was.  Or, if
  		 * node was the root node, simply update the root node
  		 * and return.  */
  		node *child = t->left != nullptr ? t->left : t->right;
  		p = t->parent;
  		if (p != nullptr) {
  			if (t == p->left) {
  				p->left = child;
  				left_deleted = true;
  			} else {
  				p->right = child;
  				left_deleted = false;
  			}
  			if (child != nullptr) child->parent = p;
  		} else {
  			if (child != nullptr) child->parent = p;
  			root = child;
  			return;
  		}
  	}

  	// Rebalance the tree.
  	do {
			p = handle_subtree_shrink(p, left_deleted ? +1 : -1, left_deleted);
  	} while (p != nullptr);
  }

  /* Swaps node X, which must have 2 children, with its in-order successor, then
   * unlinks node X.  Returns the parent of X just before unlinking, without its
   * balance factor having been updated to account for the unlink.  */
  node *swap_with_successor(node *X, bool &left_deleted_ret) {
  	node *Y = X->right, *ret;
  	if (Y->left == nullptr) {
  		/*
  		 *     P?           P?           P?
  		 *     |            |            |
  		 *     X            Y            Y
  		 *    / \          / \          / \
  		 *   A   Y    =>  A   X    =>  A   B?
  		 *      / \          / \
  		 *    (0)  B?      (0)  B?
  		 *
  		 * [ X unlinked, Y returned ]
  		 */
  		ret = Y;
  		left_deleted_ret = false;
  	} else {
  		node *Q;
  		do {
  			Q = Y;
  			Y = Y->left;
  		} while (Y->left != nullptr);

  		/*
  		 *     P?           P?           P?
  		 *     |            |            |
  		 *     X            Y            Y
  		 *    / \          / \          / \
  		 *   A   ...  =>  A  ...   =>  A  ...
  		 *       |            |            |
  		 *       Q            Q            Q
  		 *      /            /            /
  		 *     Y            X            B?
  		 *    / \          / \
  		 *  (0)  B?      (0)  B?
  		 *
  		 *
  		 * [ X unlinked, Q returned ]
  		 */

  		Q->left = Y->right;
  		if (Q->left != nullptr) Q->left->parent = Q;
  		Y->right = X->right;
  		X->right->parent = Y;
  		ret = Q;
  		left_deleted_ret = true;
  	}

  	Y->left = X->left;
  	X->left->parent = Y;

    Y->balance = X->balance;
  	Y->parent = X->parent;
  	replace_child(X->parent, X, Y);
  	return ret;
  }

  /*
   * This function handles the shrinkage of a subtree due to a deletion.
   *
   * p
   *	A node in the tree, exactly one of whose subtrees has decreased
   *	in height by 1 due to a deletion.  (This includes the case where
   *	one of the child pointers has become nullptr, since we can consider
   *	the "nullptr" subtree to have a height of 0.)
   *
   * sign
   *	+1 if the left subtree of p has decreased in height by 1;
   *	-1 if the right subtree of p has decreased in height by 1.
   *
   * left_deleted_ret
   *	If the return value is not nullptr, this will be set to true if the
   *	left subtree of the returned node has decreased in height by 1,
   *	or false if the right subtree of the returned node has decreased
   *	in height by 1.
   *
   * This function will adjust p's balance factor, then do a (single
   * or double) rotation if necessary.  The return value will be nullptr if
   * the full AVL tree is now adequately balanced, or a pointer to the
   * parent of p if p is now adequately balanced but has decreased in
   * height by 1.  Also in the latter case, left_deleted_ret will be set.
   */
  node *handle_subtree_shrink(node *p, signed char sign,
                              bool &left_deleted_ret) {
  	balance_type old_balance_factor = p->balance;
    balance_type new_balance_factor = adjust_balance(old_balance_factor, sign);
    node *t;

  	if (old_balance_factor == EH) {
  		/* Prior to the deletion, the subtree rooted at
  		 * p was perfectly balanced.  It's now
  		 * unbalanced by 1, but that's okay and its height
  		 * hasn't changed.  Nothing more to do.  */
  		p->balance = new_balance_factor;
  		return nullptr;
  	}

  	if (new_balance_factor == EH) {
  		/* The subtree rooted at p is now perfectly
  		 * balanced, whereas before the deletion it was
  		 * unbalanced by 1.  Its height must have decreased
  		 * by 1.  No rotation is needed at this location,
  		 * but continue up the tree.  */
  		p->balance = new_balance_factor;
  		t = p;
  	} else {
  		/* p is too left-heavy (new_balance_factor == -2) or
  		 * too right-heavy (new_balance_factor == +2).  */

  		t = child(p, sign);

  		/* The rotations below are similar to those done during
  		 * insertion (see handle_subtree_growth()), so full
  		 * comments are not provided.  The only new case is the
  		 * one where t has a balance factor of EH, and that is
  		 * commented.  */

  		if (sign * t->balance >= 0) {

  			rotate(p, -sign);

  			if (t->balance == EH) {
  				/*
  				 * t (B below) is perfectly balanced.
  				 *
  				 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  				 * The comment, diagram, and equations
  				 * below assume sign < 0.  The other case
  				 * is symmetric!
  				 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  				 *
  				 * Do a clockwise rotation rooted at p (A below):
  				 *
  				 *           A              B
  				 *          / \           /   \
  				 *         B   C?  =>    D     A
  				 *        / \           / \   / \
  				 *       D   E         F?  G?E   C?
  				 *      / \
  				 *     F?  G?
  				 *
  				 * Before the rotation:
  				 *	balance(A) = -2
  				 *	balance(B) =  0
  				 * Let x = height(C).  Then:
  				 *	height(B) = x + 2
  				 *	height(D) = x + 1
  				 *	height(E) = x + 1
  				 *	max(height(F), height(G)) = x.
  				 *
  				 * After the rotation:
  				 *	height(D) = max(height(F), height(G)) + 1
  				 *		  = x + 1
  				 *	height(A) = max(height(E), height(C)) + 1
  				 *		  = max(x + 1, x) + 1 = x + 2
  				 *	balance(A) = -1
  				 *	balance(B) = +1
  				 */

  				/* A: -2 => -1 (sign < 0)
  				 * or +2 => +1 (sign > 0)
  				 * No change needed --- that's the same as
  				 * old_balance_factor.  */

  				/* B: 0 => +1 (sign < 0)
  				 * or 0 => -1 (sign > 0)  */
  				t->balance = adjust_balance(t->balance, -sign);

  				/* Height is unchanged; nothing more to do.  */
  				return nullptr;
  			} else {
  				p->balance = adjust_balance(p->balance, -sign);
  				t->balance = adjust_balance(t->balance, -sign);
  			}
  		} else {
  			t = double_rotate(t, p, -sign);
  		}
  	}
  	p = t->parent;
  	if (p != nullptr) left_deleted_ret = (t == p->left);
  	return p;
  }
};

#endif
