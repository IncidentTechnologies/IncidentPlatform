 //
//  XMPCourse.mm
//  gtarLearn
//
//  Created by Kate Schnippering on 6/9/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPCourse.h"
#import "XMPContent.h"
#import "XMPChapter.h"
//#import "dss_list.h"

@implementation XMPCourse;

using namespace dss;

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;

    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_COURSE;
    
    //CRM([self ConstructCourse], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id)initWithCourseTitle:(NSString *)title author:(NSString *)author description:(NSString *)description shortdescription:(NSString *)shortdescription
{
    RESULT r = R_SUCCESS;
    
    m_Title = [[NSString alloc] initWithString:title];
    m_Author = [[NSString alloc] initWithString:author];
    m_Description = [[NSString alloc] initWithString:description];
    m_ShortDescription = [[NSString alloc] initWithString:shortdescription];
    
    m_xmpNode = NULL;
    
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_COURSE;
    m_Name = @"course";
    
    m_XMPContent = [[XMPContent alloc] init];
    [self AddXMPObject:m_XMPContent];
    
    return self;
Error:
    return NULL;
    
}

-(RESULT)ConstructCourse {
    RESULT r = R_SUCCESS;
    
    m_Description = NULL;
    m_Title = NULL;
    m_Author = NULL;
    m_unlock = 0;
    
    XMPObject *headerObj = NULL;
    if((headerObj = [self GetChildWithName:@"header"]) != NULL ) {
        m_ShortDescription = [headerObj GetChildTextWithName:@"shortdescription"];
        m_Description = [headerObj GetChildTextWithName:@"description"];
        m_Title = [headerObj GetChildTextWithName:@"title"];
        m_Author = [headerObj GetChildTextWithName:@"author"];
        
        XMPObject *unlockObj = NULL;
        if((unlockObj = [headerObj GetChildWithName:@"Unlock"]) != NULL ){
            XMPValue unlockVal = [unlockObj GetAttributeValueWithName:@"value"];
            
            if(unlockVal.m_ValueType == XMP_VALUE_INTEGER){
                unlockVal.GetValueInt((long int *)(&m_unlock));
            }else if(unlockVal.m_ValueType == XMP_VALUE_DOUBLE){
                unlockVal.GetValueDouble(&m_unlock);
            }
        }
    }
    
Error:
    return r;
}

-(RESULT)AddChapter:(XMPChapter *)chapter {
    if(m_XMPContent != NULL) {
        return [m_XMPContent AddXMPObject:(XMPObject*)chapter];
    }
    else
        return R_FAIL;
}

-(XMPChapter*)AddNewChapterWithName:(NSString *)name description:(NSString *)description {
    XMPChapter *chapter = [[XMPChapter alloc] initWithName:name description:description];
    [self AddChapter:chapter];
    return chapter;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    XMPNode *headerNode = new XMPNode((char *)"header", node);
    node->AddChild(headerNode);
    
    XMPNode *tempNode = new XMPNode((char *)"description", headerNode);
    tempNode->AppendContent((char*)[m_Description UTF8String]);
    headerNode->AddChild(tempNode);
    
    tempNode = new XMPNode((char *)"shortdescription", headerNode);
    tempNode->AppendContent((char*)[m_ShortDescription UTF8String]);
    headerNode->AddChild(tempNode);
    
    tempNode = new XMPNode((char *)"title", headerNode);
    tempNode->AppendContent((char*)[m_Title UTF8String]);
    headerNode->AddChild(tempNode);
    
    tempNode = new XMPNode((char *)"author", headerNode);
    tempNode->AppendContent((char*)[m_Author UTF8String]);
    headerNode->AddChild(tempNode);
    
    tempNode = new XMPNode((char *)"unlock", headerNode);
    tempNode->AddAttribute(new XMPAttribute((char *)"value", m_unlock));
    tempNode->AddChild(tempNode);
    
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
