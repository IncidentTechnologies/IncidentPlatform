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

#import "XMPCourse.h"
#import "XMPChapter.h"
#import "XMPSection.h"
#import "XMPSectionLesson.h"
#import "XMPLesson.h"
#import "XMPLessonChapter.h"
#import "XMPClip.h"
#import "XMPChord.h"
#import "XMPLoop.h"
#import "XMPNarrative.h"
#import "XMPInput.h"
#import "XMPPrompt.h"
#import "XMPPicture.h"
#import "XMPClear.h"
#import "XMPDelay.h"
#import "XMPDock.h"
#import "XMPUndock.h"

#import "XMPTrack.h"
#import "XMPSong.h"
#import "XMPNote.h"
#import "XMPMeasure.h"

#import "XMPSample.h"
#import "XMPData.h"

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
