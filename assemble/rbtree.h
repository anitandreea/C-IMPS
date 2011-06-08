typedef struct treeNode {
	    void* key;
		void* value;
    	struct treeNode* left;
	    struct treeNode* right;
		struct treeNode* parent;
		unsigned char color;
} *rbTreeNode;

rbTreeNode grandparent(rbTreeNode n);

rbTreeNode sibling(rbTreeNode n);

rbTreeNode uncle(rbTreeNode n);

rbTreeNode rbTreeInit();

rbTreeNode rbCreateNode(void* key, void* value, unsigned char colour, rbTreeNode left, rbTreeNode right);

typedef int (*compareFunc)(void* left, void* right);

void* rbLookup(rbTreeNode node, void* key, compareFunc compare);

void rbInsert(rbTreeNode node, void* key, void* value, compareFunc compare);

void rbDelete(rbTreeNode node, void* key, compareFunc compare);
