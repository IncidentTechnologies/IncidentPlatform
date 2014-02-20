 //
//  XMPLesson.m
//  gtarLearn
//
//  Created by Idan Beck on 12/17/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPLesson.h"
#import "dss_list.h"

@implementation XMPLesson

using namespace dss;

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;

    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithLessonName: Failed to init super");
    m_type = XMP_OBJECT_LESSON;
    
    CRM([self ConstructLesson], "initWithLessonName: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructLesson {
    RESULT r = R_SUCCESS;
    
    XMPObject *headerObj = NULL;
    if((headerObj = [self GetChildWithName:@"header"]) != NULL ) {
        m_Description = [headerObj GetChildTextWithName:@"description"];
        m_Title = [headerObj GetChildTextWithName:@"title"];
        m_Author = [headerObj GetChildTextWithName:@"author"];
    }
    else {
        m_Description = NULL;
        m_Title = NULL;
        m_Author = NULL;
    }
    
Error:
    return r;
}

@end
