//
//  GtarControllerInternal.h
//  GtarController
//
//  Created by Marty Greenia on 5/24/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "GtarController.h"

#define GTAR_CONTROLLER_MAX_FIRMWARE_PAGES 60
#define GTAR_CONTROLLER_PAGE_SIZE 1024

@class CoreMidiInterface;

// The delegate is only used for internal use. Normal communications (fret on, fret off, ..)
// only goes to the public observers
@protocol GtarControllerDelegate <NSObject>
@optional
- (void)receivedBatteryStatus:(BOOL)charging;
- (void)receivedBatteryCharge:(unsigned char)percentage;
- (void)receivedFirmwareMajorVersion:(int)majorVersion andMinorVersion:(int)minorVersion;
- (void)receivedFirmwareUpdateProgress:(unsigned char)percentage;
- (void)receivedFirmwareUpdateStatusSucceeded;
- (void)receivedFirmwareUpdateStatusFailed;
@end

typedef SInt32 MIDINotificationMessageID;

typedef enum GTAR_RX_MSG_TYPE
{
    RX_FRET_UP = 0x30,
    RX_FRET_DOWN = 0x31,
    RX_FW_VERSION = 0x32,
    RX_FW_UPDATE_ACK = 0x35,
    RX_BATTERY_STATUS = 0x36,
    RX_BATTERY_CHARGE = 0x37
} gTarRxMsgType;

// This class is an Objective C class that users sends a delegate update when events happen.
// The original serial code still required a poll for input before events could be sent, but the 
// core midi makes this entirely event driven.
@interface GtarController ()
{
    
    //id<GtarControllerDelegate> m_delegate;
    
    CoreMidiInterface * m_coreMidiInterface;    
    
//    GtarControllerEffect m_currentGuitarEffect;
    
    NSMutableArray * m_observerList;
        
    double m_previousPluckTime[GtarStringCount][GtarFretCount];
    
    NSData * m_firmware;
    int m_firmwareCurrentPage;
    BOOL m_firmwareCancelation;
    BOOL m_firmwareUpdating;
    BOOL m_scaleVelocity;
}

- (BOOL)checkNoteInterarrivalTime:(double)time forFret:(GtarFret)fret andString:(GtarString)str;

- (void)logMessage:(NSString*)str atLogLevel:(GtarControllerLogLevel)level;

- (int)getFretFromMidiNote:(int)midiNote andString:(int)str;

- (void)midiConnectionHandler:(BOOL)connected;
- (void)midiCallbackHandler:(char*)data;
- (void)midiCallbackDispatch:(NSDictionary*)dictionary;
- (void)midiCallbackWorkerThread:(NSDictionary*)dictionary;

- (void)notifyObserversGtarFretDown:(NSDictionary*)dictionary;
- (void)notifyObserversGtarFretUp:(NSDictionary*)dictionary;
- (void)notifyObserversGtarNoteOn:(NSDictionary*)dictionary;
- (void)notifyObserversGtarNoteOff:(NSDictionary*)dictionary;
- (void)notifyObserversGtarConnected:(NSDictionary*)dictionary;
- (void)notifyObserversGtarDisconnected:(NSDictionary*)dictionary;

- (void)firmwareResponseHandler:(unsigned char)status;
- (BOOL)sendFirmwarePage:(int)page;

// CC style set LED messages (not async)
//- (RESULT)ccTurnOffAllLeds;
//- (RESULT)ccTurnOffLedAtString:(GtarString)str andFret:(GtarFret)fret;
//- (RESULT)ccTurnOnLedAtString:(GtarString)str andFret:(GtarFret)fret withRed:(char)red andGreen:(char)green andBlue:(char)blue;
//- (RESULT)ccTurnOnLedWithColorMappingAtString:(GtarString)str andFret:(GtarFret)fret;

// Requests
- (BOOL)sendRequestBatteryStatus;
- (BOOL)sendEnableDebug;
- (BOOL)sendDisableDebug;

- (BOOL)sendRequestCertDownload;
- (BOOL)sendRequestFirmwareVersion;
//- (BOOL)sendFirmwarePackagePage:(void*)pBuffer bufferSize:(int)pBuffer_n fwSize:(int)fwSize fwPages:(int)fwPages curPage:(int)curPage withCheckSum:(unsigned char)checkSum;
- (BOOL)sendFirmwareUpdate:(NSData*)firmware;
- (BOOL)sendFirmwareUpdateCancelation;

- (BOOL)sendNoteMessage:(unsigned char)midiVal channel:(unsigned char)channel withVelocity:(unsigned char)midiVel andType:(const char*)type;

@property (nonatomic, assign) id<GtarControllerDelegate> m_delegate;
@property (nonatomic, assign) BOOL m_scaleVelocity;

// We should consider pushing these to the public API, they would probably be useful.
@property (nonatomic, readonly) unsigned char m_firmwareMajorVersion;
@property (nonatomic, readonly) unsigned char m_firmwareMinorVersion;

//@property (nonatomic, readonly) GtarControllerEffect m_currentGuitarEffect;

@end
