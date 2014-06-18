//
//  XMPLessonChapter.m
//  gtarLearn
//
//  Created by Idan Beck on 12/17/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPLessonChapter.h"
#import "XMPLesson.h"
#import "XMPAttribute.h"
#import "XMPValue.h"
#import "XMPObjectFactory.h"

@implementation XMPLessonChapter

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_LESSON_CHAPTER;
    
    // Construct Lesson particulars
    //CRM([self ConsructLessonChapterFromChapterNode], "initWithParentLesson: Failed to construct chapter");
    
    return self;
Error:
    return NULL;

}

-(RESULT)ConsructLessonChapterFromChapterNode {
    RESULT r = R_SUCCESS;
    
    // Get the title
    XMPValue chapterTitle = m_xmpNode->GetAttribute((char*)"title")->GetXMPValue();
    m_Title = [[NSString alloc] initWithCString:chapterTitle.GetPszValue() encoding:NSUTF8StringEncoding];
    
Error:
    return r;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    node->AddAttribute(new XMPAttribute("title", (char*)[m_Title UTF8String]));
    
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
