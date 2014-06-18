//
//  XMPObject.h
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "dss_list.h"
#import "EHM.h"
#import "XMPValue.h"
#import "XMPNode.h"
#import "XMPTree.h"

using namespace dss;

// XMP Values are types of values that can be put into the XML file format
typedef enum XMPObjectTypes
{
    XMP_OBJECT_TEXT,
    XMP_OBJECT_LEDEVENT,
    XMP_OBJECT_GTARNOTE,
    XMP_OBJECT_COURSE,
    XMP_OBJECT_CHAPTER,
    XMP_OBJECT_SECTION,
    XMP_OBJECT_SECTION_LESSON,
    XMP_OBJECT_LESSON,
    XMP_OBJECT_LESSON_CHAPTER,
    XMP_OBJECT_CLIP,
    XMP_OBJECT_LOOP,
    XMP_OBJECT_INPUT,
    XMP_OBJECT_NARRATIVE,
    XMP_OBJECT_SONG,
    XMP_OBJECT_OBJECT,
    XMP_OBJECT_TRACK,
    XMP_OBJECT_MEASURE,
    XMP_OBJECT_GROUP,
    XMP_OBJECT_NOTE,
    XMP_OBJECT_PROMPT,
    XMP_OBJECT_SAMPLE,
    XMP_OBJECT_DATA,
    XMP_OBJECT_CONTENT,
    XMP_OBJECT_SAMPLER,
    XMP_OBJECT_BANK,
    XMP_OBJECT_INSTRUMENT,
    XMP_OBJECT_INVALID
} XMP_OBJECT_TYPE;

@interface XMPObject : NSObject {
    int m_id;
    
    NSMutableArray *m_contents;
    list<XMPValue*>* m_variables;
    XMP_OBJECT_TYPE m_type;
    XMPNode *m_xmpNode;
    NSString *m_Name;
    
}

-(NSString*)GetName;
-(void)SetName:(NSString*)strName;
-(void)SetType:(XMP_OBJECT_TYPE)type;

-(id) initWithXMPNode:(XMPNode*)xmpNode;

-(RESULT)AddXMPObject:(XMPObject*)xmpObj;
-(RESULT)AddXMPVariable:(XMPValue*)xmpVar;

-(RESULT)RemoveXMPObject:(XMPObject*)xmpObj;
-(RESULT)RemoveXMPVariable:(XMPValue*)xmpVar;

-(RESULT)BuildChildren;

-(bool)ContentHasXMPNode:(XMPNode*)xmpNode;
-(XMPValue*)AddVariableFromXMPNode:(XMPNode*)xmpNode;

-(XMPObject*)GetChildWithName:(NSString *)name;
-(BOOL)HasChildWithName:(NSString *)name;
-(NSString*)GetChildTextWithName:(NSString *)name;

-(XMPAttribute*)GetAttributeWithName:(NSString *)name;
-(BOOL)HasAttributeWithName:(NSString *)name;
-(XMPValue)GetAttributeValueWithName:(NSString *)name;

-(XMPValue)GetXMPValueOfChild:(NSString *)childName withAttribute:(NSString*)attributeName;

//-(XMPNode*)CreateXMPNodeFromObject;
-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;
-(XMPTree*)GetXMPTree;
//-(XMPNode*)GetXMPNode;

-(NSString*)Text;

@end
