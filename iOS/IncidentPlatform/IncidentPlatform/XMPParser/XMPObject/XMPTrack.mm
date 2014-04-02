#import "XMPTrack.h"
#import "XMPClip.h"
#import "XMPInstrument.h"

@implementation XMPTrack

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_TRACK;
    
    CRM([self ConstructTrack], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id) initWithName:(NSString*)name {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_TRACK;
    m_Name = @"track";
    
    m_TrackName = [[NSString alloc] initWithString:name];
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructTrack {
    RESULT r = R_SUCCESS;
    
    XMPValue trackNameVal = [self GetAttributeValueWithName:@"name"];
    if(trackNameVal.m_ValueType != XMP_VALUE_INVALID)
        m_Name = [[NSString alloc] initWithCString:trackNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_Name = @"";
    
    m_TrackName = m_Name;
    
Error:
    return r;
}

-(RESULT)AddClip:(XMPClip*)clip {
    [m_contents addObject:clip];
    return R_SUCCESS;
}

-(XMPClip*)AddNewClipWithName:(NSString*)name Start:(double)startbeat End:(double)endbeat {
    XMPClip *clip = [[XMPClip alloc] initWithName:name Start:startbeat End:endbeat];
    [self AddClip:clip];
    return clip;
}

-(RESULT)AddInstrument:(XMPInstrument*)instrument {
    return [self AddXMPObject:instrument];
}

-(XMPInstrument*)AddNewInstrumentWithName:(NSString*)name {
    XMPInstrument *instrument = [[XMPInstrument alloc] initWithName:name];
    [self AddInstrument:instrument];
    return instrument;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    node->AddAttribute(new XMPAttribute("name", (char*)[m_TrackName UTF8String]));
    
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