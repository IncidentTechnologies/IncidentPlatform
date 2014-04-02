//
//  XMPSong.m
//  gtarLearn
//

#import "XMPSong.h"
#import "XMPContent.h"
#import "XMPTrack.h"

@implementation XMPSong

using namespace dss;

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_SONG;
    
    CRM([self ConstructSong], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id) initWithSongTitle:(NSString*)title author:(NSString*)author description:(NSString*)description {
    RESULT r = R_SUCCESS;
    
    m_Title = [[NSString alloc] initWithString:title];
    m_Author = [[NSString alloc] initWithString:author];
    m_Description = [[NSString alloc] initWithString:description];
    
    m_xmpNode = NULL;
    
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_SONG;
    m_Name = @"song";
    
    m_XMPContent = [[XMPContent alloc] init];
    [self AddXMPObject:m_XMPContent];
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructSong {
    RESULT r = R_SUCCESS;
    
    m_Description = NULL;
    m_Title = NULL;
    m_Author = NULL;
    m_tempo = 0;
    
    XMPObject *headerObj = NULL;
    if((headerObj = [self GetChildWithName:@"header"]) != NULL ) {
        m_Description = [headerObj GetChildTextWithName:@"description"];
        m_Title = [headerObj GetChildTextWithName:@"title"];
        m_Author = [headerObj GetChildTextWithName:@"author"];
        
        XMPObject *tempoObj = NULL;
        if((tempoObj = [headerObj GetChildWithName:@"tempo"]) != NULL ) {
            XMPValue tempoVal = [tempoObj GetAttributeValueWithName:@"value"];
            
            if(tempoVal.m_ValueType == XMP_VALUE_INTEGER)
                tempoVal.GetValueInt((long int*)(&m_tempo));
            else if(tempoVal.m_ValueType == XMP_VALUE_DOUBLE)
                tempoVal.GetValueDouble(&m_tempo);
        }
    }
    
Error:
    return r;
}

-(RESULT)AddTrack:(XMPTrack*)track {
    if(m_XMPContent != NULL) {
        return [m_XMPContent AddXMPObject:(XMPObject*)track];
    }
    else
        return R_FAIL;
}

-(XMPTrack*)AddNewTrackWithName:(NSString *)name {
    XMPTrack *track = [[XMPTrack alloc] initWithName:name];
    [self AddTrack:track];
    return track;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    XMPNode *headerNode = new XMPNode("header", node);
    node->AddChild(headerNode);
    
    XMPNode *tempNode = new XMPNode("description", headerNode);
    tempNode->AppendContentNode((char *)[m_Description UTF8String]);
    headerNode->AddChild(tempNode);
    
    tempNode = new XMPNode("title", headerNode);
    tempNode->AppendContentNode((char *)[m_Title UTF8String]);
    headerNode->AddChild(tempNode);
    
    tempNode = new XMPNode("author", headerNode);
    tempNode->AppendContentNode((char *)[m_Author UTF8String]);
    headerNode->AddChild(tempNode);
    
    tempNode = new XMPNode("tempo", headerNode);
    tempNode->AddAttribute(new XMPAttribute("value", m_tempo));
    headerNode->AddChild(tempNode);
    
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