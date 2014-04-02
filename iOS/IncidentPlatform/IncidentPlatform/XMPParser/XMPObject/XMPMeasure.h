//
//  XMPMeasure.h
//  gtarLearn

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@interface XMPMeasure : XMPObject {
    double m_startbeat;
    double m_beatcount;
    double m_beatvalue;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(RESULT)ConstructMeasure;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
