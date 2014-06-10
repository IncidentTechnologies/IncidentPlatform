//
//  XMPSection.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/9/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@interface XMPSectionLesson : XMPObject {
    NSString *m_SectionName;
    NSString *m_SectionDescription;
    
    int m_index;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithSectionLessonName:(NSString *)name andDescription:(NSString *)description andIndex:(int)index;

-(RESULT)ConstructSectionLesson;
-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
