//
//  XMPCourse.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/9/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@class XMPContent;
@class XMPChapter;

@interface XMPCourse : XMPObject {
    NSString *m_Description;
    NSString *m_Title;
    NSString *m_Author;
    double m_unlock;
    
    XMPContent *m_XMPContent;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithCourseTitle:(NSString *)title author:(NSString *)author description:(NSString *)description;

-(RESULT)ConstructCourse;
-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

- (RESULT)AddChapter:(XMPChapter*)chapter;
- (XMPChapter*)AddNewChapterWithName:(NSString *)name description:(NSString *)description;

@end
