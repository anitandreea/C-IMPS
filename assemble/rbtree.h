typedef struct rbTreeNode {
	    void* key;
		void* value;
    	struct rbTreeNode* left;
	    struct rbTreeNode* right;
		struct rbTreeNode* parent;
		unsigned char color;
} *rbTreeNode;

typedef struct assemblyInstruction {

} *aInstruction;

rbTreeNode grandparent(rbTreeNode n);

rbTreeNode sibling(rbTreeNode n);

rbTreeNode uncle(rbTreeNode n);

rbTreeNode rbTreeInit();

rbTreeNode CreateNode(void* key, void* value, unsigned char colour, rbTreeNode left, rbTreeNode right);

typedef int (*compareFunc)(void* left, void* right);

void* rbLookup(rbTreeNode node, void* key, compareFunc compare);

void rbInsert(rbTreeNode node, void* key, void* value, compareFunc compare);

void rbDelete(rbTreeNode node, void* key, compareFunc compare);
