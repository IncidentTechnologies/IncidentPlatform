 //
//  XMPChapter.mm
//  gtarLearn
//
//  Created by Kate Schnippering on 6/9/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPChapter.h"
#import "XMPSection.h"
//#import "dss_list.h"

@implementation XMPChapter

using namespace dss;

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;

    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_CHAPTER;
    
    CRM([self ConstructChapter], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id) initWithName:(NSString *)name description:(NSString *)description
{
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_CHAPTER;
    m_Name = @"chapter";
    
    m_ChapterName = [[NSString alloc] initWithString:name];
    m_ChapterDescription = [[NSString alloc] initWithString:description];
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructChapter {
    RESULT r = R_SUCCESS;
    
    m_ChapterName = NULL;
    m_ChapterDescription = NULL;
    m_unlock = 0;
    
    NSString * tempName;
    
    XMPValue chapterNameVal = [self GetAttributeValueWithName:@"name"];
    if(chapterNameVal.m_ValueType != XMP_VALUE_INVALID){
        tempName = [[NSString alloc] initWithCString:chapterNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    }else{
        tempName = @"";
    }
    
    m_ChapterName = tempName;
    
    XMPValue chapterDescVal = [self GetAttributeValueWithName:@"description"];
    if(chapterDescVal.m_ValueType != XMP_VALUE_INVALID){
        tempName = [[NSString alloc] initWithCString:chapterDescVal.GetPszValue() encoding:NSUTF8StringEncoding];
    }else{
        tempName = @"";
    }
    
    m_ChapterDescription = tempName;
    
    XMPValue chapterUnlockVal = [self GetAttributeValueWithName:@"unlock"];
    if(chapterUnlockVal.m_ValueType != XMP_VALUE_INVALID){
        chapterUnlockVal.GetValueDouble(&m_unlock);
    }
    
Error:
    return r;
}

-(RESULT)AddSection:(XMPSection *)section {
    // return [self AddXMPObject:section];
    [m_contents addObject:section];
    return R_SUCCESS;
}

-(XMPSection*)AddNewSection {
    XMPSection * section = [[XMPSection alloc] init];
    [self AddSection:section];
    return section;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    node->AddAttribute(new XMPAttribute((char *)"name", (char *)[m_ChapterName UTF8String]));
    node->AddAttribute(new XMPAttribute((char *)"description", (char *)[m_ChapterDescription UTF8String]));
    node->AddAttribute(new XMPAttribute((char *)"unlock", m_unlock));
    
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
