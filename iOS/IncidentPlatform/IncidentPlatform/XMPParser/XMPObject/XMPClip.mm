//
//  XMPClip.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPClip.h"
#import "XMPNote.h"

@implementation XMPClip

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_CLIP;
    
    CRM([self ConstructClip], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id) initWithName:(NSString*)name Start:(double)startbeat End:(double)endbeat {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_CLIP;
    m_Name = @"clip";
    
    m_strName = [[NSString alloc] initWithString:name];
    
    m_startbeat = startbeat;
    m_endbeat = endbeat;
    
    m_clipstart = 0.0f;
    m_cliplength = endbeat - startbeat;
    
    m_fLoop = false;
    m_loopstart = 0.0f;
    m_looplength = m_cliplength;
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructClip {
    RESULT r = R_SUCCESS;
    
    XMPValue trackNameVal = [self GetAttributeValueWithName:@"name"];
    if(trackNameVal.m_ValueType != XMP_VALUE_INVALID)
        m_strName = [[NSString alloc] initWithCString:trackNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_strName = @"";
    
    if([self HasAttributeWithName:@"startbeat"])
        [self GetAttributeValueWithName:@"startbeat"].GetValueDouble(&m_startbeat);
    else
        m_startbeat = 0.0f;
    
    if([self HasAttributeWithName:@"endbeat"])
        [self GetAttributeValueWithName:@"endbeat"].GetValueDouble(&m_endbeat);
    else
        m_endbeat = 0.0f;
    
    if([self HasAttributeWithName:@"looping"])
        [self GetAttributeValueWithName:@"looping"].GetValueBool((bool*)(&m_fLoop));
    else
        m_fLoop = false;
    
    if([self HasAttributeWithName:@"loopstart"])
        [self GetAttributeValueWithName:@"loopstart"].GetValueDouble(&m_loopstart);
    else
        m_loopstart = 0.0f;
    
    if([self HasAttributeWithName:@"looplength"])
        [self GetAttributeValueWithName:@"looplength"].GetValueDouble(&m_looplength);
    else
        m_looplength = 0.0f;
    
    if([self HasAttributeWithName:@"clipstart"])
        [self GetAttributeValueWithName:@"clipstart"].GetValueDouble(&m_clipstart);
    else
        m_clipstart = 0.0f;
    
    if([self HasAttributeWithName:@"cliplength"])
        [self GetAttributeValueWithName:@"cliplength"].GetValueDouble(&m_cliplength);
    else
        m_cliplength = (m_endbeat - m_clipstart);
    
Error:
    return r;
}

-(RESULT)AddNote:(XMPNote*)track {
    [m_contents addObject:track];
    return R_SUCCESS;
}

-(XMPNote*)AddNewNote:(int)value beatstart:(double)beat duration:(double)duration {
    XMPNote *note = [[XMPNote alloc] initWithValue:value Duration:duration Beatstart:beat];
    [self AddNote:note];
    
    return note;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    node->AddAttribute(new XMPAttribute("name", (char*)[m_strName UTF8String]));
    
    node->AddAttribute(new XMPAttribute("startbeat", m_startbeat));
    node->AddAttribute(new XMPAttribute("endbeat", m_endbeat));
    
    node->AddAttribute(new XMPAttribute("looping", (char*)((m_fLoop) ? "true" : "false")));
    node->AddAttribute(new XMPAttribute("loopstart", m_loopstart));
    node->AddAttribute(new XMPAttribute("looplength", m_looplength));
    
    node->AddAttribute(new XMPAttribute("clipstart", m_clipstart));
    node->AddAttribute(new XMPAttribute("cliplength", m_cliplength));
    
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
