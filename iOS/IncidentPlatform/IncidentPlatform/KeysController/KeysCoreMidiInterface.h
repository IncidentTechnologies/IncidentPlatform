//
//  KeysCoreMidiInterface.h
//  KeysController
//
//  Created by Kate Schnippering on 10/23/14.
//  Copyright (c) 2014 Incident Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>

#include <CoreMIDI/CoreMIDI.h>
#include <CoreMIDI/MIDINetworkSession.h>

#define KEYS_DEVICE_ID 0x77

typedef enum KEYS_MSG_TYPE
{
    KEYS_MSG_SET_LED = 0x00,
    KEYS_MSG_SET_NOTE_ACTIVE = 0x01,
    KEYS_MSG_SET_FRET_FOLLOW = 0x02,
    KEYS_MSG_REQ_CERT_DOWNLOAD = 0x03,
    KEYS_MSG_REQ_FW_VERSION = 0x04,
    KEYS_MSG_DWLD_FW_PAGE = 0x05,
    KEYS_MSG_DBG_ENABLE = 0x11,
    KEYS_MSG_DBG_DISABLE = 0x12,
    KEYS_MSG_REQ_BAT_STATUS = 0x0E,
    KEYS_MSG_INVALID
} keysMsgType;

typedef enum {
    KEYS_FRET_DOWN = 0x31,
    KEYS_FRET_UP = 0x30,
    KEYS_CURRENT_FIRMWARE_VERSION = 0x32,
    KEYS_FIRMWARE_ACK = 0x35,
    KEYS_BATTERY_STATUS_ACK = 0x36,
    KEYS_BATTERY_CHARGE_ACK = 0x37,
    //KEYS_RANGE_ACK = 0x40, // TODO: SET
    
    // Piezo stuff
    KEYS_GET_PIEZO_CT_MATRIX_ACK = 0x38,
    KEYS_GET_PIEZO_SENSITIVITY_ACK = 0x39,
    KEYS_GET_PIEZO_WINDOW_ACK = 0x3A,
    KEYS_GET_PIEZO_TIMEOUT_ACK = 0x3B,
    KEYS_CALIBRATE_PIEZO_STRING_ACK = 0x3D,
    
    KEYS_SERIAL_NUMBER_ACK = 0x3C,
    
    KEYS_COMMIT_USERSPACE_ACK = 0x3E,
    KEYS_RESET_USERSPACE_ACK = 0x3F,
    KEYS_INCOMING_INVALID
} KEYS_INCOMING_MESSAGES;

typedef enum {
    KEYS_SET_LED = 0x00,
    KEYS_SET_LED_EX = 0x0A,
    
    KEYS_SET_NOTE_ACTIVE = 0x01,
    KEYS_SET_KEY_FOLLOW = 0x02,
    
    KEYS_SET_SLIDE_STATE = 0x0B,
    
    KEYS_FORCE_REDOWNLOAD_CERT = 0x03,
    KEYS_REQUEST_BATTERY_STATUS = 0x0E,
    KEYS_REQUEST_FIRMWARE_VERSION = 0x04,
    KEYS_REQUEST_SERIAL_NUMBER = 0x18,
    KEYS_REQUEST_KEYS_RANGE = 0x19,
    
    KEYS_DOWNLOAD_NEW_FIRMWARE = 0x05,
    KEYS_EXECUTE_FIRMWARE_UPGRADE = 0x06,
    
    KEYS_ENABLE_VELOCITY = 0x0C,
    KEYS_DISABLE_VELOCITY = 0x0D,
    
    KEYS_ENABLE_DEBUG = 0x11,
    KEYS_DISABLE_DEBUG = 0x12,
    
    KEYS_SET_PIEZO_STATE = 0x07,
    KEYS_SET_PIEZO_THRESH = 0x08,
    KEYS_SET_SMART_PICK_THRESH = 0x09,
    KEYS_SET_FRET_BOARD_THRESH = 0x0F,
    
    // Piezo Stuff
    KEYS_SET_PIEZO_CT_MATRIX = 0x13,
    KEYS_GET_PIEZO_CT_MATRIX = 0x14,
    KEYS_SET_PIEZO_SENSITIVITY = 0x15,
    KEYS_GET_PIEZO_SENSITIVITY = 0x16,
    KEYS_SET_PIEZO_WINDOW = 0x17,
    KEYS_GET_PIEZO_WINDOW = 0x10,
    KEYS_GET_PIEZO_TIMEOUT = 0x1B,
    KEYS_SET_PIEZO_TIMEOUT = 0x1C,
    
    KEYS_CALIBRATE_PIEZO_STRING = 0x0B,
    
    KEYS_SET_SERIAL_NUMBER = 0x19,
    
    KEYS_COMMIT_USERSPACE = 0x1D,
    KEYS_RESET_USERSPACE = 0x1E,
    
    KEYS_INVALID
} KEYS_MESSAGE;

// C-style MIDI callback functions
void KeysMIDIStateChangedHandler(const MIDINotification *message, void *refCon);
void KeysMIDICompletionHander(MIDISysexSendRequest  *request);
void KeysMIDIReadHandler(const MIDIPacketList *pPacketList, void *pReadProcCon, void *pSrcConnCon);

@class KeysController;

@interface KeysCoreMidiInterface : NSObject {
    
    BOOL m_connected;
    BOOL m_keysConnected;
    
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

@property (nonatomic, assign) KeysController * m_keysController;

@property (nonatomic, readonly) BOOL m_connected;

@property (nonatomic, readonly) BOOL m_keysConnected;

// Send related functions
- (id)initWithKeysController:(KeysController*)keysController;

- (void)processSendQueue;

- (BOOL)sendBuffer:(unsigned char*)buffer withLength:(int)bufferLength;
- (BOOL)sendSysExBuffer:(unsigned char*)buffer withLength:(int)bufferLength;
- (BOOL)sendSyncSysExBuffer:(unsigned char*)buffer withLength:(int)bufferLength;

// Effects
- (BOOL)sendSetNoteActiveRed:(unsigned char)red andGreen:(unsigned char) green andBlue:(unsigned char)blue;
- (BOOL)sendSetKeyFollowRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue;

// Requests
- (BOOL)sendRequestBatteryStatus;
- (BOOL)sendRequestSerialNumber:(unsigned char)byte;
- (BOOL)sendRequestKeysRange;
- (BOOL)sendEnableDebug;
- (BOOL)sendDisableDebug;

- (BOOL)sendRequestCommitUserspace;
- (BOOL)sendRequestResetUserspace;

// Piezo
/*
- (BOOL)sendPiezoSensitivityString:(unsigned char)str thresh:(unsigned char)thresh;
- (BOOL)sendPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column value:(unsigned char)value;
- (BOOL)sendPiezoWindowIndex:(unsigned char)index value:(unsigned char)value;

- (BOOL)sendRequestPiezoSensitivityString:(unsigned char)str;
- (BOOL)sendRequestPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column;
- (BOOL)sendRequestPiezoWindowIndex:(unsigned char)index;
*/

- (BOOL)sendRequestCertDownload;
- (BOOL)sendRequestFirmwareVersion;
- (BOOL)sendFirmwarePackagePage:(unsigned char*)page andPageSize:(int)pageSize andFirmwareSize:(int)firmwareSize andPageCount:(int)pageCount andCurrentPage:(int)currentPage andChecksum:(unsigned char)checksum;


//- (BOOL)sendCCSetLedStatusKey:(unsigned char)key andRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;
- (BOOL)sendSetLedStateKey:(unsigned char)key andRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;
- (BOOL)sendNoteMessageOnChannel:(unsigned char)channel andMidiValue:(unsigned char)midiVal andMidiVel:(unsigned char)velocity andType:(const char*)type;

// Helpers
- (unsigned char)encodeValueWithRed:(unsigned char)red andGreen:(unsigned char)green andBlue:(unsigned char)blue andMessage:(unsigned char)message;

@end