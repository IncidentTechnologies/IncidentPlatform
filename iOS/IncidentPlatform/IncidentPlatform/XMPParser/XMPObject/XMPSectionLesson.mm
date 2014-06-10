 //
//  XMPCourse.mm
//  gtarLearn
//
//  Created by Kate Schnippering on 6/9/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPSectionLesson.h"
#import "dss_list.h"

@implementation XMPSectionLesson

using namespace dss;

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;

    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_SECTION_LESSON;
    
    CRM([self ConstructSectionLesson], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id)initWithSectionLessonName:(NSString *)name andDescription:(NSString *)description andIndex:(int)index
{
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_SECTION_LESSON;
    m_Name = @"sectionlesson";
    
    m_SectionName = [[NSString alloc] initWithString:name];
    m_SectionDescription = [[NSString alloc] initWithString:description];
    m_index = index;
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructSectionLesson {
    RESULT r = R_SUCCESS;

    m_SectionName = NULL;
    m_SectionDescription = NULL;
    m_index = 0;
    
    NSString * tempName;
    
    XMPValue sessionNameVal = [self GetAttributeValueWithName:@"name"];
    if(sessionNameVal.m_ValueType != XMP_VALUE_INVALID){
        tempName = [[NSString alloc] initWithCString:sessionNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    }else{
        tempName = @"";
    }
    
    m_SectionName = tempName;
    
    XMPValue sessionDescVal = [self GetAttributeValueWithName:@"description"];
    if(sessionDescVal.m_ValueType != XMP_VALUE_INVALID){
        tempName = [[NSString alloc] initWithCString:sessionDescVal.GetPszValue() encoding:NSUTF8StringEncoding];
    }else{
        tempName = @"";
    }
    
    m_SectionDescription = tempName;
    
    XMPValue sectionIndexVal = [self GetAttributeValueWithName:@"index"];
    if(sectionIndexVal.m_ValueType != XMP_VALUE_INVALID){
        sectionIndexVal.GetValueInt((long *)&m_index);
    }
    
Error:
    return r;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    node->AddAttribute(new XMPAttribute((char *)"name", (char *)[m_SectionName UTF8String]));
    node->AddAttribute(new XMPAttribute((char *)"description", (char *)[m_SectionDescription UTF8String]));
    node->AddAttribute(new XMPAttribute((char *)"index", m_index));
    
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
