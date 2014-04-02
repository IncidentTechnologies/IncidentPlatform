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

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    XMPNode *tempNode = new XMPNode("startbeat", node);
    tempNode->AddAttribute(new XMPAttribute("value", m_startbeat));
    node->AddChild(tempNode);
    
    tempNode = new XMPNode("beatcount", node);
    tempNode->AddAttribute(new XMPAttribute("value", m_beatcount));
    node->AddChild(tempNode);
    
    tempNode = new XMPNode("beatvalue", node);
    tempNode->AddAttribute(new XMPAttribute("value", m_beatvalue));
    node->AddChild(tempNode);
    
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