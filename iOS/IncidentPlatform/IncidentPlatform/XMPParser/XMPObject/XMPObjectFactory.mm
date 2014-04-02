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
    else if(strcmp(pszName, (char*)"content") == 0) {
        retObj = [[XMPContent alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"sampler") == 0) {
        retObj = [[XMPSampler alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"bank") == 0) {
        retObj = [[XMPBank alloc] initWithXMPNode:xmpNode];
    }
    else if(strcmp(pszName, (char*)"instrument") == 0) {
        retObj = [[XMPInstrument alloc] initWithXMPNode:xmpNode];
    }
    else {
        retObj = [[XMPObject alloc] initWithXMPNode:xmpNode];
        [retObj SetType:XMP_OBJECT_INVALID];
    }
    /*

    else if(strcmp(pszName, (char*)"song")) {
        
    }
    */
    
    return retObj;
}

+(XMPObject*)MakeXMPObjectFromType:(XMP_OBJECT_TYPE)type {
    XMPObject *retObject = NULL;
    
    switch(type) {
        case XMP_OBJECT_TEXT: {
            retObject = [[XMPText alloc] init];
            [retObject SetName:@"text"];
        } break;
            
        case XMP_OBJECT_LEDEVENT: {
            retObject = [[XMPLEDEvent alloc] init];
            [retObject SetName:@"ledout"];
        } break;
            
        case XMP_OBJECT_LOOP: {
            // TODO: retObject = [[XMPLoop alloc] init];
            //[retObject SetName:@"text"];
        } break;
            
        case XMP_OBJECT_GTARNOTE: {
            retObject = [[XMPGtarNote alloc] init];
            [retObject SetName:@"guitarposition"];
        } break;
            
        case XMP_OBJECT_INPUT: {
            retObject = [[XMPInput alloc] init];
            [retObject SetName:@"input"];
        } break;
            
        case XMP_OBJECT_LESSON: {
            retObject = [[XMPLesson alloc] init];
            [retObject SetName:@"lesson"];
        } break;
            
        case XMP_OBJECT_CHAPTER: {
            retObject = [[XMPLessonChapter alloc] init];
            [retObject SetName:@"chapter"];
        } break;
            
        case XMP_OBJECT_SONG: {
            retObject = [[XMPSong alloc] init];
            [retObject SetName:@"song"];
        } break;
            
        case XMP_OBJECT_OBJECT: {
            retObject = [[XMPObject alloc] init];
            [retObject SetName:@"xmp"];
        } break;
            
        case XMP_OBJECT_TRACK: {
            retObject = [[XMPTrack alloc] init];
            [retObject SetName:@"track"];
        } break;
            
        case XMP_OBJECT_MEASURE: {
            retObject = [[XMPMeasure alloc] init];
            [retObject SetName:@"measure"];
        } break;
            
        case XMP_OBJECT_CLIP: {
            retObject = [[XMPClip alloc] init];
            [retObject SetName:@"clip"];
        } break;
            
        case XMP_OBJECT_GROUP: {
            //TODO: retObject = [[XMPGroup alloc] init];
            //[retObject SetName:@"group"];
        } break;
            
        case XMP_OBJECT_NOTE: {
            retObject = [[XMPNote alloc] init];
            [retObject SetName:@"note"];
        } break;
            
        case XMP_OBJECT_SAMPLE: {
            retObject = [[XMPSample alloc] init];
            [retObject SetName:@"sample"];
        } break;
            
        case XMP_OBJECT_DATA: {
            retObject = [[XMPData alloc] init];
            [retObject SetName:@"data"];
        } break;
            
        case XMP_OBJECT_CONTENT: {
            retObject = [[XMPContent alloc] init];
            [retObject SetName:@"content"];
        } break;
            
        case XMP_OBJECT_INSTRUMENT: {
            retObject = [[XMPInstrument alloc] init];
            [retObject SetName:@"instrument"];
        } break;
            
        case XMP_OBJECT_SAMPLER: {
            retObject = [[XMPSampler alloc] init];
            [retObject SetName:@"sampler"];
        } break;
            
        case XMP_OBJECT_BANK: {
            retObject = [[XMPBank alloc] init];
            [retObject SetName:@"bank"];
        } break;
            
        default: {
            retObject = NULL;
        } break;
    }
    
    return retObject;
}

@end
