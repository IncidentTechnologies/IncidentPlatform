//
//  XMPLessonChapter.h
//  gtarLearn
//
//  Created by Idan Beck on 12/17/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMPObject.h"
#import "XMPNode.h"

@class XMPLesson;

@interface XMPLessonChapter : XMPObject {
    NSString *m_Title;
}

//-(id)initWithParentLesson:(XMPLesson*)parentLesson andXMPNode:(XMPNode*)xmpNode;
-(id) initWithXMPNode:(XMPNode*)xmpNode;

-(RESULT)ConsructLessonChapterFromChapterNode;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
