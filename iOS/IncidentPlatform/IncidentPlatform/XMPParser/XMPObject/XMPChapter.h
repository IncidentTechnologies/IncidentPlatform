//
//  XMPChapter.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/9/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@class XMPSection;

@interface XMPChapter : XMPObject {
    NSString *m_ChapterName;
    NSString *m_ChapterDescription;
    
    double m_unlock;
}

//@property (nonatomic, readonly) NSString * m_ChapterName;
//@property (nonatomic, readonly) NSString * m_ChapterDescription;

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithName:(NSString *)name description:(NSString *)description;

-(RESULT)ConstructChapter;
-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

- (RESULT)AddSection:(XMPSection*)section;
- (XMPSection*)AddNewSection;

@end
