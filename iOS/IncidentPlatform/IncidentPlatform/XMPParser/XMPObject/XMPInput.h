//
//  XMPText.h
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPInput : XMPObject {
    
    NSString * m_formula;
    
    bool m_scrolling;
    bool m_fBlock;
}

- (id) initWithScrolling:(bool)scrolling andFormula:(NSString *)formula andBlock:(bool)block;

- (id) initWithXMPNode:(XMPNode*)xmpNode;
- (RESULT)ConstructInput;

- (XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
