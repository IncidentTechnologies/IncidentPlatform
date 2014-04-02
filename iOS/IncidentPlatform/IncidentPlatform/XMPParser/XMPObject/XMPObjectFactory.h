//
//  XMPObjectFactory.h
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMPNode.h"
#import "XMPObject.h"

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

#import "XMPClip.h"
#import "XMPContent.h"
#import "XMPSampler.h"
#import "XMPBank.h"
#import "XMPInstrument.h"

@interface XMPObjectFactory : NSObject {
    
}

// Will take an XMP node as input and produce an object out of it
+(XMPObject*)MakeXMPObjectFromFilename:(NSString *)XMPFileName;
+(XMPObject*)MakeXMPObject:(XMPNode*)xmpNode;

+(XMPObject*)MakeXMPObjectFromType:(XMP_OBJECT_TYPE)type;

//+(const unsigned int)str2int:(const char *)str withH:(int)h;

@end
