typedef struct treeNode {
	    void* key;
		void* value;
    	struct treeNode* left;
	    struct treeNode* right;
		struct treeNode* parent;
		unsigned char color;
} *rbTreeNode;

typedef struct tree {
	    rbTreeNode root;
} *rbTree;

rbTreeNode grandparent(rbTreeNode n);

rbTreeNode sibling(rbTreeNode n);

rbTreeNode uncle(rbTreeNode n);

rbTree rbTreeCreate();

rbTreeNode rbCreateNode(void* key, void* value, unsigned char colour, rbTreeNode left, rbTreeNode right);

typedef int (*compareFunc)(void* left, void* right);

void* rbLookup(rbTree tree, void* key, compareFunc compare);

void rbInsert(rbTree tree, void* key, void* value, compareFunc compare);

void rbDelete(rbTree tree, void* key, compareFunc compare);

void printTree(rbTree t);
