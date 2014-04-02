//
//  XMPObject.m
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"
#import "XMPObjectFactory.h"

@implementation XMPObject 

-(id) init {
    RESULT r = R_SUCCESS;
    
    //m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_OBJECT;
    m_contents = [[NSMutableArray alloc] init];
    m_variables = new list<XMPValue*>();

    if(m_xmpNode != NULL)
        m_Name = [[NSString alloc] initWithCString:m_xmpNode->GetName() encoding:NSUTF8StringEncoding];
    else if([m_Name length] == 0)
        m_Name = @"xmp";
    
    // Note: This will build the children before the sub-class initialization
    [self BuildChildren];
    
    return self;
Error:
    return NULL;
}

-(XMPValue)GetXMPValueOfChild:(NSString *)childName withAttribute:(NSString*)attributeName {
    XMPObject *childObject = [self GetChildWithName:childName];
    XMPValue retVal;
    
    if( childObject != NULL)
        return [childObject GetAttributeValueWithName:attributeName];
    else
        return retVal;
}

-(XMPAttribute*)GetAttributeWithName:(NSString *)name {
    return m_xmpNode->GetAttribute((char*)[name cStringUsingEncoding:NSUTF8StringEncoding]);
}

-(BOOL)HasAttributeWithName:(NSString *)name {
    return m_xmpNode->HasAttribute((char*)[name cStringUsingEncoding:NSUTF8StringEncoding]);
}

-(XMPValue)GetAttributeValueWithName:(NSString *)name {
    XMPAttribute *tempAttr = NULL;
    if((tempAttr = m_xmpNode->GetAttribute((char*)[name cStringUsingEncoding:NSUTF8StringEncoding])) != NULL)
        return tempAttr->GetXMPValue();
    
    return XMPValue();
}

// Combines HasChildWithName and GetChildWithName for getting text content
-(NSString*)GetChildTextWithName:(NSString *)name {
    for (id xmpObj in m_contents) {
        if([xmpObj isMemberOfClass:[XMPObject class]]) {
            XMPObject *tempObj = (XMPObject*)(xmpObj);
            if([tempObj->m_Name isEqualToString:name])
                return [tempObj Text];
        }
    }
    
    return NULL;
}

-(BOOL)HasChildWithName:(NSString *)name {
    if([self GetChildWithName:name] != NULL)
        return true;

    return false;
}

-(XMPObject*)GetChildWithName:(NSString *)name {
    for (id xmpObj in m_contents) {
        if([xmpObj isMemberOfClass:[XMPObject class]]) {
            XMPObject *tempObj = (XMPObject*)(xmpObj);
            if([tempObj->m_Name isEqualToString:name])
                return tempObj;
        }
    }
    
    return NULL;
}

-(NSString*)Text {
    if(m_xmpNode->HasContent())
        return [[NSString alloc] initWithCString:m_xmpNode->text() encoding:NSUTF8StringEncoding];
    else
        return NULL;
}

// This will go through the contents and look for an object
// with the given XMP node
-(bool)ContentHasXMPNode:(XMPNode*)xmpNode {
    for (id xmpObj in m_contents) {
        if([xmpObj isMemberOfClass:[XMPObject class]]) {
            XMPObject *tempObj = (XMPObject*)(xmpObj);
            if(tempObj->m_xmpNode == xmpNode)
                return true;
        }
    }
    
    return false;
}

-(XMPValue*)AddVariableFromXMPNode:(XMPNode*)xmpNode {
    XMPValue *xmpVar = new XMPValue(xmpNode);
    
    for(list<XMPValue*>::iterator it = m_variables->First(); it != NULL; it++) {
        if(strcpy(xmpVar->GetName(), (*it)->GetName()) == 0) {
            // Variable exists so update it's value
            (*it) = xmpVar;
            
            if(xmpVar != NULL) {
                delete xmpVar;
                xmpVar = NULL;
            }
            
            return (*it);
        }
    }
    
    m_variables->Append(xmpVar);
    return xmpVar;
}

-(RESULT)BuildChildren {
    RESULT r = R_SUCCESS;
    
    if(m_xmpNode != NULL) {
        list<XMPNode*> *childrens = m_xmpNode->GetChildren();
        for(list<XMPNode*>::iterator it = childrens->First(); it != NULL; it++) {
            // Check for variables (handled separately)
            char *pszName = (*it)->GetName();
            if(strcmp(pszName, (char*)"var") == 0) {
                // Add variable here
                [self AddVariableFromXMPNode:(*it)];
            }
            else {
                // Check to see this node isn't already present
                if([self ContentHasXMPNode:(*it)] == false) {
                    XMPObject *tempObject = [XMPObjectFactory MakeXMPObject:(*it)];
                    if(tempObject != NULL)
                        [m_contents addObject:tempObject];
                }
            }
        }
    }
    
Error:
    return r;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    
    return self;
Error:
    return NULL;
}

-(RESULT)AddXMPObject:(XMPObject*)xmpObj {
    [m_contents addObject:xmpObj];
    return R_SUCCESS;
}

-(RESULT)AddXMPVariable:(XMPValue*)xmpVar {
    m_variables->Append(xmpVar);
    return R_SUCCESS;
}

-(RESULT)RemoveXMPObject:(XMPObject*)xmpObj {
    if([m_contents containsObject:xmpObj]) {
        [m_contents removeObject:xmpObj];
        return R_SUCCEED;
    }
    else {
        return R_FAIL;
    }
}

-(RESULT)RemoveXMPVariable:(XMPValue*)xmpVar {
    RESULT r = R_SUCCEED;
    
    XMPValue* tempVal = NULL;
    CRM(m_variables->Remove(tempVal, (void*)(xmpVar), GET_BY_ITEM), "RemoveXMPVariable: Failed to remove variable from XMP Object");;

Error:
    return r;
}

-(NSString*)GetName {
    return m_Name;
}

-(void)SetName:(NSString*)strName {
    m_Name = [[NSString alloc] initWithString:strName];
}

-(void)SetType:(XMP_OBJECT_TYPE)type {
    m_type = type;
}

-(XMPTree*)GetXMPTree {
    XMPTree *tree = new XMPTree();
    tree->AddChild([self CreateXMPNodeFromObjectWithParent:tree->GetRootNode()]);
    return tree;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    // Shouldn't have any children, but if it does
    for(XMPObject *child in m_contents) {
        if(child->m_type != XMP_OBJECT_OBJECT) {
            XMPNode *childNode = [child CreateXMPNodeFromObjectWithParent:node];
            node->AddChild(childNode);
        }
    }
    
    return node;
}

@end
