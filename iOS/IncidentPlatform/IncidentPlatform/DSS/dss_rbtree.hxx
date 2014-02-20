#ifndef _DSS_RBTREE_H
#define _DSS_RBTREE_H
#pragma once

//#include "RESULT.h"
#include "EHM.h"

namespace dss {

// Red Black Template Tree
// This is an efficient binary tree that is also self balancing
// designed to be used as a template based class.  This data
// structure can be very useful for a number of different purposes
// notably for associative arrays and search

typedef enum NodeColor
{
	RED_NODE,
	BLACK_NODE
} NODE_COLOR;

template <class T>
class RBTreeNode
{
public:
	RBTreeNode(T data = NULL, NodeColor c =  BLACK_NODE, RBTreeNode* parent = NULL) :
	  m_color(c),
	  //m_left(NULL),
	  //m_right(NULL),
	  m_data(data),
      m_parent(parent)
	{
		if(m_data != NULL)
        {
            m_left = new RBTreeNode<T>(NULL, BLACK_NODE, this);
            m_right = new RBTreeNode<T>(NULL, BLACK_NODE, this);
        }
        else
        {
            m_left = NULL;
            m_right = NULL;
        }
        
	}

    ~RBTreeNode()
    {
        if(m_data != NULL)
            delete m_data;
        if(m_left != NULL)
            delete m_left;
        if(m_right != NULL)
            delete m_right;
    }

    RESULT InsertChild(T data)
    {
        if(m_data != NULL)
        {
            printf("Error! Adding data to non-null leaf node\n");
            return R_ERROR;
        }
        else if(m_color != BLACK_NODE)
        {
            printf("Error! Adding data to a non-black leaf node!\n");
            return R_ERROR;
        }
        else
        {
            m_color = RED_NODE;
            m_data = data;
            // Add black leaf nodes with this as parent
            m_left = new RBTreeNode<T>(NULL, BLACK_NODE, this);
            m_right = new RBTreeNode<T>(NULL, BLACK_NODE, this);
        }

        return R_OK;
    }

	RBTreeNode*&left(){ return m_left; }
	RBTreeNode*&right(){ return m_right; }
	T &NodeData(){ return m_data; }

	RESULT SetColor(NodeColor c){ m_color = c; return R_OK; }
	NodeColor GetColor(){ return m_color; }

public:
	RBTreeNode  *m_left;
	RBTreeNode *m_right;
    RBTreeNode *m_parent;
	T m_data;
	NodeColor m_color;
};

bool CompareTwoIntPointers(int *a, int *b)
{
    return (*a > *b) ? true : false;
}

RESULT PrintIntPointer(int *a)
{
    printf("%d\n", *a);
    return R_OK;
}

template <class T>
class RBTree
{
public:
    typedef bool (*CompareDataFunction)(T, T);
    typedef RESULT (*PrintDataFunction)(T);

    RBTree(T data = NULL,
           CompareDataFunction cf = CompareTwoIntPointers, 
           PrintDataFunction pf = PrintIntPointer) :
        m_ComparisonFunction(cf),
        m_PrintFunction(pf),
        m_Length(0)
	{
		//m_root = new RBTreeNode<T>(data);
        m_root = new RBTreeNode<T>();
        if(data != NULL) Insert(data);
	}
    

private:
    RESULT SetNodeSubtreeRoot(RBTreeNode<T>*parent, RBTreeNode<T>*NewRoot)
    {
        // reassign parent of subtree to correct root
        // if rotating about root there is no parent so
        // be careful
        if(parent == m_root)
        {            
            m_root = NewRoot;
        }
        else if(parent->m_parent->m_left == parent)
        {
            parent->m_parent->m_left = NewRoot;
        }
        else if(parent->m_parent->m_right == parent)
        {
            parent->m_parent->m_right = NewRoot;
        }

        // reassign parents
        NewRoot->m_parent = parent->m_parent;
        parent->m_parent = NewRoot;

        return R_OK;
    }

    RESULT RotateSubTreeLeft(RBTreeNode<T>*node)
    {
        RBTreeNode<T>*root = node->m_right;
        SetNodeSubtreeRoot(node, root);

        // pass left child of new root to current root
        node->m_right = root->m_left;
        root->m_left = node;
        node->m_right->m_parent = node;

        return R_OK;
    }

    RESULT RotateSubTreeRight(RBTreeNode<T>*node)
    {
        RBTreeNode<T>*root = node->m_left;

        SetNodeSubtreeRoot(node, root);

        // pass left child of new root to current root
        node->m_left = root->m_right;
        root->m_right = node;
        node->m_left->m_parent = node;        

        return R_OK;
    }

    RESULT SwapNodeColors(RBTreeNode<T>*a, RBTreeNode<T>*b)
    {
        NodeColor TempColor = a->GetColor();
        a->SetColor(b->GetColor());
        b->SetColor(TempColor);

        return R_OK;
    }

    // Find node by contained data
    RBTreeNode<T>*find(RBTreeNode<T>* node, T data)
    {
        RBTreeNode<T>*temp = NULL;
        
        if(node == NULL)
            return NULL;
        
        if((temp = find(node->m_left, data)) != NULL)
            return temp;

        if(node->m_data == data)
            return node;

        if((temp = find(node->m_right, data)) != NULL)
            return temp;
    }

    RBTreeNode<T>* GetNodeSibling(RBTreeNode<T>*node)
    {
        if(node == node->m_parent->m_left)
            return node->m_parent->m_right;
        else
            return node->m_parent->m_left;
    }

    // Just swaps the data in the nodes
    RESULT SwapNodes(RBTreeNode<T>*&a, RBTreeNode<T>*&b)
    {
        // Swap Data
        T TempData = a->m_data;
        a->m_data = b->m_data;
        b->m_data = TempData;

        // Swap colors
        NodeColor TempColor = a->GetColor();
        a->m_color = b->GetColor();
        b->m_color = TempColor;

        // This will swap the pointers put into the 
        // function call and should not affect the
        // actual tree structure
        RBTreeNode<T>*TempNode = a;
        a = b;
        b = TempNode;

        return R_OK;
    }

    /*RESULT DeleteNode(RBTreeNode<T>*node)
    {
        if(node->m_left )
        
        return R_OK;
    }*/

public:

    // Standard BST removal
    // Need to adjust this to deal with RB algorithm
    RBTreeNode<T>*remove(RBTreeNode<T>*node)
    {
        if(node == NULL)
        {
            return NULL;
        }

        if(node->m_left->m_data == NULL)
        {     
            // Now we have a node with at most one child 
            RESULT r = RBRemove(node);
        }
        else if(node->m_right->m_data == NULL)
        {
            // Now we have a node with at most one child 
            RESULT r = RBRemove(node);
        }
        else
        {
            // swap data and then remove the node with the data to be removed
            T TempData = node->m_data;            
            node->m_data = node->m_right->m_data;
            node->m_right->m_data = TempData;

            node->m_right = remove(node->m_right);
        }

        return node;
    }

    RESULT RBRemoveAlgorithm(RBTreeNode<T>*node)
    {
        RESULT r = R_OK;
        
        if(node->m_parent == NULL)
        {
            // Case 1: If child has no parent we have just removed the root
            // and we are ok to move on if not we proceed to other cases
            return r;
        } 
        else if(GetNodeSibling(node)->GetColor() == RED_NODE)
        {
            // Case 2: If the sibling is red then we reverse color of parent and sibling and rotate the tree 
            // around sibling to make the sibling the grandparent
            node->m_parent->SetColor(RED_NODE);
            GetNodeSibling(node)->SetColor(BLACK_NODE);
            if(node == node->m_parent->m_left)
                RotateSubTreeLeft(node->m_parent);
            else
                RotateSubTreeRight(node->m_parent);
        }                
        else if(node->m_parent->GetColor() == BLACK_NODE        && 
                GetNodeSibling(node)->GetColor() == BLACK_NODE  &&
                GetNodeSibling(node)->m_left->GetColor() == BLACK_NODE &&
                GetNodeSibling(node)->m_right->GetColor() == BLACK_NODE)
        {
            // Case 3: If parent, sibling and both sibling children are black
            // so we can simply repaint the sibling RED and then rebalance 
            // on Parent
            GetNodeSibling(node)->SetColor(RED_NODE);
            RBRemoveAlgorithm(node->m_parent);
        }
        else if(node->m_parent->GetColor() == RED_NODE &&
                GetNodeSibling(node)->GetColor() == BLACK_NODE  &&
                GetNodeSibling(node)->m_left->GetColor() == BLACK_NODE &&
                GetNodeSibling(node)->m_right->GetColor() == BLACK_NODE)
        {
            // Case 4: parent is RED but sibling and sibling children are black
            // we exchange the colors of sibling and parent
            GetNodeSibling(node)->SetColor(RED_NODE);
            node->m_parent->SetColor(BLACK_NODE);
        }
        else //if(GetNodeSibling(node)->GetColor() == BLACK_NODE)  Case 5 and 6 are the same
        {
            // Case 5: Sibling is black and one of the sibling children are red and other is black
            if((node == node->m_parent->m_left) &&
               (GetNodeSibling(node)->m_right->GetColor() == BLACK_NODE) &&
               (GetNodeSibling(node)->m_left->GetColor() == RED_NODE))
            {
                GetNodeSibling(node)->SetColor(RED_NODE);
                GetNodeSibling(node)->m_left->SetColor(BLACK_NODE);
                RotateSubTreeRight(GetNodeSibling(node));
            }
            else if((node == node->m_parent->m_right) &&
                    (GetNodeSibling(node)->m_left->GetColor() == BLACK_NODE) &&
                    (GetNodeSibling(node)->m_right->GetColor() == RED_NODE))
            {
                GetNodeSibling(node)->SetColor(RED_NODE);
                GetNodeSibling(node)->m_right->SetColor(BLACK_NODE);
                RotateSubTreeLeft(GetNodeSibling(node));
            }

            GetNodeSibling(node)->SetColor(node->m_parent->GetColor());
            node->m_parent->SetColor(BLACK_NODE);

            if(node == node->m_parent->m_left)
            {
                GetNodeSibling(node)->m_right->SetColor(BLACK_NODE);
                RotateSubTreeLeft(node->m_parent);
            }
            else
            {
                GetNodeSibling(node)->m_left->SetColor(BLACK_NODE);
                RotateSubTreeRight(node->m_parent);
            }
            
        }
Error:
        return r;
    }

    RESULT RBRemove(RBTreeNode<T>*node)
    {
        RESULT r = R_OK;

        RBTreeNode<T>*child = NULL;
        if(node->m_left->m_data != NULL)
            child = node->m_left;
        else
            child = node->m_right;

        SwapNodes(node, child);

        if(node->GetColor() == BLACK_NODE)
        {
            if(child->GetColor() == RED_NODE)
            {
                // Simple case set the child node color to black and delete the node
                child->SetColor(BLACK_NODE);
            }
            else
            {
                r = RBRemoveAlgorithm(child);
            }
            
        }

        if(node == node->m_parent->m_left)
            node->m_parent->m_left = new RBTreeNode<T>(NULL, BLACK_NODE, node->m_parent);
        else
            node->m_parent->m_right = new RBTreeNode<T>(NULL, BLACK_NODE, node->m_parent);;

        delete node;
        m_Length--;
        
Error:
        return r;
    }

public:
    RBTreeNode<T>*find(T data)
    {
        return find(m_root, data);
    }

private:
    RESULT RedBlackAlgorithm(RBTreeNode<T>*node)
    {
        RESULT r = R_OK;

        RBTreeNode<T>*grandparent = NULL;
        RBTreeNode<T>*uncle = NULL;
        RBTreeNode<T>*parent = NULL;

        if(node->m_parent != NULL)
            parent = node->m_parent;
        if(parent != NULL && parent->m_parent != NULL)
            grandparent = parent->m_parent;
        if(grandparent != NULL)
        {
            if(parent == grandparent->m_left)
            {
                if(grandparent->m_right != NULL)
                    uncle = grandparent->m_right;
            }
            else //if(parent == grandparent->m_right)
            {
                if(grandparent->m_left != NULL)
                    uncle = grandparent->m_left;
            }
        }

        if(node->m_parent == NULL)
        {
            // Case 1: node is the root of the tree
            node->SetColor(BLACK_NODE);
            return R_OK;
        }
        else if(node->m_parent->GetColor() == BLACK_NODE)
        {
            // case 2: still valid tree
            return R_OK;
        }
        else if(uncle->GetColor() == RED_NODE)
        {
            // case 3: both parent and uncle are red
            // repaint them black and paint grandparent red
            // then recursively call the algorithm on the grandparent
            grandparent->SetColor(RED_NODE);
            parent->SetColor(BLACK_NODE);
            uncle->SetColor(BLACK_NODE);
            RedBlackAlgorithm(grandparent);
        }
        else if(uncle->GetColor() == BLACK_NODE)
        {
            // case 4 & 5: parent is red and uncle is black
            // need to determiine which direction from parent
            // case 4 is a sub case of case 5 since we always
            // will execute 5 so geet 4 ready for 5 before we get there
            if(node == parent->m_right && parent == grandparent->m_left)
            {
                // case 4 need to rotate "parent tree" left
                // and n becomes n->left
                r = RotateSubTreeLeft(parent);
                node = node->m_left;
            }
            else if(node == parent->m_left && parent == grandparent->m_right)
            {
                // case 4 need to rotate "parent tree" right
                // and n becomes n->right
                r = RotateSubTreeRight(parent);
                node = node->m_right;
            }

            // We may have rotated the tree so all of the relationships may
            // have changed

            if(node->m_parent != NULL)
                parent = node->m_parent;
            if(parent != NULL && parent->m_parent != NULL)
                grandparent = parent->m_parent;
            if(grandparent != NULL)
            {
                if(parent == grandparent->m_left)
                {
                    if(grandparent->m_right != NULL)
                        uncle = grandparent->m_right;
                }
                else //if(parent == grandparent->m_right)
                {
                    if(grandparent->m_left != NULL)
                        uncle = grandparent->m_left;
                }
            }

            parent->SetColor(BLACK_NODE);
            grandparent->SetColor(RED_NODE);

            // Now the tree should look like case 5
            if(node == parent->m_left && parent == grandparent->m_left)
            {
                r = RotateSubTreeRight(grandparent);
                //SwapNodeColors(grandparent, parent);
            }
            else //if(node == parent->m_right && parent == grandparent->m_right)
            {
                r = RotateSubTreeLeft(grandparent);
                //SwapNodeColors(grandparent, parent);
            }
        }

Error:
        return r;
    }

    // This function is very much a plain binary tree insert function
    // After the node is inserted the Red Black algorithm is run on the inserted node
    RESULT Insert(T data, RBTreeNode<T>*node)
    {
        RESULT r = R_OK;

        if(node->NodeData() == NULL)
        {
            // We found a good spot here
            // Parent and Grandparent are stored in parent/grandparent
            // So we can do red-black balancing at this point
            node->InsertChild(data);

            m_Length++;

            if((r = RedBlackAlgorithm(node)) != R_OK)
            {
                r = R_FAIL;
            }

            return r;
        }
        else if(m_ComparisonFunction(data, node->NodeData()))
        {
            return Insert(data, node->right());
        }
        else
        {
            return Insert(data, node->left());
        }

Error:
        return r;
    }



    // Print will also analyze the tree to ensure that the tree conforms
    // to the red-black rules
    RESULT Print(RBTreeNode<T>*node, PrintDataFunction PrintFunction, int depth)
    {
        RESULT r = R_OK;
        
        if(node->m_left != NULL)
        {
            if((r = Print(node->m_left, PrintFunction, depth)) != R_OK)
                return r;
        }
        if(node->m_data != NULL)
        {
            // Analyze tree according to the five rules
            // first rule is implied by using the typedef enum for node color
            // Rule #2: root node is black
            if(m_root == node)
            {
                if(node->GetColor() != BLACK_NODE)
                    printf("RBTree Error: Root node is not black\n");;
            }

            // Rule #4: Both children of red nodes are black
            if(node->GetColor() == RED_NODE)
            {
                if(node->m_left->GetColor() != BLACK_NODE || 
                   node->m_right->GetColor() != BLACK_NODE )
                {
                    printf("RBTree Error: Red node has a non-black child\n");
                }
            }
            
            printf("%s", (node->m_color == RED_NODE) ? "r:" : "b:");

            if((r = PrintFunction(node->m_data)) != R_OK)
                return r;
        }
        else
        {
            // Rule #3: All leaf nodes are black (null data nodes)
            if(node->GetColor() != BLACK_NODE)
            {
                printf("RBTree Error: Null data leaf node is not black\n");
            }

            // Rule #5: Every simple path from a given node 

        }
        if(node->m_right != NULL)
        {
            if((r = Print(node->m_right, PrintFunction, depth)) != R_OK)
                return r;
        }

Error:
        return r;
    }

public:
    int m_BlackDepth;

    int DepthBlackNodes(RBTreeNode<T>*node, RBTreeNode<T>*RootNode)
    {
        if(node->m_data == NULL)
        {
            // Follow the parents up to see how many black nodes
            RBTreeNode<T>*TempNode = node;
            int i = 0;
            int j = 0;
            while(TempNode != RootNode)
            {
                if(TempNode->GetColor() == BLACK_NODE) i++;
                TempNode = TempNode->m_parent;
                j++;
            }
            if(TempNode->GetColor() == BLACK_NODE) i++;
            j++;

            if(m_BlackDepth == -1)
            {
                printf("node black depth:%d\n", i);
                m_BlackDepth = i;
            }
            else if( i != m_BlackDepth )
            {
                printf("ERROR: Black depth differs on a branch\n");
            }

            return 0;
        }

        int LeftDepth = DepthBlackNodes(node->m_left, RootNode);
        int RightDepth = DepthBlackNodes(node->m_right, RootNode);

        if(LeftDepth > RightDepth)
            return LeftDepth + ((node->GetColor() == BLACK_NODE) ? 1 : 0);
        else
            return RightDepth + ((node->GetColor() == BLACK_NODE) ? 1 : 0);
    }

    int Depth(RBTreeNode<T>*node)
    {
        if(node->m_data == NULL)
            return 0;
        
        int LeftDepth = Depth(node->m_left);
        int RightDepth = Depth(node->m_right);
        
        if(LeftDepth > RightDepth)
            return LeftDepth + 1;
        else
            return RightDepth + 1;
    }

    int Depth(){ return Depth(m_root); }
    int depth(){ return Depth(); }
    int GetDepth(){ return Depth(); }

    RESULT Insert(T data)
    {
        RESULT r = R_OK;

        if(m_root == NULL)
        {
            // If this is case 1
            m_root = new RBTreeNode<T>(data, BLACK_NODE, NULL);
        }
        else
        {
            // root node has NULL parent / grandparent
            r = Insert(data, m_root);        
        }

Error:
        return r;
    }

    RESULT Print()
    {
        RESULT r = R_OK;

        printf("Printing Red-Black Tree\n");
        r = Print(m_root, m_PrintFunction, -1);

        printf("Tree Depth:%d Length:%d\n", depth(), length());
        
        m_BlackDepth = -1;
        DepthBlackNodes(m_root, m_root);

Error:
        return r;
    }

    int length(){ return m_Length; }

private:
	RBTreeNode<T>*m_root;
    CompareDataFunction m_ComparisonFunction;
    PrintDataFunction m_PrintFunction;
    int m_Length;
};

} // namespace dss

#endif // _DSS_RBTREE_H
