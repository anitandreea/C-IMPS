#include "rbtree.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

rbTreeNode grandparent(rbTreeNode node) {
	assert (node != NULL);
	assert (node->parent != NULL); /* Not the root rbTreeNode */
	assert (node->parent->parent != NULL); /* Not child of root */
	return node->parent->parent;
}

rbTreeNode sibling(rbTreeNode node) {
	assert (node != NULL);
	assert (node->parent != NULL); /* Root rbTreeNode has no sibling */
	if (node == node->parent->left)
		return node->parent->right;
	else
		return node->parent->left;
}

rbTreeNode uncle(rbTreeNode node) {
	assert (node != NULL);
	assert (node->parent != NULL); /* Root rbTreeNode has no uncle */
	assert (node->parent->parent != NULL); /* Children of root have no uncle */
	return sibling(node->parent);
}

unsigned char nodeColor(rbTreeNode node) {
	    return node == NULL ? 0 : node->color;
}

rbTree rbTreeCreate() {
	rbTree t = (rbTree)malloc(sizeof(struct tree));
	t->root = NULL;
	return t;
}

rbTreeNode rbCreateNode(void* key, void* value, unsigned char color, rbTreeNode left, rbTreeNode right) {
	rbTreeNode result = (rbTreeNode)malloc(sizeof(struct treeNode));
	result->key = key;
	result->value = value;
	result->color = color;
	result->left = left;
	result->right = right;
	if (left  != NULL)  left->parent = result;
	if (right != NULL) right->parent = result;
	result->parent = NULL;
	return result;
}

rbTreeNode rbLookupNode(rbTree tree, void* key, compareFunc compare) {
	rbTreeNode node = tree->root;
	while (node != NULL) {
		int comparisonResult = compare(key, node->key);
		if (comparisonResult == 0) {
			return node;
		} else if (comparisonResult < 0) {
			node = node->left;
		} else {
			assert(comparisonResult > 0);
			node = node->right;
		}
	}
	return node;
}

void* rbLookup(rbTree tree, void* key, compareFunc compare) {
	rbTreeNode node = rbLookupNode(tree, key, compare);
	return node == NULL ? NULL : node->value;
}

void rbReplaceNode(rbTree tree, rbTreeNode oldNode, rbTreeNode newNode) {
	if (oldNode->parent == NULL) {
		tree->root = newNode;
	} else {
		if (oldNode == oldNode->parent->left)
			oldNode->parent->left = newNode;
		else
			oldNode->parent->right = newNode;
	}
    if (newNode != NULL) {
		newNode->parent = oldNode->parent;
	}
}

void rbRotateLeft(rbTree tree, rbTreeNode node) {
	rbTreeNode right = node->right;
	rbReplaceNode(tree, node, right);
	node->right = right->left;
	if (right->left != NULL) {
		right->left->parent = node;
	}
	right->left = node;
    node->parent = right;
}

void rbRotateRight(rbTree tree, rbTreeNode node) {
	rbTreeNode left = node->left;
    rbReplaceNode(tree, node, left);
	node->left = left->right;
	if (left->right != NULL) {
		left->right->parent = node;
	}
	left->right = node;
	node->parent = left;
}

void insertCase5(rbTree tree, rbTreeNode node) {
	node->parent->color = 0; //BLACK
	grandparent(node)->color = 1; //RED
	if (node == node->parent->left && node->parent == grandparent(node)->left) {
		rbRotateRight(tree, grandparent(node));
	} else {
		assert (node == node->parent->right && node->parent == grandparent(node)->right);
		rbRotateLeft(tree, grandparent(node));
	}
}

void insertCase4(rbTree tree, rbTreeNode node) {
	if (node == node->parent->right && node->parent == grandparent(node)->left) {
		rbRotateLeft(tree, node->parent);
		node = node->left;
	} else if (node == node->parent->left && node->parent == grandparent(node)->right) {
		rbRotateRight(tree, node->parent);
		node = node->right;
	}
	insertCase5(tree, node);
}

void insertCase2(rbTree tree, rbTreeNode node);

void insertCase1(rbTree tree, rbTreeNode node) {
	if (node->parent == NULL)
		node->color = 0; // BLACK
	else
		insertCase2(tree, node);
}

void insertCase3(rbTree tree, rbTreeNode node) {
	if (nodeColor(uncle(node)) == 1) {  //RED
		node->parent->color = 0; //BLACK
		uncle(node)->color = 0; //BLACK
		grandparent(node)->color = 1; //RED
		insertCase1(tree, grandparent(node));
	} else {
		insertCase4(tree, node);
	}
}

void insertCase2(rbTree tree, rbTreeNode node) {
	if (nodeColor(node->parent) == 0) //BLACK
		return; /* Tree is still valid */
	else
		insertCase3(tree, node);
}

void rbInsert(rbTree tree, void* key, void* value, compareFunc compare) {
	rbTreeNode newNode = rbCreateNode(key, value, 1, NULL, NULL);
    if (tree->root == NULL) {
		tree->root = newNode;
	} else {
		rbTreeNode node = tree->root;
		while (1) {
			int comparisonResult = compare(key, node->key);
			if (comparisonResult == 0) {
				node->value = value;
				/* newNode isn't going to be used, don't leak it */
				free (newNode);
				return;
			} else if (comparisonResult < 0) {
				if (node->left == NULL) {
	            	node->left = newNode;
					break;
				} else {
					node = node->left;
				}
            } else {
				assert (comparisonResult > 0);
				if (node->right == NULL) {
					node->right = newNode;
					break;
                } else {
					node = node->right;
				}
			}
		}
		newNode->parent = node;
	}
	insertCase1(tree, newNode);
}

rbTreeNode maximumNode(rbTreeNode node) {
	assert (node != NULL);
	while (node->right != NULL) {
		node = node->right;
	}
	return node;
}

void deleteCase6(rbTree tree, rbTreeNode node) {
	sibling(node)->color = nodeColor(node->parent);
	node->parent->color = 0;
	if (node == node->parent->left) {
		assert (nodeColor(sibling(node)->right) == 1);
		sibling(node)->right->color = 0;
		rbRotateLeft(tree, node->parent);
	} else {
		assert (nodeColor(sibling(node)->left) == 1);
		sibling(node)->left->color = 0;
		rbRotateRight(tree, node->parent);
	}
}

void deleteCase5(rbTree tree, rbTreeNode node) {
	if (node == node->parent->left && nodeColor(sibling(node)) == 0 &&
		nodeColor(sibling(node)->left) == 1 && nodeColor(sibling(node)->right) == 0) {
			sibling(node)->color = 1;
			sibling(node)->left->color = 0;
			rbRotateRight(tree, sibling(node));
	} else if (node == node->parent->right && nodeColor(sibling(node)) == 0 &&
			nodeColor(sibling(node)->right) == 1 && nodeColor(sibling(node)->left) == 0) {
				sibling(node)->color = 1;
				sibling(node)->right->color = 0;
				rbRotateLeft(tree, sibling(node));
	}
	deleteCase6(tree, node);
}

void deleteCase4(rbTree tree, rbTreeNode node) {
	if (nodeColor(node->parent) == 1 && nodeColor(sibling(node)) == 0 &&
		nodeColor(sibling(node)->left) == 0 && nodeColor(sibling(node)->right) == 0) {
 			sibling(node)->color = 1;
 			node->parent->color = 0;
 	} else {
		deleteCase5(tree, node);
	}
}

void deleteCase1(rbTree tree, rbTreeNode node);

void deleteCase3(rbTree tree, rbTreeNode node) {
	if (nodeColor(node->parent) == 0 && nodeColor(sibling(node)) == 0 &&
		nodeColor(sibling(node)->left) == 0 && nodeColor(sibling(node)->right) == 0) { //All BLACK
			sibling(node)->color = 1; //RED
			deleteCase1(tree, node->parent);
	} else {
		deleteCase4(tree, node);
	}
}

void deleteCase2(rbTree tree, rbTreeNode node) {
	if (nodeColor(sibling(node)) == 1) { //RED
		node->parent->color = 1; //RED
        sibling(node)->color = 0; //BLACK
		if (node == node->parent->left)
			rbRotateLeft(tree, node->parent);
		else
			rbRotateRight(tree, node->parent);
	}
	deleteCase3(tree, node);
}

void deleteCase1(rbTree tree, rbTreeNode node) {
	if (node->parent == NULL)
		return;
	else
		deleteCase2(tree, node);
}

void rbDelete(rbTree tree, void* key, compareFunc compare) {
	rbTreeNode child;
	rbTreeNode node = rbLookup(tree, key, compare);
	if (node == NULL) return;  /* Key not found, do nothing */
	if (node->left != NULL && node->right != NULL) {
		/* Copy key/value from predecessor and then delete it instead */
		rbTreeNode pred = maximumNode(node->left);
        node->key   = pred->key;
		node->value = pred->value;
		node = pred;
  	}

    assert(node->left == NULL || node->right == NULL);
	child = node->right == NULL ? node->left  : node->right;
	if (nodeColor(node) == 0) { // BLACK
		node->color = nodeColor(child);
		deleteCase1(tree, node);
	}

	rbReplaceNode(tree, node, child);
	
	if (node->parent == NULL && child != NULL)
		child->color = 0; // BLACK
	free(node);
}


void printTreeHelper(rbTreeNode n, int indent) {
	int i;
	const int identStep = 4;
	if (n == NULL) {
		fputs("<empty tree>", stdout);
		return;
	}

	if (n->right != NULL) {
		printTreeHelper(n->right, indent + identStep);
	}

	for(i=0; i<indent; i++)
		fputs(" ", stdout);

	if (n->color == 0)
		printf("%s\n", (char *)n->key);
	else
		printf("<%s>\n", (char *)n->key);

	if (n->left != NULL) {
		printTreeHelper(n->left, indent + identStep);
	}
}

void printTree(rbTree t) {
	printTreeHelper(t->root, 0);
	puts("");
}
