//
//  KeysControllerInternal.h
//  KeysController
//
//  Created by Kate Schnippering on 10/23/14.
//  Copyright (c) 2014 Incident Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "KeysController.h"

#define KEYS_CONTROLLER_MAX_FIRMWARE_PAGES 60
#define KEYS_CONTROLLER_PAGE_SIZE 1024

@class KeysCoreMidiInterface;

// The delegate is only used for internal use. Normal communications (fret on, fret off, ..)
// only goes to the public observers
@protocol KeysControllerDelegate <NSObject>
@optional
- (void)receivedResponse:(unsigned char)response;
- (void)receivedBatteryStatus:(BOOL)charging;
- (void)receivedBatteryCharge:(unsigned char)percentage;
- (void)receivedSerialNumber:(unsigned char *)number;
- (void)receivedFirmwareMajorVersion:(int)majorVersion andMinorVersion:(int)minorVersion;
- (void)receivedFirmwareUpdateProgress:(unsigned char)percentage;
- (void)receivedFirmwareUpdateStatusSucceeded;
- (void)receivedFirmwareUpdateStatusFailed;

- (void)receivedCTMatrixValue:(unsigned char)value row:(unsigned char)row col:(unsigned char)col;
- (void)receivedSensitivityValue:(unsigned char)value string:(unsigned char)str;
- (void)receivedPiezoWindow:(unsigned char)value;

- (void)receivedCommitUserspaceAck:(unsigned char)status;
- (void)receivedResetUserspaceAck:(unsigned char)status;
@end

typedef SInt32 MIDINotificationMessageID;

typedef enum KEYS_RX_MSG_TYPE
{
    RX_KEY_UP = 0x30,
    RX_KEY_DOWN = 0x31,
    RX_FW_VERSION = 0x32,
    RX_FW_UPDATE_ACK = 0x35,
    RX_BATTERY_STATUS = 0x36,
    RX_BATTERY_CHARGE = 0x37,
    //RX_KEY_RANGE_CHANGE = 0x38, // TODO: set
    
    // Piezo stuff
    //RX_GET_PIEZO_CT_MATRIX_ACK = 0x38,
    //RX_GET_PIEZO_SENSITIVITY_ACK = 0x39,
    //RX_GET_PIEZO_WINDOW_ACK = 0x3A,
    
    RX_SERIAL_NUMBER_ACK = 0x3C,
    /*
     RX_GET_PIEZO_TIMEOUT_ACK = 0x3B,
     RX_CALIBRATE_PIEZO_STRING_ACK = 0x3D,
     
     RX_SERIAL_NUMBER_ACK = 0x3C,
     RX_COMMIT_USERSPACE_ACK = 0x3E,
     RX_RESET_USERSPACE_ACK = 0x3F,*/
    RX_INCOMING_INVALID
    
} keysRxMsgType;

// This class is an Objective C class that users sends a delegate update when events happen.
// The original serial code still required a poll for input before events could be sent, but the
// core midi makes this entirely event driven.
@interface KeysController () {
    
    //id<KeysControllerDelegate> m_delegate;
    
    KeysCoreMidiInterface * m_coreMidiInterface;
    
    //    KeysControllerEffect m_currentGuitarEffect;
    
    NSMutableArray * m_observerList;
    
    double m_previousPluckTime[KeysMaxKeyPosition];
    
    NSData * m_firmware;
    int m_firmwareCurrentPage;
    BOOL m_firmwareCancelation;
    BOOL m_firmwareUpdating;
    BOOL m_scaleVelocity;
    
    BOOL m_fPendingMatrixValue;
    unsigned char m_PendingMatrixValueRow;
    unsigned char m_PendingMatrixValueColumn;
    
    BOOL m_fPendingSensString;
    unsigned char m_PendingSensString;
    
    BOOL m_fPendingRequest;
    
    int m_pendingSerialByte;
    unsigned char m_serialNumber[16];
    
    unsigned char m_sensitivity[6];
    unsigned char m_ctmatrix[6][6];
}

- (unsigned char)GetSensitivityString:(unsigned char)str;
- (unsigned char)GetCTMatrixRow:(unsigned char)row Column:(unsigned char)col;

- (BOOL) InitiateSerialNumberRequest;
- (BOOL) InterruptSerialNumberRequest;
- (NSString *) GetSerialNumber;
- (NSString *) GetSerialNumberUpper;
- (NSString *) GetSerialNumberLower;

- (BOOL) SetPendingRequest;
- (BOOL) IsPendingRequest;
- (BOOL) ReleasePendingRequest;

- (BOOL) SetPendingMatrixValueRow:(unsigned char)row col:(unsigned char)col;
- (BOOL) ReleasePendingMatrixValue;
- (BOOL) IsPendingMatrixValue;

- (BOOL) SetPendingSensitivityValueString:(unsigned char)str;
- (BOOL) ReleasePendingSensitivityValue;
- (BOOL) IsPendingSensitivityValue;

- (BOOL)sendRequestCommitUserspace;
- (BOOL)sendRequestResetUserspace;

- (BOOL)checkNoteInterarrivalTime:(double)time forKey:(KeyPosition)key;
- (void)logMessage:(NSString*)str atLogLevel:(KeysControllerLogLevel)level;
- (int)getFretFromMidiNote:(int)midiNote andString:(int)str;

- (void)midiConnectionHandler:(BOOL)connected;
- (void)midiConnectionHandler:(BOOL)connected keysDeviceConnected:(BOOL)keysDeviceConnected;
- (void)midiCallbackHandler:(char*)data;
- (void)midiCallbackDispatch:(NSDictionary*)dictionary;
- (void)midiCallbackWorkerThread:(NSDictionary*)dictionary;

- (void)notifyObserversKeyDown:(NSDictionary*)dictionary;
- (void)notifyObserversKeyUp:(NSDictionary*)dictionary;
- (void)notifyObserversKeysNoteOn:(NSDictionary*)dictionary;
- (void)notifyObserversKeysNoteOff:(NSDictionary*)dictionary;
- (void)notifyObserversKeysConnected:(NSDictionary*)dictionary;
- (void)notifyObserversKeysDisconnected:(NSDictionary*)dictionary;
- (void)notifyObserversKeysRangeChange:(NSDictionary*)dictionary;

- (void)firmwareResponseHandler:(unsigned char)status;
- (BOOL)sendFirmwarePage:(int)page;

// CC style set LED messages (not async)
//- (RESULT)ccTurnOffAllLeds;
//- (RESULT)ccTurnOffLedAtString:(KeysString)str andFret:(KeysFret)fret;
//- (RESULT)ccTurnOnLedAtString:(KeysString)str andFret:(KeysFret)fret withRed:(char)red andGreen:(char)green andBlue:(char)blue;
//- (RESULT)ccTurnOnLedWithColorMappingAtString:(KeysString)str andFret:(KeysFret)fret;

// Requests
- (BOOL)sendRequestBatteryStatus;
- (BOOL)sendRequestSerialNumber;
- (BOOL)sendRequestKeysRange;
- (BOOL)sendEnableDebug;
- (BOOL)sendDisableDebug;

// Piezo Request
- (BOOL)sendRequestPiezoSensitivityString:(unsigned char)str;
- (BOOL)sendRequestPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column;
- (BOOL)sendRequestPiezoWindowIndex:(unsigned char)index;

// Piezo Set Request
- (BOOL)sendPiezoSensitivityString:(unsigned char)str thresh:(unsigned char)thresh;
- (BOOL)sendPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column value:(unsigned char)value;
- (BOOL)sendPiezoWindowIndex:(unsigned char)index value:(unsigned char)value;

- (BOOL)sendRequestCertDownload;
- (BOOL)sendRequestFirmwareVersion;
//- (BOOL)sendFirmwarePackagePage:(void*)pBuffer bufferSize:(int)pBuffer_n fwSize:(int)fwSize fwPages:(int)fwPages curPage:(int)curPage withCheckSum:(unsigned char)checkSum;
- (BOOL)sendFirmwareUpdate:(NSData*)firmware;
- (BOOL)sendFirmwareUpdateCancelation;

- (BOOL)sendNoteMessage:(unsigned char)midiVal channel:(unsigned char)channel withVelocity:(unsigned char)midiVel andType:(const char*)type;

@property (nonatomic, assign) id<KeysControllerDelegate> m_delegate;
@property (nonatomic, assign) BOOL m_scaleVelocity;

// We should consider pushing these to the public API, they would probably be useful.
@property (nonatomic, readonly) unsigned char m_firmwareMajorVersion;
@property (nonatomic, readonly) unsigned char m_firmwareMinorVersion;

//@property (nonatomic, readonly) KeysControllerEffect m_currentGuitarEffect;

@end
