#import "XMPMeasure.h"

@implementation XMPMeasure

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_MEASURE;
    
    CRM([self ConstructMeasure], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructMeasure {
    RESULT r = R_SUCCESS;
    
    [self GetXMPValueOfChild:@"startbeat" withAttribute:@"value"].GetValueDouble(&m_startbeat);
    [self GetXMPValueOfChild:@"beatcount" withAttribute:@"value"].GetValueDouble(&m_beatcount);
    [self GetXMPValueOfChild:@"beatvalue" withAttribute:@"value"].GetValueDouble(&m_beatvalue);
    
Error:
    return r;
}

@end