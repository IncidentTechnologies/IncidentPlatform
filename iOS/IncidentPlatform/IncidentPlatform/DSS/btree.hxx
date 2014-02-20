// Simple Binary Search Tree Template Implementation

#pragma once

#include "EHM.h"
#include <stdlib.h>

#ifdef _MEM_TRACK
    #include "MemTrack.h"
    #define MEM_TRACK_NEW new(__FILE__, __LINE__)
#else
    #define MEM_TRACK_NEW new
#endif

namespace dss {

typedef enum btree_sort_mode
{
    BTREE_PREORDER,
    BTREE_INORDER,
    BTREE_POSTORDER
} BTREE_SORT_MODE;

// The Node
template <class Type>
class btreenode {
public:
    // Constructor (no null constructor)
    btreenode(const Type &Data) :
        m_data(Data),
        m_left(NULL),
        m_right(NULL)
    { }

    // Destructor
    ~btreenode()
    {
        if(m_left != NULL) m_left = NULL;
        if(m_right != NULL) m_right = NULL;
    }

#ifdef _MEM_TRACK
    // Memory Management
    // **************************************************************
    void* operator new(unsigned long int size, const char *file, int line) {
        void *ptr = (void *)malloc(size);
        AddTrack((DWORD)ptr, size, file, line);
        return ptr;
    }

    void operator delete(void *p)
    {
        RemoveTrack((DWORD)p);
        free(p);
        return;
    }
    // *************************************************************
#endif
    
    // links
    btreenode<Type> *m_left;
    btreenode<Type> *m_right;

    // Data
    Type m_data;
};

template <class Type>
class btree
{
public:
    btree() :
      m_root(NULL),
      m_NodeCount(0)
    { }

    btree(Type Data)
    {
        m_root = MEM_TRACK_NEW btreenode<Type>(Data);
        if(m_root != NULL)
        {
            m_NodeCount = 1;
        }
        else
        {
            m_NodeCount = 0;
        }
    }

    ~btree()
    {
        Clear();
    }


    // Insert
    // *********************************************************************************
    static RESULT Insert(Type Data, btreenode<Type>*node) {
        if(Data <= node->m_data) {
            if(node->m_left == NULL) {
                node->m_left = MEM_TRACK_NEW btreenode<Type>(Data);
                return (node->m_left != NULL) ? R_BTREE_NODE_INSERTED : R_BTREE_NODE_NOTCREATED;
            }
            else
                return Insert(Data, node->m_left);
        }
        else {
            if(node->m_right == NULL) {
                node->m_right = MEM_TRACK_NEW btreenode<Type>(Data);
                return (node->m_right != NULL) ? R_BTREE_NODE_INSERTED : R_BTREE_NODE_NOTCREATED;
            }
            else
                return Insert(Data, node->m_right);
        }
    }
    
    RESULT Insert(Type Data) {
        btreenode<Type>*cur = m_root;
        if(Empty() == R_YES)  {
            m_root = MEM_TRACK_NEW btreenode<Type>(Data);
            if(m_root != NULL)  {
                m_NodeCount++;
                return R_BTREE_NODE_INSERTED;
            }
            else
                return R_BTREE_NODE_NOTCREATED;
        }
        else {
            RESULT res = Insert(Data, cur);
            if(res == R_BTREE_NODE_INSERTED) {
                m_NodeCount++;
                return res;
            }
            else
                return res;
        }
    }

    // *************************************************************************** 

    static RESULT Clear(btreenode<Type>* &node)
    {
        
        if(node->m_left != NULL) 
        {
            Clear(node->m_left);
        }

        if(node->m_right != NULL) 
        {
            Clear(node->m_right);
        }

        delete node;
        node = NULL;

        return R_SUCCESS;
    }
    
    RESULT Clear()
    {
        if(Empty() == R_YES)
        {
            // Tree already empty no need to clear;
            return R_SUCCESS;
        }
        return Clear(m_root);
    }

    static RESULT Delete(Type Data, btreenode<Type>* node, btreenode<Type>* nodeL)
    {
        if(node->m_data == Data)
        {
            if(node->m_left == NULL)
            {
                if(nodeL != NULL)
                {
                    if(nodeL->m_left == node)
                    {
                        nodeL->m_left = node->m_right;
                    }
                    else if(nodeL->m_right == node)
                    {
                        nodeL->m_right = node->m_right;
                    }
                }

                delete node; 
                //m_NodeCount--;
                return R_BTREE_NODE_REMOVED;                
            }
            else if(node->m_right == NULL)
            {
                if(nodeL != NULL)
                {
                    if(nodeL->m_left == node)
                    {
                        nodeL->m_left = node->m_left;
                    }
                    else if(nodeL->m_right == node)
                    {
                        nodeL->m_right = node->m_left;
                    }
                }
                delete node;
                //m_NodeCount--;
                return R_BTREE_NODE_REMOVED;
            }            
            else
            {
                btreenode<Type>*temp = node->m_left;
                btreenode<Type>*tempL = NULL;

                while(temp->m_right != NULL)
                {
                    tempL = temp;
                    temp = temp->m_right;
                }

                node->m_data = temp->m_data;

                if(tempL == NULL)
                {
                    node->m_left = temp->m_left;
                    delete temp;
                    //m_NodeCount--;
                    return R_BTREE_NODE_REMOVED;
                }
                else
                {
                    tempL->m_right = temp->m_left;
                    delete temp;
                    //m_NodeCount--;
                    return R_BTREE_NODE_REMOVED;
                }
            }
        }
        else
        {
            if(node->m_data > Data)
            {
                if(node->m_left != NULL) 
                {
                    return Delete(Data, node->m_left, node);
                }
                else
                {
                    return R_BTREE_NODE_NOTFOUND;
                }
            }
            else
            {
                if(node->m_right != NULL) 
                {
                    return Delete(Data, node->m_right, node);
                }
                else
                {
                    return R_BTREE_NODE_NOTFOUND;
                }
            }
        }
    }

    RESULT Delete(Type Data)
    {
        // First check to see we are not deleting the root node
        if(m_root->m_data == Data)
        {
            if(m_root->m_left == NULL && m_root->m_right == NULL)
            {
                delete m_root;
                m_root = NULL;
                m_NodeCount--;
                return R_BTREE_NODE_REMOVED;
            }
            else if(m_root->m_left == NULL)
            {
                btreenode<Type>* temp = m_root->m_right;
                delete m_root;
                m_root = temp;
                m_NodeCount--;
                return R_BTREE_NODE_REMOVED;
            }
            else if(m_root->m_right == NULL)
            {       
                btreenode<Type>* temp = m_root->m_left;
                delete m_root;
                m_root = temp;
                m_NodeCount--;
                return R_BTREE_NODE_REMOVED;
            }
        }        
        
        if(m_root != NULL)
        {
            RESULT res = Delete(Data, m_root, NULL);
            if(res == R_BTREE_NODE_REMOVED)
            {
                m_NodeCount--;
                return res;
            }
            else
            {
                return res;
            }   
        }
        else
        {
            return R_BTREE_EMPTY;
        }       
    }

    // Find 
    // ****************************************************************************
    static btreenode<Type>* FindNode(Type Data, btreenode<Type>*node)
    {
        if(node->m_data == Data)
        {
            return node;
        }
        return NULL;
    }

    static btreenode<Type>* FindLeft(Type Data, btreenode<Type>*node, BTREE_SORT_MODE bsm)
    {
        if(node->m_left != NULL)
        {
            return Find(Data, node->m_left, bsm);
        }
        return NULL;
    }

    static btreenode<Type>* FindRight(Type Data, btreenode<Type>*node, BTREE_SORT_MODE bsm)
    {
        if(node->m_right != NULL)
        {
            return Find(Data, node->m_right, bsm);
        }
        return NULL;
    }


    static btreenode<Type>* Find(Type Data, btreenode<Type>*node, BTREE_SORT_MODE bsm)
    {       
        btreenode<Type>*temp = NULL;
        switch(bsm)
        {
        /*case BTREE_PREORDER:    if(FindNode(Data, node, pResult) == R_YES) return R_YES;
                                if(FindLeft(Data, node, bsm, pResult) == R_YES) return R_YES;
                                if(FindRight(Data, node, bsm, pResult) == R_YES) return R_YES;
                                break;*/

        case BTREE_INORDER:     temp = FindLeft(Data, node, bsm);
                                if(temp != NULL) return temp;

                                temp = FindNode(Data, node);
                                if(temp != NULL) return temp;                    

                                temp = FindRight(Data, node, bsm);
                                if(temp != NULL) return temp;

                                break;

        /*case BTREE_POSTORDER:   if(FindLeft(Data, node, bsm, pResult) == R_YES) return R_YES;
                                if(FindRight(Data, node, bsm, pResult) == R_YES) return R_YES;
                                if(FindNode(Data, node, pResult) == R_YES) return R_YES;
                                break;*/

        default:                return NULL;
                                break;
        }

        return NULL;
    }   
    
    
    RESULT Find(Type Data, BTREE_SORT_MODE bsm, btreenode<Type>*pResult)
    {
        btreenode<Type>*cur = m_root;
        pResult = Find(Data, cur, bsm, pResult);
        return (pResult != NULL) ? R_YES : R_NO ;
    }

    // ****************************************************************************

    // Iterate a Function Through the Tree
    // ********************************************************************************
    // Pass as Type: RESULT Function(Type)
    static RESULT Iterate(btreenode<Type>*node, RESULT (*F)(Type, void*), void* Arg)
    {
        if(node->m_left != NULL) Iterate(node->m_left, F, Arg);
        if(node != NULL) F(node->m_data, Arg);
        if(node->m_right != NULL) Iterate(node->m_right, F, Arg);
        return R_SUCCESS;
    }    

    RESULT Iterate(RESULT (*F)(Type, void*), void* Arg)
    {
        if(m_root != NULL)
        {
            if(m_root->m_left != NULL) Iterate(m_root->m_left, F, Arg);
            if(m_root != NULL) F(m_root->m_data, Arg);
            if(m_root->m_right != NULL) Iterate(m_root->m_right, F, Arg);
            return R_SUCCESS;
        }
        else
        {
            return R_BTREE_EMPTY;
        }
        
    }

    // Pass by reference: RESULT Function(Type &)
    static RESULT Iterate(btreenode<Type>*node, RESULT (*F)(Type &))
    {
        if(node->m_left != NULL) Iterate(node->m_left, F);
        if(node != NULL) F(node->m_data);
        if(node->m_right != NULL) Iterate(node->m_right, F);
        return R_SUCCESS;
    }    

    RESULT Iterate(RESULT (*F)(Type &))
    {
        if(m_root != NULL)
        {
            if(m_root->m_left != NULL) Iterate(m_root->m_left, F);
            if(m_root != NULL) F(m_root->m_data);
            if(m_root->m_right != NULL) Iterate(m_root->m_right, F);
            return R_SUCCESS;
        }
        else
        {
            return R_BTREE_EMPTY;
        }
    }

    // Pass normally: RESULT Function(Type)
    static RESULT Iterate(btreenode<Type>*node, RESULT (*F)(Type))
    {
        if(node->m_left != NULL) Iterate(node->m_left, F);
        if(node != NULL) F(node->m_data);
        if(node->m_right != NULL) Iterate(node->m_right, F);
        return R_SUCCESS;
    }    

    RESULT Iterate(RESULT (*F)(Type))
    {
        if(m_root != NULL)
        {
            if(m_root->m_left != NULL) Iterate(m_root->m_left, F);
            if(m_root != NULL) F(m_root->m_data);
            if(m_root->m_right != NULL) Iterate(m_root->m_right, F);
            return R_SUCCESS;
        }
        else
        {
            return R_BTREE_EMPTY;
        }
    }
    // *********************************************************************************

    // Is tree empty?
    RESULT Empty()
    {
        return (m_root == NULL) ? R_YES : R_NO ;
    }

    static RESULT Print(btreenode<Type>*node)
    {
        // In order by default
        if(node->m_left != NULL) Print(node->m_left);
#ifdef _DEBUG
        printf("node->m_data: %d\n", node->m_data);
#endif
        if(node->m_right != NULL) Print(node->m_right);

        return R_SUCCESS;
    }

    int GetNodeCount(){ return m_NodeCount; }

    RESULT Print()
    {
        if(Empty() == R_YES) 
        {
#ifdef _DEBUG
            printf("Tree Empty\n");
#endif
        }
        else
        {
#ifdef _DEBUG
            printf("Tree:\n");
#endif
            btreenode<Type>*cur = m_root;
            return Print(cur);
        }
        return R_SUCCESS;
    }

private:
    btreenode<Type> *m_root;
    int m_NodeCount;
};

} // namespace dss