#ifndef _DSS_LIST_H
#define _DSS_LIST_H
#pragma once

#include "EHM.h"

namespace dss {

// list = Singly Linked List
// A low level single linked list. This should be dealt with as a primitive
// and can be reused for whatever purpose.  Template based so the data items
// contained within are non-specific.

// Two build modes
// BEST_PERFORMANCE
// Best performance will focus on having the fastest run time and will not worry about the 
// memory footprint as much.  This list will look much more like a double linked list.

// BEST_MEMORY
// Best memory will focus on having the smallest memory footprint and will not worry as much about
// the run time.

#undef BEST_PERFORMANCE
#undef BEST_MEMORY		

// Functions
// List should define stack/queue functionality
// as well as generic linked list functionality
// each functionality should be optimized for that
// specific use so no using "insert" with PushBack
// and if this requires extra memory overhead this is
// deemed to be ok

// LIST
// Insert - Insert a node at a given point in the list by Iterator, ID, or Item
//          an important note about this function is you will be given the choice to append
//          or prepend the item to the location provided.
// Remove - Remove a given node, either by ID, Item or Iterator
// GetItem - Get a given item by ID, Item, or Iterator

typedef enum {
	GET_BY_ID,
	GET_BY_ITEM,
	GET_BY_ITERATOR,
    GET_BY_POSITION
} get_by_type;

typedef enum {
	PLACE_APPEND,
	PLACE_PREPEND
} place_type;

// QUEUE (limited list)
// Append - Push an item to the back of the queue
// Detach - Pop an item from the back of the queue

// STACK (limited queue)
// Push - Push an item to the top of the stack 
// Pop - Pop an item from the top of the stack

template <class T>
class listNode {
public:
	T m_Item;
	listNode *m_pNextItem;
	
	listNode *m_pPrevItem;

	int m_iID;

	// constructor
	listNode() {
		m_pNextItem = NULL;
		m_pPrevItem = NULL;
	}

	listNode(int ID) {
		m_pNextItem = NULL;
		m_pPrevItem = NULL;
		m_iID = ID;
	}

	listNode(int ID, T item) {
		m_Item = item;
		m_pNextItem = NULL;		
		m_pPrevItem = NULL;
		m_iID = ID;
	}
	// destructor
	~listNode() {}

	void Print() {
		printf("listnode id:%d value:%d", this->m_iID, this->m_Item);
	}
};


template <class T>
class list {
public:
	listNode<T> *m_pFirstItem;
	listNode<T> *m_pLastItem;
	int m_cCount;
	int m_cIDCount;

    int length(){ return m_cCount; }
    int Length(){ return length(); }

	// constructor
	list() {
	    m_pFirstItem = NULL;
		  m_pLastItem = NULL;
	    m_cCount = 0;
	    m_cIDCount = 0;
    }

	list(T item) {
	    m_pFirstItem = new listNode<T>(0, item);
		  m_pLastItem = m_pFirstItem;
	    m_cCount = 1;
	    m_cIDCount = 1;
    }

	// destructor
	~list() {
		listNode<T> *temp;
		temp = m_pFirstItem;
		for(int i=0;(i < m_cCount) && temp != NULL ;i++) {
			listNode<T> *temp_for = temp;
			temp = temp->m_pNextItem;
			delete temp_for;
		}    	
	}
	
public:
	class iterator;
	class iterator {
		friend class list<T>;

	public:
		iterator() :
		  m_itNode(NULL)
		{/*empty stub*/}
		
		iterator(listNode<T> *itNode) :
		  m_itNode(itNode)
		{/*empty stub*/}

		~iterator(){/*empty stub*/}

		// Operators
		// Unlike the STL we are defining a & as the actual node
		// and the * operator as the data item.
		
        //listNode<T>* operator*() const {return (m_itNode); }
        //T& operator &() const { return (*m_itNode).m_Item; }

        T& operator*() const { return (*m_itNode).m_Item; }		
        listNode<T>* operator&() const {return (m_itNode); }

		iterator& operator++(int) {
			m_itNode = m_itNode->m_pNextItem;
			return *this;
		}

		iterator& operator--(int) {
			m_itNode = m_itNode->m_pPrevItem;
			return *this;
		}

        bool operator!=(iterator i){ return (i.m_itNode != this->m_itNode); }
        bool operator==(iterator i){ return (i.m_itNode == this->m_itNode); }

	private:
		listNode<T> *m_itNode;
	};

public:
	iterator First(){
        return (iterator)(m_pFirstItem);
    }
    
	iterator Last(){
        return (iterator)(m_pLastItem);
    }
    
	int Size(){ return m_cCount; }

    // Output to Static Buffer function will return a pointer to
    // a static array containing the elements
    T* StaticBufferCopy() {
        T* pTemp = new T[length()];
        int i = 0;
        for(list<T>::iterator it = First(); it != NULL; it++)
        {
            pTemp[i] = (*it);
            i++;
        }
        return pTemp;
    }
	
	// We want to maintain normal functionality
	// however we may have an inconsistent tree
	// in this case we will return -1 otherwise 0 for non-empty and 1 for empty
	int Empty() {
		if(m_pFirstItem == NULL)
			if(m_pLastItem == NULL)
				if(m_cCount == 0)
					return 1;
				else 
          return -1;
			else 
        return -1;
		else 
      return 0;
	}

private:
	RESULT DeleteNode(listNode<T> *d_cur) {
		RESULT r = R_LIST_OK;
		if(d_cur == m_pFirstItem && d_cur == m_pLastItem) {
			// first and only node in the list
			m_pFirstItem = m_pLastItem = NULL;
			delete d_cur;
			d_cur = NULL;
			m_cCount--;
		}
		else if(d_cur == m_pFirstItem) {
			m_pFirstItem = d_cur->m_pNextItem;
			m_pFirstItem->m_pPrevItem = NULL;
			delete d_cur;			
			d_cur = NULL;
			m_cCount--;
		}
		else if(d_cur == m_pLastItem) {
			m_pLastItem = d_cur->m_pPrevItem;
			m_pLastItem->m_pNextItem = NULL;
			delete d_cur;		
			d_cur = NULL;
			m_cCount--;
		}
		else {
			d_cur->m_pNextItem->m_pPrevItem = d_cur->m_pPrevItem;
			d_cur->m_pPrevItem->m_pNextItem = d_cur->m_pNextItem;
			delete d_cur;
			d_cur = NULL;
			m_cCount--;
		}

Error:
		return r;
	}

	RESULT InsertNode(T item, listNode<T> *cur, place_type pt) {
		RESULT r = R_LIST_OK;
		
		listNode<T> *NewNode = new listNode<T>(m_cIDCount, item);
		m_cIDCount++;

		// Found ID, insert item
		if(pt == PLACE_APPEND) {
			if(cur == m_pLastItem) {
				cur->m_pNextItem = NewNode;
				NewNode->m_pPrevItem = cur;
				m_pLastItem = NewNode;
			}
			else {
				NewNode->m_pNextItem = cur->m_pNextItem;
				NewNode->m_pPrevItem = cur;

				cur->m_pNextItem->m_pPrevItem = NewNode;
				cur->m_pNextItem = NewNode;
			}
			m_cCount++;
		}
		else if(pt == PLACE_PREPEND) {
			if(cur == m_pFirstItem) {
				cur->m_pPrevItem = NewNode;
				NewNode->m_pNextItem = cur;
				m_pFirstItem = NewNode;
			}
			else {
				NewNode->m_pNextItem = cur;
				NewNode->m_pPrevItem = cur->m_pPrevItem;

				cur->m_pPrevItem->m_pNextItem = NewNode;
				cur->m_pPrevItem = NewNode;
			}
			m_cCount++;
		}
Error:
		return r;
	}

	RESULT InsertFirstNode(T item) {
		RESULT r = R_LIST_OK;
		
		m_pFirstItem = new listNode<T>(m_cIDCount, item);
		CPR(m_pFirstItem);
        
		m_pLastItem = m_pFirstItem;
		m_cCount++;
		m_cIDCount++;

Error:
		return r;
	}

	RESULT GetNode(listNode<T>* &node, void *pLocation, get_by_type gbt) {
		RESULT r = R_SUCCESS;
		if(Empty() == 1) {
			// Cannot retrieve anything out of an empty list
			return R_LIST_EMPTY;
		}
		else {
			node = m_pFirstItem;
			switch(gbt) {				
                case GET_BY_ID: {
                    while(node->m_iID != *((int*)(pLocation)))
                    {
                      node = node->m_pNextItem;
                      if(node == NULL)
                      {
                        return R_LIST_ID_NOT_FOUND;
                      }
                    }			
                } break;

                case GET_BY_ITEM: {
                    while(node->m_Item != *((T*)(pLocation)))
                    {
                      node = node->m_pNextItem;
                      if(node == NULL)
                      {
                        return R_LIST_ITEM_NOT_FOUND;
                      }
                    }				
                } break;
                    
                case GET_BY_POSITION: {
                    for(int i = 0; i < *((int*)(pLocation)); i++)
                        node = node->m_pNextItem;
                } break;

                case GET_BY_ITERATOR: {
                    node = reinterpret_cast<listNode<T>*>(pLocation);
                } break;

                default: break;
			}
		}

Error:
		return r;
	}
    
public:
    list<T> operator+(list<T> &rhs) {
        list<T> res;
        
        for(list<T>::iterator it = First(); it != NULL; it++)
            res.Push(*it);
        
        for(list<T>::iterator it = rhs.First(); it != NULL; it++)
            res.Push(*it);
        
        return res;
    }
    
    T& operator[](const int& i) {
        listNode<T> *node = NULL;
        this->GetNode(node, (void*)(&i), GET_BY_POSITION);
        return node->m_Item;
    }
    
    const T& operator[](const int& i) const {
        listNode<T> *node = NULL;
        this->GetNode(node, (void*)(&i), GET_BY_POSITION);
        return node->m_Item;
    }

public:
	// LIST
	// ******************************************************************************************
	// Insert will insert an element 
	// notice location is a void* as it can be
	// a pointer to an ID, an Item, or it can be an Iterator as well
	RESULT Insert(T item, void *pLocation, place_type pt, get_by_type gbt) {
		RESULT r = R_K;
		if(Empty() == 1)
		{
			// If empty we return an error based on the get_by_t
			// except for the iterator type where we will attempt to
			// find (but we are adding fail safes here as well)
			switch(gbt) {
				case GET_BY_ID:		return R_LIST_ID_NOT_FOUND;		break;
				case GET_BY_ITEM:	return R_LIST_ITEM_NOT_FOUND;	break;
				case GET_BY_ITERATOR: {
					if(reinterpret_cast<listNode<T>*>(pLocation) == m_pFirstItem) {
						r = InsertFirstNode(item);
					}
				} break;
				default:											break;				
			}
		}
		else {
			listNode<T> *cur;			
			CR(GetNode(cur, pLocation, gbt));
			CR(InsertNode(item, cur, pt));						
		}
Error:
		return r;
	}	

	RESULT GetItem(T &obj, void *pLocation, get_by_type gbt) {
		RESULT r = R_K;
		if(Empty() == 1) {
			// Cannot retrieve anything out of an empty list
			return R_LIST_EMPTY;
		}
		else {
			listNode<T> *cur;			
			CR(GetNode(cur, pLocation, gbt));
			obj = cur->m_Item;			
		}
Error:
		return r;
	}	

	// Remove will delete and element out of the list
	// again the location is a void*
	RESULT Remove(void *pLocation, get_by_type gbt) {
		RESULT r = R_SUCCESS;
		if(Empty() == 1) {
			// Cannot remove anything out of an empty list
			return R_LIST_EMPTY;
		}
		else {
			listNode<T> *cur;			
			CR(GetNode(cur, pLocation, gbt));
			CR(DeleteNode(cur));			
		}
Error:		
		return r;
	}
    
    // This version of Remove will also save the item
    RESULT Remove(T &obj, void *pLocation, get_by_type gbt) {
		RESULT r = R_SUCCESS;
		if(Empty() == 1) {
			// Cannot remove anything out of an empty list
			return R_LIST_EMPTY;
		}
		else {
			listNode<T> *cur;
			CR(GetNode(cur, pLocation, gbt));
            obj = cur->m_Item;
			CR(DeleteNode(cur));
		}
    Error:
		return r;
	}

  RESULT Remove(listNode<T> *node) {
      RESULT r = R_K;
      
      CR(DeleteNode(node));

Error:
      return r;
  }

	// QUEUE
	// ******************************************************************************************
	// Append will add an element to the end of the queue
	// (NEED TO MAKE IT COMPAT)
	RESULT Append(T item) {
    RESULT r = R_K;
		if(m_pFirstItem == NULL){
		    // empty list
		    m_pFirstItem = new listNode<T>(m_cIDCount, item);
		    CPR(m_pFirstItem);
			  m_pLastItem = m_pFirstItem;

		    m_cCount++;
		    m_cIDCount++;
	  } 
		else {
      listNode<T> *temp = m_pFirstItem;
      
      while(temp->m_pNextItem != NULL)
        temp = temp->m_pNextItem;

      temp->m_pNextItem = new listNode<T>(m_cIDCount, item);			
      temp->m_pNextItem->m_pPrevItem = temp;
      m_pLastItem = temp->m_pNextItem;

      m_cCount++;
      m_cIDCount++;
    }
Error:
		return r;
  }

	// This detaches the last element
	// and returns it (not a copy!)
	T Detach() {
		listNode<T> *temp = m_pLastItem;
		if(Empty() == 1)
			return NULL;
		else if(m_pLastItem == m_pFirstItem) {
			temp = m_pLastItem;
			m_pLastItem = NULL;
			m_pFirstItem = NULL;
			m_cCount--;
		}
		else {			
			m_pLastItem = m_pLastItem->m_pPrevItem;
			m_pLastItem->m_pNextItem = NULL;
			m_cCount--;
		}
		return temp->m_Item;
	}

	// STACK
	// ******************************************************************************************
	// Push will add an element to the beginning of the list
	RESULT Push(T item) {
		RESULT r = R_SUCCESS;
		listNode<T> *NewNode = new listNode<T>(m_cIDCount, item);
		m_cIDCount++;
		if(Empty() == 1) {
			m_pFirstItem = NewNode;
			m_pLastItem = m_pFirstItem;
			m_cCount++;
		}
		else if(m_pFirstItem == m_pLastItem) {
			m_pFirstItem = NewNode;
			m_pFirstItem->m_pNextItem = m_pLastItem;
			m_pLastItem->m_pPrevItem = m_pFirstItem;
			m_cCount++;
		}
		else {
			NewNode->m_pNextItem = m_pFirstItem;
			m_pFirstItem->m_pPrevItem = NewNode;
			m_pFirstItem = NewNode;
			m_cCount++;
		}
Error:
		return r;
	}

	T Pop() {
		listNode<T> *temp = m_pFirstItem;
		if(Empty() == 1) {
			// Nothing to pop off an empty stack
			return NULL;
		}
		else if(m_pFirstItem == m_pLastItem) {
			temp = m_pFirstItem;
			m_pFirstItem = NULL;
			m_pLastItem = NULL;
			m_cCount--;
		}	
		else {
			m_pFirstItem = m_pFirstItem->m_pNextItem;
			m_pFirstItem->m_pPrevItem = NULL;
			m_cCount--;
		}

		return temp->m_Item;
	}

	// Remove will remove an item based on it's ID
	int Remove_old(int ID) {
    if(m_pFirstItem == NULL) {
      // list empty
      return -1;
    } 
		else {
		    listNode<T> *prev = m_pFirstItem;
		    listNode<T> *temp = prev->m_pNextItem;
		    if(temp == NULL) {
			    // list has one item
			    if(prev->m_iID == ID) {
				    m_pFirstItem = NULL;
				    m_cCount--;
				    return m_cCount;
			    } 
          else 
          {
				    // not found
				    return -1;
			    }
		    } 
        else {
			    if(prev->m_iID == ID) {
				    // second item
				    m_pFirstItem = temp;
				    m_cCount--;
				    return m_cCount;
			    } 
          else 
          {
				    while(temp->m_iID != ID) {
					    prev = temp;
					    temp = temp->m_pNextItem;

              // not found
					    if(temp == NULL) 
						    return -1;
				    }

				    prev->m_pNextItem = temp->m_pNextItem;
				    delete temp;
				    m_cCount--;
				    return m_cCount;
			    }
		    }
	    }
	}

  typedef RESULT(*FunctionPtrArg)(T, void*);

  // Iterate
  // Runs a function through the list
  // ************************************************************************
  // Pass as Type: RESULT Function(Type)
  RESULT Iterate(FunctionPtrArg F, void *Arg) {
      listNode<T> *Cur = m_pFirstItem;
      while(Cur != NULL) {
          F(Cur->m_Item, Arg);
          Cur = Cur->m_pNextItem;
      }

      return R_SUCCESS;
  }

  // Pass by reference
  RESULT Iterate(RESULT (*F)(T* &))
  {
      listNode<T> *Cur = m_pFirstItem;
      while(Cur != NULL) {
          F(Cur->m_Item);
          Cur = Cur->m_pNextItem;
      }

      return R_SUCCESS;
  }

  // Pass normally
  RESULT Iterate(RESULT (*F)(T))
  {
      listNode<T> *Cur = m_pFirstItem;
      while(Cur != NULL) {
          F(Cur->m_Item);
          Cur = Cur->m_pNextItem;
      }

      return R_SUCCESS;
  }

	// Debug Print List
	// also checks size coherency
	void Print() {
	    listNode<T> *temp = m_pFirstItem;
      int c = 0;
      printf("** List *** Count: %d **********\n", Size());
	    while(temp != NULL) {
		    temp->Print();
			
        if(temp == m_pFirstItem) printf(" F");
        if(temp == m_pLastItem) printf(" L");
        printf("\n");

        c++;
		    temp = temp->m_pNextItem;
	    }

      if(c != Size()) printf("** debug count mismatch!:%d\n", c);
      else printf("***************************\n");
    }
};

} // namespace dss

#endif // _DSS_LIST_H
