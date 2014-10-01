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
    
    //CRM([self ConstructClip], "initWithXMPNode: Failed to construct from XMP");
    
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
    
    m_sound = true;
    m_display = true;
    m_persist = false;
    m_metronome = false;
    m_autocomplete = false;
    m_wrongnotes = false;
    m_requirefret = false;
    m_tempo = 0.0f;
    
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
    
    if([self HasAttributeWithName:@"sound"])
        [self GetAttributeValueWithName:@"sound"].GetValueBool((bool*)(&m_sound));
    else
        m_sound = true;
    
    if([self HasAttributeWithName:@"display"])
        [self GetAttributeValueWithName:@"display"].GetValueBool((bool*)(&m_display));
    else
        m_display = true;
    
    if([self HasAttributeWithName:@"persist"])
        [self GetAttributeValueWithName:@"persist"].GetValueBool((bool*)(&m_persist));
    else
        m_persist = true;
    
    if([self HasAttributeWithName:@"metronome"])
        [self GetAttributeValueWithName:@"metronome"].GetValueBool((bool*)(&m_metronome));
    else
        m_metronome = true;
    
    if([self HasAttributeWithName:@"autocomplete"])
        [self GetAttributeValueWithName:@"autocomplete"].GetValueBool((bool*)(&m_autocomplete));
    else
        m_autocomplete = false;
    
    if([self HasAttributeWithName:@"wrongnotes"])
        [self GetAttributeValueWithName:@"wrongnotes"].GetValueBool((bool*)(&m_wrongnotes));
    else
        m_wrongnotes = false;
    
    if([self HasAttributeWithName:@"requirefret"])
        [self GetAttributeValueWithName:@"requirefret"].GetValueBool((bool*)(&m_requirefret));
    else
        m_requirefret = false;
    
    if([self HasAttributeWithName:@"tempo"])
        [self GetAttributeValueWithName:@"tempo"].GetValueDouble((double*)(&m_tempo));
    else
        m_tempo = false;
    
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
    
    node->AddAttribute(new XMPAttribute((char *)"name", (char*)[m_strName UTF8String]));
    
    node->AddAttribute(new XMPAttribute((char *)"startbeat", m_startbeat));
    node->AddAttribute(new XMPAttribute((char *)"endbeat", m_endbeat));
    
    node->AddAttribute(new XMPAttribute((char *)"looping", (char*)((m_fLoop) ? "true" : "false")));
    node->AddAttribute(new XMPAttribute((char *)"loopstart", m_loopstart));
    node->AddAttribute(new XMPAttribute((char *)"looplength", m_looplength));
    
    node->AddAttribute(new XMPAttribute((char *)"clipstart", m_clipstart));
    node->AddAttribute(new XMPAttribute((char *)"cliplength", m_cliplength));
    
    node->AddAttribute(new XMPAttribute((char *)"sound", m_sound));
    node->AddAttribute(new XMPAttribute((char *)"display", m_display));
    node->AddAttribute(new XMPAttribute((char *)"persist", m_persist));
    node->AddAttribute(new XMPAttribute((char *)"metronome", m_metronome));
    node->AddAttribute(new XMPAttribute((char *)"autocomplete", m_autocomplete));
    node->AddAttribute(new XMPAttribute((char *)"wrongnotes", m_wrongnotes));
    node->AddAttribute(new XMPAttribute((char *)"requirefret",m_requirefret));
    node->AddAttribute(new XMPAttribute((char *)"tempo", m_tempo));
    
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
