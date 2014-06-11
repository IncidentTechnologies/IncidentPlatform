//
//  XMPLesson.h
//  gtarLearn
//
//  Created by Idan Beck on 12/17/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@interface XMPLesson : XMPObject {
    NSString *m_Description;
    NSString *m_Title;
    NSString *m_Author;
}

- (id) initWithLessonName:(NSString *)LessonName;

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(RESULT)ConstructLesson;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
