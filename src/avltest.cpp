#include <iostream>
#include <limits>
#include <string>

#include "avltree.hpp"

using namespace std;

// Simple driver program for testing the AVL tree implementation.
// It starts with an empty tree and reads from stdin commands of the form:
//
//   i key   insert key
//   l key   prints "Y" if key was found, "N" if it was not
//   r key   removes key, if it exists
//   s       prints the tree size, i.e., number of nodes
//   c       clears the tree, i.e., removes all of its nodes
//   p       prints the tree's elements using in-order traversal
//   s       prints the result of the AVL sanity check
// All keys are integer numbers.

int main() {
	avltree<int> t;
	char op;
	while (cin >> op) {
		switch (op) {
		case 'i': {
			int key;
			cin >> key;
			t.insert(key);
			break;
		}
		case 'l': {
			int key;
			cin >> key;
			auto i = t.lookup(key);
			cout << (i == t.end() ? "N" : "Y") << endl;
			break;
		}
		case 'r': {
			int key;
			cin >> key;
			t.remove(key);
			break;
		}
		case 's': {
			cout << t.size() << endl;
			break;
		}
		case 'c': {
			t.clear();
			break;
		}
		case 'p': {
			bool sep = false;
			for (int x : t) {
				cout << (sep ? " " : "") << x;
				sep = true;
			}
			cout << endl;
			break;
		}
		case 's': {
			bool sanity = t.sanity();
			if (sanity) cout << "passed sanity check" << endl;
			else cout << "failed sanity check" << endl;
		}
		default: {
			cerr << "Unknown operation: " << op << endl;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		}
	}
	

}
