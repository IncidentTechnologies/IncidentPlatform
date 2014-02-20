//
//  XMPText.m
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPText.h"

@implementation XMPText

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_TEXT;
    
    // Grab the text
    m_text = [[NSString alloc] initWithCString:m_xmpNode->text() encoding:NSUTF8StringEncoding];
    
    return self;
Error:
    return NULL;
}

@end
