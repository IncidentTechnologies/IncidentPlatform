 //
//  XMPCourse.mm
//  gtarLearn
//
//  Created by Kate Schnippering on 6/9/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPSection.h"
#import "XMPSectionLesson.h"
#import "dss_list.h"

@implementation XMPSection

using namespace dss;

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;

    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_SECTION;
    
    CRM([self ConstructSection], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id)init{
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_SECTION;
    m_Name = @"section";
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructSection {
    RESULT r = R_SUCCESS;
    
Error:
    return r;
}

-(RESULT)AddSectionLesson:(XMPSectionLesson *)lesson {
    // return [self AddXMPObject:lesson];
    [m_contents addObject:lesson];
    return R_SUCCESS;
}

-(XMPSectionLesson*)AddNewSectionLessonWithName:(NSString *)name andDescription:(NSString *)description andIndex:(int)index {
    XMPSectionLesson *lesson = [[XMPSectionLesson alloc] initWithSectionLessonName:name andDescription:description andIndex:index];
    [self AddSectionLesson:lesson];
    return lesson;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    //node = new XMPNode((char*)[m_Name UTF8String], parent);
    
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
