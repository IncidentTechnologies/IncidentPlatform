//
//  XMPSong.m
//  gtarLearn
//

#import "XMPSong.h"

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

@end