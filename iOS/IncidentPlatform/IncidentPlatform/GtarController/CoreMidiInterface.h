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

typedef enum {
    GTAR_FRET_DOWN = 0x31,
    GTAR_FRET_UP = 0x30,
    GTAR_CURRENT_FIRMWARE_VERSION = 0x32,
    GTAR_FIRMWARE_ACK = 0x35,
    GTAR_BATTERY_STATUS_ACK = 0x36,
    GTAR_BATTERY_CHARGE_ACK = 0x37,
    
    // Piezo stuff
    GTAR_GET_PIEZO_CT_MATRIX_ACK = 0x38,
	GTAR_GET_PIEZO_SENSITIVITY_ACK = 0x39,
	GTAR_GET_PIEZO_WINDOW_ACK = 0x3A,
    GTAR_GET_PIEZO_TIMEOUT_ACK = 0x3B,
    GTAR_CALIBRATE_PIEZO_STRING_ACK = 0x3D,
    
	GTAR_SERIAL_NUMBER_ACK = 0x3C,
    
    GTAR_COMMIT_USERSPACE_ACK = 0x3E,
    GTAR_RESET_USERSPACE_ACK = 0x3F,
    GTAR_INCOMING_INVALID
} GTAR_INCOMING_MESSAGES;

typedef enum {
    GTAR_SET_LED = 0x00,
    GTAR_SET_LED_EX = 0x0A,
    
    GTAR_SET_NOTE_ACTIVE = 0x01,
    GTAR_SET_FRET_FOLLOW = 0x02,
    
    GTAR_SET_SLIDE_STATE = 0x0B,
    
    GTAR_FORCE_REDOWNLOAD_CERT = 0x03,
    GTAR_REQUEST_BATTERY_STATUS = 0x0E,
    GTAR_REQUEST_FIRMWARE_VERSION = 0x04,
    GTAR_REQUEST_SERIAL_NUMBER = 0x18,
    
    GTAR_DOWNLOAD_NEW_FIRMWARE = 0x05,
    GTAR_EXECUTE_FIRMWARE_UPGRADE = 0x06,
    
    GTAR_ENABLE_VELOCITY = 0x0C,
    GTAR_DISABLE_VELOCITY = 0x0D,
    
    GTAR_ENABLE_DEBUG = 0x11,
    GTAR_DISABLE_DEBUG = 0x12,
    
    GTAR_SET_PIEZO_STATE = 0x07,
    GTAR_SET_PIEZO_THRESH = 0x08,
    GTAR_SET_SMART_PICK_THRESH = 0x09,
    GTAR_SET_FRET_BOARD_THRESH = 0x0F,
    
    // Piezo Stuff
    GTAR_SET_PIEZO_CT_MATRIX = 0x13,
	GTAR_GET_PIEZO_CT_MATRIX = 0x14,
	GTAR_SET_PIEZO_SENSITIVITY = 0x15,
	GTAR_GET_PIEZO_SENSITIVITY = 0x16,
	GTAR_SET_PIEZO_WINDOW = 0x17,
	GTAR_GET_PIEZO_WINDOW = 0x10,
	GTAR_GET_PIEZO_TIMEOUT = 0x1B,
	GTAR_SET_PIEZO_TIMEOUT = 0x1C,
    
    GTAR_CALIBRATE_PIEZO_STRING = 0x0B,
    
    GTAR_SET_SERIAL_NUMBER = 0x19,
    
    GTAR_COMMIT_USERSPACE = 0x1D,
    GTAR_RESET_USERSPACE = 0x1E,
    
    GTAR_INVALID
} GTAR_MESSAGE;

// C-style MIDI callback functions
void MIDIStateChangedHandler(const MIDINotification *message, void *refCon);
void MIDICompletionHander(MIDISysexSendRequest  *request);
void MIDIReadHandler(const MIDIPacketList *pPacketList, void *pReadProcCon, void *pSrcConnCon);

@class GtarController;

@interface CoreMidiInterface : NSObject {
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
    
    BOOL m_fPendingMatrixValue;
    unsigned char m_PendingMatrixValueRow;
    unsigned char m_PendingMatrixValueColumn;
    
    BOOL m_fPendingSensString;
    unsigned char m_PendingSensString;
}

@property (nonatomic, assign) GtarController * m_gtarController;

@property (nonatomic, readonly) BOOL m_connected;

// Send related functions
- (id)initWithGtarController:(GtarController*)gtarController;

- (void)processSendQueue;

- (BOOL)sendBuffer:(unsigned char*)buffer withLength:(int)bufferLength;
- (BOOL)sendSysExBuffer:(unsigned char*)buffer withLength:(int)bufferLength;
- (BOOL)sendSyncSysExBuffer:(unsigned char*)buffer withLength:(int)bufferLength;

// Effects
- (BOOL)sendSetNoteActiveRed:(unsigned char)red andGreen:(unsigned char) green andBlue:(unsigned char)blue;
- (BOOL)sendSetFretFollowRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue;

// Requests
- (BOOL)sendRequestBatteryStatus;
- (BOOL)sendEnableDebug;
- (BOOL)sendDisableDebug;

- (BOOL)sendRequestCommitUserspace;
- (BOOL)sendRequestResetUserspace;

// Piezo
- (BOOL)sendPiezoSensitivityString:(unsigned char)str thresh:(unsigned char)thresh;
- (BOOL)sendPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column value:(unsigned char)value;
- (BOOL)sendPiezoWindowIndex:(unsigned char)index value:(unsigned char)value;

- (BOOL)sendRequestPiezoSensitivityString:(unsigned char)str;
- (BOOL)sendRequestPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column;
- (BOOL)sendRequestPiezoWindowIndex:(unsigned char)index;

- (BOOL)sendRequestCertDownload;
- (BOOL)sendRequestFirmwareVersion;
- (BOOL)sendFirmwarePackagePage:(unsigned char*)page andPageSize:(int)pageSize andFirmwareSize:(int)firmwareSize andPageCount:(int)pageCount andCurrentPage:(int)currentPage andChecksum:(unsigned char)checksum;


- (BOOL)sendCCSetLedStatusFret:(unsigned char)fret andString:(unsigned char)str andRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;
- (BOOL)sendSetLedStateFret:(unsigned char)fret andString:(unsigned char)str andRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;
- (BOOL)sendNoteMessageOnChannel:(unsigned char)channel andMidiValue:(unsigned char)midiVal andMidiVel:(unsigned char)velocity andType:(const char*)type;

// Helpers
- (unsigned char)encodeValueWithRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;

@end