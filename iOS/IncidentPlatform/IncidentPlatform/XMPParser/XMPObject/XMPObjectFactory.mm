//
//  XMPObjectFactory.m
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#include "EHM.h"

#import "XMPObjectFactory.h"

#import "XMPTree.h"

#import "XMPText.h"
#import "XMPGtarNote.h"
#import "XMPLEDEvent.h"
#import "XMPLoop.h"
#import "XMPInput.h"
#import "XMPLesson.h"
#import "XMPLessonChapter.h"

#import "XMPTrack.h"
#import "XMPSong.h"
#import "XMPNote.h"
#import "XMPMeasure.h"

#import "XMPSample.h"
#import "XMPData.h"

@implementation XMPObjectFactory

/*
+(const unsigned int)str2int:(const char *)str withH:(int)h {
    return !str[h] ? 5381 : ([XMPObjectFactory str2int:str withH:(h+1)]*33) ^ str[h];
}
*/

+(XMPObject*)MakeXMPObjectFromFilename:(NSString *)XMPFileName {
    RESULT r = R_SUCCESS;
    XMPObject *retObj = NULL;
    XMPTree *tempTree = NULL;
    NSString *pTempFilePath = NULL;
    
    pTempFilePath = [[NSBundle mainBundle] pathForResource:XMPFileName ofType:@"xmp"];
    CNRM(pTempFilePath, "MakeXMPObject: File not found!");
    
    tempTree = new XMPTree((char *)[pTempFilePath UTF8String]);
    //tempTree->PrintXMPTree();
    
    //m_type = XMP_OBJECT_LESSON;
    //m_xmpNode = m_pxmp->GetRootNode();
    
    retObj = [XMPObjectFactory MakeXMPObject:tempTree->GetRootNode()];
    
Error:
    return retObj;
}

// Will take an XMP node as input and produce an object out of it
// This function can recurse
+(XMPObject*)MakeXMPObject:(XMPNode*)xmpNode {
    char *pszName = xmpNode->GetName();
    XMPObject *retObj = NULL;
    
    if(strcmp(pszName, (char*)"text") == 0) {
        retObj = [[XMPText alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"guitarposition") == 0) {
        retObj = [[XMPGtarNote alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"loop") == 0) {
        retObj = [[XMPLoop alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"ledout") == 0) {
        retObj = [[XMPLEDEvent alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"input") == 0) {
        retObj = [[XMPInput alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"lesson") == 0) {
        retObj = [[XMPLesson alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"chapter") == 0) {
        retObj = [[XMPLessonChapter alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"xmp") == 0) {
        retObj = [[XMPObject alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"track") == 0) {
        retObj = [[XMPTrack alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"measure") == 0) {
        retObj = [[XMPMeasure alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"note") == 0) {
        retObj = [[XMPNote alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"song") == 0) {
        retObj = [[XMPSong alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"sample") == 0) {
        retObj = [[XMPSample alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"data") == 0) {
        retObj = [[XMPData alloc] initWithXMPNode:xmpNode];
    }
    else {
        retObj = [[XMPObject alloc] initWithXMPNode:xmpNode];
    }
    /*

    else if(strcmp(pszName, (char*)"song")) {
        
    }
    */
    
    return retObj;
}

@end
