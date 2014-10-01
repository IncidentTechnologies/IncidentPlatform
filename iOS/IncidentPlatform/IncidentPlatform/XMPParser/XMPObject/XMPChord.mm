//
//  XMPChord.m
//  IncidentPlatform
//
//  Created by Kate Schnippering on 7/28/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPChord.h"
#import "XMPNote.h"

@implementation XMPChord

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_CHORD;
    
    return self;
Error:
    return NULL;
}

-(id) initWithName:(NSString*)name beatstart:(double)beatstart duration:(double)duration
{
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_CLIP;
    m_Name = @"clip";
    
    m_name = [[NSString alloc] initWithString:name];
    
    m_beatstart = beatstart;
    m_duration = duration;
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructChord {
    RESULT r = R_SUCCESS;
    
    XMPValue trackNameVal = [self GetAttributeValueWithName:@"name"];
    if(trackNameVal.m_ValueType != XMP_VALUE_INVALID)
        m_name = [[NSString alloc] initWithCString:trackNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_name = @"";
    
    if([self HasAttributeWithName:@"beatstart"])
        [self GetAttributeValueWithName:@"beatstart"].GetValueDouble(&m_beatstart);
    else
        m_beatstart = 0.0f;
    
    if([self HasAttributeWithName:@"duration"])
        [self GetAttributeValueWithName:@"duration"].GetValueDouble(&m_duration);
    else
        m_duration= 0.0f;
    
    
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
    
    node->AddAttribute(new XMPAttribute((char *)"name", (char*)[m_name UTF8String]));
    
    node->AddAttribute(new XMPAttribute((char *)"beatstart", m_beatstart));
    node->AddAttribute(new XMPAttribute((char *)"duration", m_duration));
    
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
