//
//  CoreMidiInterface.h
//  GtarController
//
//  Created by Marty Greenia on 9/26/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>

#include <CoreMIDI/CoreMIDI.h>
#include <CoreMIDI/MIDINetworkSession.h>

#define GTAR_DEVICE_ID 0x77

typedef enum GTAR_MSG_TYPE
{
    GTAR_MSG_SET_LED = 0x00,
    GTAR_MSG_SET_NOTE_ACTIVE = 0x01,
    GTAR_MSG_SET_FRET_FOLLOW = 0x02,
    GTAR_MSG_REQ_CERT_DOWNLOAD = 0x03,
    GTAR_MSG_REQ_FW_VERSION = 0x04,
    GTAR_MSG_DWLD_FW_PAGE = 0x05,
    GTAR_MSG_DBG_ENABLE = 0x11,
    GTAR_MSG_DBG_DISABLE = 0x12,
    GTAR_MSG_REQ_BAT_STATUS = 0x0E,
    GTAR_MSG_INVALID
} gTarMsgType;

// C-style MIDI callback functions
void MIDIStateChangedHandler(const MIDINotification *message, void *refCon);
void MIDICompletionHander(MIDISysexSendRequest  *request);
void MIDIReadHandler(const MIDIPacketList *pPacketList, void *pReadProcCon, void *pSrcConnCon);

@class GtarController;

@interface CoreMidiInterface : NSObject
{
    
    //GtarController * m_gtarController;
    
    BOOL m_connected;
    
    BOOL m_sourceConnected;
    BOOL m_destinationConnected;
    
    MIDIClientRef m_pMidiClient;
    MIDIPortRef m_pMidiInputPort;
    MIDIPortRef m_pMidiOutputPort;
    
    NSMutableArray * m_sendQueue;
    NSMutableArray * m_midiSources;
    NSMutableArray * m_midiDestinations;
    
}

@property (nonatomic, assign) GtarController * m_gtarController;

@property (nonatomic, readonly) BOOL m_connected;

// Send related functions
- (id)initWithGtarController:(GtarController*)gtarController;

- (void)processSendQueue;

- (BOOL)sendBuffer:(unsigned char*)buffer withLength:(int)bufferLength;
- (BOOL)sendSysExBuffer:(unsigned char*)buffer withLength:(int)bufferLength;

// Effects
- (BOOL)sendSetNoteActiveRed:(unsigned char)red andGreen:(unsigned char) green andBlue:(unsigned char)blue;
- (BOOL)sendSetFretFollowRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue;

// Requests
- (BOOL)sendRequestBatteryStatus;
- (BOOL)sendEnableDebug;
- (BOOL)sendDisableDebug;

- (BOOL)sendRequestCertDownload;
- (BOOL)sendRequestFirmwareVersion;
- (BOOL)sendFirmwarePackagePage:(unsigned char*)page andPageSize:(int)pageSize andFirmwareSize:(int)firmwareSize andPageCount:(int)pageCount andCurrentPage:(int)currentPage andChecksum:(unsigned char)checksum;


- (BOOL)sendCCSetLedStatusFret:(unsigned char)fret andString:(unsigned char)str andRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;
- (BOOL)sendSetLedStateFret:(unsigned char)fret andString:(unsigned char)str andRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;
- (BOOL)sendNoteMessageOnChannel:(unsigned char)channel andMidiValue:(unsigned char)midiVal andMidiVel:(unsigned char)velocity andType:(const char*)type;

// Helpers
- (unsigned char)encodeValueWithRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;

@end