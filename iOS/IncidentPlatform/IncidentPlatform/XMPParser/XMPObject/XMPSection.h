//
//  XMPSection.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/9/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@class XMPSectionLesson;

@interface XMPSection : XMPObject {
    
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) init;

-(RESULT)ConstructSection;
-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

- (RESULT)AddSectionLesson:(XMPSectionLesson*)lesson;
- (XMPSectionLesson*)AddNewSectionLessonWithName:(NSString*)name;

@end
