// Binary Space Partitioning Tree Implementation
// This uses n = 3 or planes for separation
// Obviously need to include the plane for this

#pragma once

#include "RESULT.h"
#include "DimensionPrimitives.h"  // also includes the DimObject

/* NOTE: Replace with our own list.h eventually */
#include <list>

// We rotate the orientation of the separating planes
// at every level of the tree
typedef enum kd_tree_orientations
{
    DIM_TREE_DEFAULT = 0,
    DIM_TREE_YZ = 0,
    DIM_TREE_XZ = 1,
    DIM_TREE_XY = 2
} KD_TREE_ORIENTATION;

#define KD_TREE_DIM 3

class kdtreenode
{
public:
    kdtreenode(DimObject* obj, KD_TREE_ORIENTATION orientation) :
        m_positive(NULL),
        m_negative(NULL),
        m_parent(NULL),
        m_dto(orientation),
        m_plane(plane(obj->m_ptOrigin))
    {
        m_Object = obj;
        
        if(orientation == DIM_TREE_YZ) m_plane.SetPlane(obj->m_ptOrigin, vector(1.0f, 0.0f, 0.0f));
        else if(orientation == DIM_TREE_XZ) m_plane.SetPlane(obj->m_ptOrigin, vector(0.0f, 1.0f, 0.0f));
        else if(orientation == DIM_TREE_XY) m_plane.SetPlane(obj->m_ptOrigin, vector(0.0f, 0.0f, 1.0f));
    }

    kdtreenode(DimObject* obj, kdtreenode *parent) :
        m_positive(NULL),
        m_negative(NULL),
        m_parent(parent),
        m_plane(plane(obj->m_ptOrigin))
    {
        m_Object = obj;
        
        // Based on the orientation of the parent plane we orient the child orthonormally
        // such that we get the correct order
        if(m_parent->m_dto == DIM_TREE_YZ) m_plane.SetPlane(obj->m_ptOrigin, vector(0.0f, 1.0f, 0.0f));
        else if(m_parent->m_dto == DIM_TREE_XZ) m_plane.SetPlane(obj->m_ptOrigin, vector(0.0f, 0.0f, 1.0f));
        else if(m_parent->m_dto == DIM_TREE_XY) m_plane.SetPlane(obj->m_ptOrigin, vector(1.0f, 0.0f, 0.0f));
    }

    ~kdtreenode() {
        if(m_positive != NULL) m_positive = NULL;
        if(m_negative != NULL) m_negative = NULL;
    }

#ifdef _MEM_TRACK
    // Memory Management
    // **************************************************************
    void* operator new(unsigned long size, const char *file, int line) {
        void *ptr = (void *)malloc(size);
        AddTrack((DWORD)ptr, size, file, line);
        return ptr;
    }

    void operator delete(void *p) {
        RemoveTrack((DWORD)p);
        free(p);
        return;
    }
    // *************************************************************
#endif

    DimObject *m_Object;    // Pointer to object contained in node

    plane m_plane;         // splitting plane
    KD_TREE_ORIENTATION m_dto;
    kdtreenode *m_parent;  // parent node

    // Negative : left / down
    // Positive : right / up
    kdtreenode *m_positive;
    kdtreenode *m_negative;
};


class kdtree
{
public:
    kdtree() :
        m_root(NULL),
        m_Node_c(0)
    {
        
    }

    kdtree(DimObject *obj)
    {
        m_root = MEM_TRACK_NEW kdtreenode(obj, DIM_TREE_YZ);        
        m_Node_c = (m_root != NULL) ? 1 : 0;
    }

    ~kdtree()
    {
        Clear();
    }
    
    // Insert Into Tree
    // **********************************************************************
    static RESULT Insert(DimObject *obj, kdtreenode *node)
    {        
        float c = (obj->m_ptOrigin - node->m_plane.m_ptOrigin) * node->m_plane.GetNormal();

        if( c >= 0)
        {
            if(node->m_positive == NULL)
            {
                node->m_positive = MEM_TRACK_NEW kdtreenode(obj, node);
                return (node->m_positive != NULL) ? R_BTREE_NODE_INSERTED : R_BTREE_NODE_NOTCREATED;
            }
            else
            {
                return Insert(obj, node->m_positive);
            }
        }
        else
        {
            if(node->m_negative == NULL)
            {
                node->m_negative = MEM_TRACK_NEW kdtreenode(obj, node);
                return (node->m_negative != NULL) ? R_BTREE_NODE_INSERTED : R_BTREE_NODE_NOTCREATED;
            }
            else
            {
                return Insert(obj, node->m_negative);
            }
        }

        return R_SUCCESS;
    }   

    RESULT Insert(DimObject *obj)
    {
        //dimtreenode *cur = m_root;
        if(Empty() == R_YES)
        {
            m_root = MEM_TRACK_NEW kdtreenode(obj, DIM_TREE_DEFAULT);
            if(m_root != NULL) 
            {
                m_Node_c++;
                return R_BTREE_NODE_INSERTED;
            }
            else
            {
                return R_BTREE_NODE_NOTCREATED;
            }
        }
        else
        {
            // First balance the tree            
            RESULT res = Insert(obj, m_root);
            if(res == R_BTREE_NODE_INSERTED)
            {
                m_Node_c++;
                return res;
            }
            else
            {
                return res;
            }
        }
    }
    // **********************************************************************

    static RESULT OutToList(kdtreenode *node, std::list<DimObject*>&Objects)
    {
        if(node->m_positive != NULL) OutToList(node->m_positive, Objects);
        if(node->m_Object != NULL) Objects.push_back(node->m_Object);
        if(node->m_negative != NULL) OutToList(node->m_negative, Objects);

        return R_SUCCESS;
    }

    static kdtreenode* BuildTree(std::list<DimObject*>&Objects, int depth)
    {
        if(Objects.size() == 0) 
        {
            return NULL;
        }
        else
        {
            KD_TREE_ORIENTATION dto = (KD_TREE_ORIENTATION)(depth % KD_TREE_DIM);
            int median = Objects.size() / 2;
            int size = Objects.size();
            std::list<DimObject*>posHalf;
            std::list<DimObject*>negHalf;
            
            std::list<DimObject*>::iterator it = Objects.begin();
            for(int i = 0; i < median; i++) it++;

            kdtreenode *node = MEM_TRACK_NEW kdtreenode(*it, dto);

            // positive half
            if(median != 0) 
            {
                posHalf = std::list<DimObject*>(Objects.begin(), it);            
            }
            if(median != Objects.size() - 1)
            {
                it = Objects.begin();
                for(int i = 0; i < median + 1; i++) it++;
                negHalf = std::list<DimObject*>(it, Objects.end());
            }

            node->m_positive = BuildTree(posHalf, depth + 1);
            node->m_negative = BuildTree(negHalf, depth + 1);

            return node;            
        }
        
    }

    // This should be called to optimize the tree
    RESULT Balance()
    {
        std::list<DimObject*>Objects;        
        RESULT res = R_SUCCESS;
         
        res = OutToList(m_root, Objects);             

        // Once we have all the objects in a linear array
        // we can clear the k-d tree
        res = Clear();

        // Now we want to recursively create lists 
        m_root = BuildTree(Objects, 0);

        return R_SUCCESS;
    }

    static RESULT Clear(kdtreenode * &node)
    {
        if(node->m_positive != NULL)
        {
            Clear(node->m_positive);
        }
        if(node->m_negative != NULL)
        {
            Clear(node->m_negative);
        }
        delete node;
        node = NULL;
        return R_SUCCESS;
    }

    RESULT Clear()
    {
        if(Empty() == R_YES)
        {
            return R_SUCCESS;
        }
        else return Clear(m_root);
    }

    typedef RESULT(*FunctionPtrArg)(DimObject*, void*);
    typedef RESULT(*FunctionPtrArgDepthObj)(DimObject*, void*, int, DimObject*);
    typedef RESULT(*FunctionPtrArgDepthObjCams)(DimObject*, void*, int, DimObject*, camera*, camera*);

    // Iterate a Function Through the Tree
    // ********************************************************************************
    // Pass as Type: RESULT Function(Type) with depth and caller object so it doesn't reflect/refract itself
    static RESULT Iterate(kdtreenode *node, FunctionPtrArgDepthObjCams F, void* Arg, int depth, DimObject* caller, camera* pRefractCamera, camera* pReflectCamera)
    {
        if(node->m_positive != NULL) Iterate(node->m_positive, F, Arg, depth, caller, pRefractCamera, pReflectCamera);
        if(node != NULL) F(node->m_Object, Arg, depth, caller, pRefractCamera, pReflectCamera);
        if(node->m_negative != NULL) Iterate(node->m_negative, F, Arg, depth, caller, pRefractCamera, pReflectCamera);
        return R_SUCCESS;
    }    

    RESULT Iterate(FunctionPtrArgDepthObjCams F, void* Arg, int depth, DimObject* caller, camera* pRefractCamera, camera* pReflectCamera)
    {
        if(m_root != NULL)
        {
            if(m_root->m_positive != NULL) Iterate(m_root->m_positive, F, Arg, depth, caller, pRefractCamera, pReflectCamera);
            if(m_root != NULL) F(m_root->m_Object, Arg, depth, caller, pRefractCamera, pReflectCamera);
            if(m_root->m_negative != NULL) Iterate(m_root->m_negative, F, Arg, depth, caller, pRefractCamera, pReflectCamera);
            return R_SUCCESS;
        }
        else
            return R_BTREE_EMPTY;        
    }
    
    static RESULT Iterate(kdtreenode *node, FunctionPtrArgDepthObj F, void* Arg, int depth, DimObject* caller)
    {
        if(node->m_positive != NULL) Iterate(node->m_positive, F, Arg, depth, caller);
        if(node != NULL) F(node->m_Object, Arg, depth, caller);
        if(node->m_negative != NULL) Iterate(node->m_negative, F, Arg, depth, caller);
        return R_SUCCESS;
    }    

    RESULT Iterate(FunctionPtrArgDepthObj F, void* Arg, int depth, DimObject* caller)
    {
        if(m_root != NULL)
        {
            if(m_root->m_positive != NULL) Iterate(m_root->m_positive, F, Arg, depth, caller);
            if(m_root != NULL) F(m_root->m_Object, Arg, depth, caller);
            if(m_root->m_negative != NULL) Iterate(m_root->m_negative, F, Arg, depth, caller);
            return R_SUCCESS;
        }
        else
            return R_BTREE_EMPTY;
    }

    // Pass as Type: RESULT Function(Type) no depth
    static RESULT Iterate(kdtreenode *node, FunctionPtrArg F, void* Arg)
    {
        if(node->m_positive != NULL) Iterate(node->m_positive, F, Arg);
        if(node != NULL) F(node->m_Object, Arg);
        if(node->m_negative != NULL) Iterate(node->m_negative, F, Arg);
        return R_SUCCESS;
    }    

    RESULT Iterate(FunctionPtrArg F, void* Arg)
    {
        if(m_root != NULL)
        {
            if(m_root->m_positive != NULL) Iterate(m_root->m_positive, F, Arg);
            if(m_root != NULL) F(m_root->m_Object, Arg);
            if(m_root->m_negative != NULL) Iterate(m_root->m_negative, F, Arg);
            return R_SUCCESS;
        }
        else
            return R_BTREE_EMPTY;
    }

    // Pass by reference: RESULT Function(Type &)
    static RESULT Iterate(kdtreenode *node, RESULT (*F)(DimObject* &))
    {
        if(node->m_positive != NULL) Iterate(node->m_positive, F);
        if(node != NULL) F(node->m_Object);
        if(node->m_negative != NULL) Iterate(node->m_negative, F);
        return R_SUCCESS;
    }    

    RESULT Iterate(RESULT (*F)(DimObject* &))
    {
        if(m_root != NULL)
        {
            if(m_root->m_positive != NULL) Iterate(m_root->m_positive, F);
            if(m_root != NULL) F(m_root->m_Object);
            if(m_root->m_negative != NULL) Iterate(m_root->m_negative, F);
            return R_SUCCESS;
        }
        else
        {
            return R_BTREE_EMPTY;
        }
    }

    // Pass normally: RESULT Function(Type)
    static RESULT Iterate(kdtreenode* node, RESULT (*F)(DimObject*))
    {
        if(node->m_positive != NULL) Iterate(node->m_positive, F);
        if(node != NULL) F(node->m_Object);
        if(node->m_negative != NULL) Iterate(node->m_negative, F);
        return R_SUCCESS;
    }    

    RESULT Iterate(RESULT (*F)(DimObject*))
    {
        if(m_root != NULL)
        {
            if(m_root->m_positive != NULL) Iterate(m_root->m_positive, F);
            if(m_root != NULL) F(m_root->m_Object);
            if(m_root->m_negative != NULL) Iterate(m_root->m_negative, F);
            return R_SUCCESS;
        }
        else
        {
            return R_BTREE_EMPTY;
        }
    }
    // *********************************************************************************

    // Is tree empty?
    RESULT Empty(){ return ( m_root == NULL ) ? R_YES : R_NO ; }

    int GetNodeCount(){ return m_Node_c; }
    

private:
    kdtreenode *m_root;
    int m_Node_c;
};

