//
//  KeysController.m
//  KeysController
//
//  Created by Kate Schnippering on 10/23/14.
//  Copyright (c) 2014 Incident Technologies. All rights reserved.
//

#import "KeysControllerInternal.h"
#import "KeysCoreMidiInterface.h"

#import <UIKit/UIKit.h>


#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>

// Returns true if the current process is being debugged (either
// running under the debugger or has a debugger attached post facto).
static bool AmIBeingDebugged(void) {
    int                 junk;
    int                 mib[4];
    struct kinfo_proc   info;
    size_t              size;
    
    // Initialize the flags so that, if sysctl fails for some bizarre
    // reason, we get a predictable result.
    info.kp_proc.p_flag = 0;
    
    // Initialize mib, which tells sysctl the info we want, in this case
    // we're looking for information about a specific process ID.
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();
    
    // Call sysctl.
    size = sizeof(info);
    junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
    assert(junk == 0);
    
    // We're being debugged if the P_TRACED flag is set.
    return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
}

@implementation KeysController

@synthesize info = m_info;
@synthesize connected = m_connected;
@synthesize spoofed = m_spoofed;
@synthesize responseThread = m_responseThread;
@synthesize logLevel = m_logLevel;
@synthesize minimumInterarrivalTime = m_minimumInterarrivalTime;
@synthesize colorMap = m_colorMap;

@synthesize m_delegate;
@synthesize m_firmwareMajorVersion;
@synthesize m_firmwareMinorVersion;
@synthesize m_scaleVelocity;

//@synthesize m_currentGuitarEffect;

- (id)init {
    self = [super init];
    
    if ( self )  {
        memset(m_sensitivity, 0, sizeof(unsigned char) * 6);
        memset(m_ctmatrix, 0, sizeof(unsigned char) * 6 * 6);
        
        m_fPendingMatrixValue = false;
        m_PendingMatrixValueRow = 0;
        m_PendingMatrixValueColumn = 0;
        
        m_fPendingSensString = false;
        m_PendingSensString = 0;
        
        m_fPendingRequest = false;
        
        m_info = @"KeysController v1.0.5";
        
        m_spoofed = NO;
        m_connected = NO;
        
        m_logLevel = KeysControllerLogLevelAll;
        
        m_responseThread = KeysControllerThreadMain;
        
        // Set a default color map
        KeysLedColorMap colorMap;
        
        // Define all the LED Colors
        colorMap.keyColor[0].red = 3;
        colorMap.keyColor[0].green = 0;
        colorMap.keyColor[0].blue = 0;
        
        colorMap.keyColor[1].red = 2;
        colorMap.keyColor[1].green = 1;
        colorMap.keyColor[1].blue = 0;
        
        colorMap.keyColor[2].red = 3;
        colorMap.keyColor[2].green = 3;
        colorMap.keyColor[2].blue = 0;
        
        colorMap.keyColor[3].red = 0;
        colorMap.keyColor[3].green = 3;
        colorMap.keyColor[3].blue = 0;
        
        colorMap.keyColor[4].red = 0;
        colorMap.keyColor[4].green = 0;
        colorMap.keyColor[4].blue = 3;
        
        colorMap.keyColor[5].red = 2;
        colorMap.keyColor[5].green = 0;
        colorMap.keyColor[5].blue = 2;
        
        colorMap.keyColor[6].red = 3;
        colorMap.keyColor[6].green = 0;
        colorMap.keyColor[6].blue = 0;
        
        colorMap.keyColor[7].red = 2;
        colorMap.keyColor[7].green = 1;
        colorMap.keyColor[7].blue = 0;
        
        colorMap.keyColor[8].red = 3;
        colorMap.keyColor[8].green = 3;
        colorMap.keyColor[8].blue = 0;
        
        colorMap.keyColor[9].red = 0;
        colorMap.keyColor[9].green = 3;
        colorMap.keyColor[9].blue = 0;
        
        colorMap.keyColor[10].red = 0;
        colorMap.keyColor[10].green = 0;
        colorMap.keyColor[10].blue = 3;
        
        colorMap.keyColor[11].red = 2;
        colorMap.keyColor[11].green = 0;
        colorMap.keyColor[11].blue = 2;
        
        
        self.colorMap = colorMap;
        
        // Initialize the Serial Number
        [self InterruptSerialNumberRequest];
        
        // Initialize the previous pluck times to zero
        for ( KeyPosition key = 0; key < KeysMaxKeyPosition; key++ )
            m_previousPluckTime[key] = 0;
        
        [self logMessage:@"KeysController initializing"
              atLogLevel:KeysControllerLogLevelInfo];
        
        // Create the midi interface
        m_coreMidiInterface = [[KeysCoreMidiInterface alloc] initWithKeysController:self];
        m_observerList = [[NSMutableArray alloc] init];
        
        if ( m_coreMidiInterface == nil ) {
            [self logMessage:@"KeysCoreMidiInterface failed to init"
                  atLogLevel:KeysControllerLogLevelError];
            
            m_connected = NO;
        }
        
        m_firmwareUpdating = NO;
        m_firmwareCancelation = NO;
        
        m_scaleVelocity = YES;
    }
    
    return self;
}

- (BOOL) SetPendingMatrixValueRow:(unsigned char)row col:(unsigned char)col {
    if(m_fPendingMatrixValue)
        return false;
    
    m_PendingMatrixValueRow = row;
    m_PendingMatrixValueColumn = col;
    m_fPendingMatrixValue = true;
    
    return true;
}


- (BOOL) ReleasePendingMatrixValue {
    if(m_fPendingMatrixValue) {
        m_fPendingMatrixValue = false;
        return true;
    }
    else
        return false;
}

- (BOOL) IsPendingMatrixValue {
    return m_fPendingMatrixValue;
}

- (BOOL) SetPendingSensitivityValueString:(unsigned char)str {
    if(m_fPendingSensString)
        return false;
    
    m_PendingSensString = str;
    m_fPendingSensString = true;
    
    return true;
}

- (BOOL) ReleasePendingSensitivityValue {
    if(m_fPendingSensString) {
        m_fPendingSensString = false;
        return true;
    }
    else
        return false;
}

- (BOOL) IsPendingSensitivityValue {
    return m_fPendingSensString;
}

- (BOOL) SetPendingRequest {
    if(m_fPendingRequest)
        return false;
    
    m_fPendingRequest = true;
    return true;
}

- (BOOL) IsPendingRequest {
    return m_fPendingRequest;
}

- (BOOL) ReleasePendingRequest {
    if(m_fPendingRequest) {
        m_fPendingRequest = false;
        return true;
    }
    else
        return false;
}

+ (KeysController *)sharedInstance {
    static KeysController *sharedSingleton;
    
    @synchronized(self) {
        if (!sharedSingleton) {
            sharedSingleton = [[KeysController alloc] init];
        }
        
        return sharedSingleton;
    }
}

- (void)dealloc {
    
    //[m_coreMidiInterface release];
    
    //[m_observerList release];
    
    //[m_firmware release];
    
    //[super dealloc];
    
}

#pragma mark - External debug functions

- (KeysControllerStatus)debugSpoofConnected {
    
    // Pretend we are connected for debug purposes
    
    [self logMessage:@"Spoofing device connected"
          atLogLevel:KeysControllerLogLevelInfo];
    
    m_connected = YES;
    m_spoofed = YES;
    
    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
    
    [responseDictionary setValue:@"notifyObserversKeysConnected:" forKey:@"Selector"];
    
    [self midiCallbackDispatch:responseDictionary];
    
    //[responseDictionary release];
    
    return KeysControllerStatusOk;
    
}

- (KeysControllerStatus)debugSpoofDisconnected {
    // Pretend we are connected for debug purposes
    
    [self logMessage:@"Spoofing device disconnected"
          atLogLevel:KeysControllerLogLevelInfo];
    
    m_connected = NO;
    m_spoofed = NO;
    
    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
    
    [responseDictionary setValue:@"notifyObserversKeysDisconnected:" forKey:@"Selector"];
    
    [self midiCallbackDispatch:responseDictionary];
    
    //[responseDictionary release];
    
    return KeysControllerStatusOk;
    
}

#pragma mark - Internal Functions

- (BOOL)checkNoteInterarrivalTime:(double)time forKey:(KeyPosition)key {
    
    if ( (time - m_previousPluckTime[key]) >= m_minimumInterarrivalTime ) {
        m_previousPluckTime[key] = time;
        
        return YES;
    }
    else {
        [self logMessage:[NSString stringWithFormat:@"Dropping double-triggered note: %f secs", (time - m_previousPluckTime[key])]
              atLogLevel:KeysControllerLogLevelInfo];
        
        return NO;
    }
    
    return NO;
}

- (void)logMessage:(NSString*)str atLogLevel:(KeysControllerLogLevel)level {
    NSString *outputString = NULL;
    
    if (level <= m_logLevel) {
        switch (level) {
            case KeysControllerLogLevelError: {
                outputString = [[NSString alloc] initWithFormat:@"KeysController: Error: %@", str];
            } break;
                
            case KeysControllerLogLevelWarn: {
                outputString = [[NSString alloc] initWithFormat:@"KeysController: Warning: %@", str];
            } break;
                
            case KeysControllerLogLevelInfo: {
                outputString = [[NSString alloc] initWithFormat:@"KeysController: Info: %@", str];
            } break;
                
            default: {
                outputString = [[NSString alloc] initWithFormat:@"KeysController: %@", str];
            } break;
                
        }
    }
    
    if(AmIBeingDebugged()) {
        NSLog(@"%@", outputString);
    }
#ifdef KEYS_ALERT_LOG
    else {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Keys Log Message"
                                                        message:outputString
                                                       delegate:nil
                                              cancelButtonTitle:@"Thanks Keys!"
                                              otherButtonTitles:nil];
        [alert show];
    }
#endif
}

- (int)getKeyFromMidiNote:(int)midiNote
{
    return midiNote;
}

#pragma mark - Internal MIDI functions

- (void)midiConnectionHandler:(BOOL)connected
{
    
    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
    
    // update the delegate as to what has happened
    if ( connected == true )
    {
        [self logMessage:@"Keys Midi device connected"
              atLogLevel:KeysControllerLogLevelInfo];
        
        m_connected = YES;
        m_spoofed = NO;
        
        [responseDictionary setValue:@"notifyObserversKeysConnected:" forKey:@"Selector"];
    }
    else
    {
        [self logMessage:@"Keys Midi device disconnected"
              atLogLevel:KeysControllerLogLevelInfo];
        
        m_connected = NO;
        m_spoofed = NO;
        
        if ( m_firmwareUpdating == YES )
        {
            // HACK there is a firmware bug. it doesn't ack the last page.
            // It just resets itself.
            if ( m_firmwareCurrentPage == (KEYS_CONTROLLER_MAX_FIRMWARE_PAGES-1) )
            {
                @synchronized ( self )
                {
                    // we are done
                    //[m_firmware release];
                    
                    m_firmware = nil;
                    
                    m_firmwareUpdating = NO;
                    m_firmwareCancelation = NO;
                }
                
                
                if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusSucceeded)] == YES )
                {
                    [m_delegate receivedFirmwareUpdateStatusSucceeded];
                }
                else
                {
                    [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusSucceeded %@", m_delegate]
                          atLogLevel:KeysControllerLogLevelWarn];
                    
                }
            }
            else
            {
                // Otherwise, straight up cancel this.
                [self sendFirmwareUpdateCancelation];
                m_firmwareCancelation = NO;
            }
        }
        
        [responseDictionary setValue:@"notifyObserversKeysDisconnected:" forKey:@"Selector"];
    }
    
    [self midiCallbackDispatch:responseDictionary];
    
    //[responseDictionary release];
}

- (void)midiCallbackHandler:(char*)data
{
    
    unsigned char msgType = (data[0] & 0xF0) >> 4;
    
    double currentTime = [NSDate timeIntervalSinceReferenceDate];
    
    switch ( msgType )
    {
            // This is the Note Off event.
        case 0x8:
        {
            
            unsigned char key = [self getKeyFromMidiNote:data[1]];
            
            NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
            
            [responseDictionary setValue:@"notifyObserversKeysNoteOff:" forKey:@"Selector"];
            [responseDictionary setValue:[[NSNumber alloc] initWithChar:key] forKey:@"Key"];
            
            [self midiCallbackDispatch:responseDictionary];
            
            //[responseDictionary release];
            
        } break;
            
            // This is the Note On event.
        case 0x9:
        {
            
            unsigned char key = [self getKeyFromMidiNote:data[1]];
            unsigned char velocity = data[2];
            
            // Filter out any messages that arrive within a certain time
            if ( [self checkNoteInterarrivalTime:currentTime forKey:key] == YES )
            {
                NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                
                [responseDictionary setValue:@"notifyObserversKeysNoteOn:" forKey:@"Selector"];
                [responseDictionary setValue:[[NSNumber alloc] initWithChar:key] forKey:@"Key"];
                [responseDictionary setValue:[[NSNumber alloc] initWithChar:velocity] forKey:@"Velocity"];
                
                [self midiCallbackDispatch:responseDictionary];
                
                //[responseDictionary release];
            }
            
        } break;
            
            // Control Channel Message
        case 0xB:
        {
            
            unsigned char KeysMsgType = data[1];
            
            switch ( KeysMsgType )
            {
                case RX_KEY_UP:
                {
                    // Key Up
                    unsigned char key = data[2];
                    
                    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                    
                    [responseDictionary setValue:@"notifyObserversKeyUp:" forKey:@"Selector"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:key] forKey:@"Key"];
                    
                    [self midiCallbackDispatch:responseDictionary];
                    
                    //[responseDictionary release];
                    
                } break;
                    
                case RX_KEY_DOWN:
                {
                    // Key Down
                    unsigned char key = data[2];
                    
                    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                    
                    [responseDictionary setValue:@"notifyObserversKeyDown:" forKey:@"Selector"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:key] forKey:@"Key"];
                    
                    [self midiCallbackDispatch:responseDictionary];
                    
                    //[responseDictionary release];
                    
                } break;
                    
                case RX_FW_VERSION:
                {
                    // Current Version Number
                    unsigned char majorVersion = (data[2] & 0xF0) >> 4;
                    unsigned char minorVersion = (data[2] & 0x0F);
                    
                    m_firmwareMajorVersion = majorVersion;
                    m_firmwareMinorVersion = minorVersion;
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedFirmwareMajorVersion:andMinorVersion:)] == YES )
                    {
                        [m_delegate receivedFirmwareMajorVersion:(int)majorVersion andMinorVersion:(int)minorVersion];
                    }
                    else
                    {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareMajorVersion:andMinorVersion: %@", m_delegate]
                              atLogLevel:KeysControllerLogLevelWarn];
                        
                    }
                    
                } break;
                    
                case RX_FW_UPDATE_ACK:
                {
                    // Firmware Ack
                    unsigned char status = data[2];
                    
                    [self firmwareResponseHandler:status];
                    
                } break;
                    
                case RX_BATTERY_STATUS:
                {
                    // Battery status Ack
                    unsigned char battery = data[2];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedBatteryStatus:)] == YES )
                    {
                        [m_delegate receivedBatteryStatus:(BOOL)battery];
                    }
                    else
                    {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxBatteryStatus %@", m_delegate]
                              atLogLevel:KeysControllerLogLevelWarn];
                        
                    }
                    
                } break;
                    
                case RX_BATTERY_CHARGE: {
                    // Battery charge Ack
                    unsigned char percentage = data[2];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedBatteryCharge:)] == YES ) {
                        [m_delegate receivedBatteryCharge:percentage];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxBatteryCharge %@", m_delegate]
                              atLogLevel:KeysControllerLogLevelWarn];
                    }
                } break;
                    
                case RX_KEY_RANGE_CHANGE:
                case KEYS_RANGE_ACK:
                {
                    
                    // Key Down
                    unsigned char keyMin = (data[0] & 0xF);
                    unsigned char keyMax = data[2];
                    
                    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                    
                    [responseDictionary setValue:@"notifyObserversKeysRangeChange:" forKey:@"Selector"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:keyMin] forKey:@"KeyMin"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:keyMax] forKey:@"KeyMax"];
                    
                    [self midiCallbackDispatch:responseDictionary];
                    
                    //[responseDictionary release];
                    
                } break;
                    
                case KEYS_SERIAL_NUMBER_ACK: {
                    unsigned char number = data[2];
                    
                    if(m_pendingSerialByte >= 0 && m_pendingSerialByte < 16)
                        m_serialNumber[m_pendingSerialByte] = number;
                    
                    m_pendingSerialByte += 1;
                    
                    [self ReleasePendingRequest];
                    
                    if(m_pendingSerialByte < 16) {
                        BOOL request = [self sendRequestSerialNumber];
                        
                        if(request == FALSE)
                            [self logMessage:[NSString stringWithFormat:@"Failed to request serial number byte %d", m_pendingSerialByte] atLogLevel:KeysControllerLogLevelError];
                    }
                    else {
                        m_pendingSerialByte = -1;
                        
                        // Delegate callback
                        if ( [m_delegate respondsToSelector:@selector(receivedSerialNumber:)] == YES ) {
                            [m_delegate receivedSerialNumber:m_serialNumber];
                        }
                        else {
                            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxSerialNumber %@", m_delegate]
                                  atLogLevel:KeysControllerLogLevelWarn];
                        }
                    }
                } break;
                    
                case KEYS_COMMIT_USERSPACE_ACK: {
                    unsigned char status = data[2];
                    
                    [self ReleasePendingRequest];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedCommitUserspaceAck:)] == YES ) {
                        [m_delegate receivedCommitUserspaceAck:status];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxCommitUserspaceAck%@", m_delegate]
                              atLogLevel:KeysControllerLogLevelWarn];
                    }
                } break;
                    
                case KEYS_RESET_USERSPACE_ACK: {
                    unsigned char status = data[2];
                    
                    [self ReleasePendingRequest];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedResetUserspaceAck:)] == YES ) {
                        [m_delegate receivedResetUserspaceAck:status];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxResetUserspaceAck%@", m_delegate]
                              atLogLevel:KeysControllerLogLevelWarn];
                    }
                } break;
                    
                default:
                {
                    
                    [self logMessage:[NSString stringWithFormat:@"Unhandled control msg of type 0x%x", KeysMsgType]
                          atLogLevel:KeysControllerLogLevelError];
                    
                } break;
                    
            }
            
        } break;
            
        default:
        {
            
            [self logMessage:[NSString stringWithFormat:@"Unhandled midi msg of type 0x%x", msgType]
                  atLogLevel:KeysControllerLogLevelError];
            
        } break;
    }
    
}

- (unsigned char)GetSensitivityString:(unsigned char)str {
    return m_sensitivity[str];
}

- (unsigned char)GetCTMatrixRow:(unsigned char)row Column:(unsigned char)col {
    return m_ctmatrix[row][col];
}

- (void)midiCallbackDispatch:(NSDictionary*)dictionary
{
    
    // Hold onto the dictionary before we queue up a worker thread.
    // This is primarily important for the async case.
    //[dictionary retain];
    
    if ( m_responseThread == KeysControllerThreadMain )
    {
        // This queues up request asynchronously
        [self performSelectorOnMainThread:@selector(midiCallbackWorkerThread:) withObject:dictionary waitUntilDone:NO];
    }
    else
    {
        [self performSelector:@selector(midiCallbackWorkerThread:) withObject:dictionary];
    }
}

- (void)midiCallbackWorkerThread:(NSDictionary*)dictionary
{
    
    NSString * selectorString = [dictionary objectForKey:@"Selector"];
    
    //    NSLog(@"Selector string: %@", selectorString);
    //    NSLog(@"Dictionary %@", dictionary);
    
    SEL selector = NSSelectorFromString(selectorString);
    
    [self performSelector:selector withObject:dictionary];
    
    // Release the dictionary that was retained in the dispatcher.
    // This is primarily important for the async case.
    //[dictionary release];
    
}

#pragma Internal notification functions
// Notifying observers
- (void)notifyObserversKeyDown:(NSDictionary*)dictionary
{
    
    NSNumber * keyNumber = [dictionary objectForKey:@"Key"];
    
    KeyPosition keysPosition = [keyNumber integerValue];
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(keyDown:)] == YES )
        {
            [observer keyDown:keysPosition];
        }
    }
}

- (void)notifyObserversKeyUp:(NSDictionary*)dictionary
{
    
    NSNumber * keyNumber = [dictionary objectForKey:@"Key"];
    
    KeyPosition keysPosition = [keyNumber integerValue];
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(keyUp:)] == YES )
        {
            [observer keyUp:keysPosition];
        }
    }
}

- (void)notifyObserversKeysNoteOn:(NSDictionary*)dictionary
{
    
    NSNumber * keyNumber = [dictionary objectForKey:@"Key"];
    NSNumber * velocityNumber = [dictionary objectForKey:@"Velocity"];
    
    KeysPress keysPress;
    
    if ( m_scaleVelocity == YES )
    {
        float buggedMax = KeysMaxPressVelocity * 0.6;
        float adjusted = [velocityNumber floatValue] / buggedMax * KeysMaxPressVelocity;
        keysPress.velocity = MIN(KeysMaxPressVelocity, adjusted);
    }
    else
    {
        keysPress.velocity = [velocityNumber integerValue];
    }
    
    keysPress.position = [keyNumber integerValue];
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(keysNoteOn:)] == YES )
        {
            [observer keysNoteOn:keysPress];
        }
    }
}

- (void)notifyObserversKeysNoteOff:(NSDictionary*)dictionary
{
    
    NSNumber * keyNumber = [dictionary objectForKey:@"Key"];
    
    KeyPosition keysPosition = [keyNumber intValue];
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(keysNoteOff:)] == YES )
        {
            [observer keysNoteOff:keysPosition];
        }
    }
}

- (void)notifyObserversKeysConnected:(NSDictionary*)dictionary
{
    
    // The dictionary will be nil and unused
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(keysConnected)] == YES )
        {
            [observer keysConnected];
        }
    }
}

- (void)notifyObserversKeysDisconnected:(NSDictionary*)dictionary
{
    
    // The dictionary will be nil and unused
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(keysDisconnected)] == YES )
        {
            [observer keysDisconnected];
        }
    }
}

- (void)notifyObserversKeysRangeChange:(NSDictionary *)dictionary
{
    NSNumber * keyMin = [dictionary objectForKey:@"KeyMin"];
    NSNumber * keyMax = [dictionary objectForKey:@"KeyMax"];
    
    KeysRange keysRange;
    keysRange.keyMin = (KeyPosition)keyMin;
    keysRange.keyMax = (KeyPosition)keyMax;
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(keysRangeChange:)] == YES )
        {
            [observer keysRangeChange:keysRange];
        }
    }
}

#pragma mark - Internal firmware handling

- (void)firmwareResponseHandler:(unsigned char)status
{
    
    if ( status == 0x00 )
    {
        
        // success
        if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateProgress:)] == YES )
        {
            [m_delegate receivedFirmwareUpdateProgress:((m_firmwareCurrentPage+1)*100)/KEYS_CONTROLLER_MAX_FIRMWARE_PAGES];
        }
        else
        {
            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateProgress: %@", m_delegate]
                  atLogLevel:KeysControllerLogLevelWarn];
            
        }
        
        if ( m_firmwareCancelation == YES || m_firmwareUpdating == NO)
        {
            
            @synchronized ( self )
            {
                m_firmwareUpdating = NO;
                m_firmwareCancelation = NO;
            }
            
            // Cancel the transfer, abort now.
            [self logMessage:[NSString stringWithFormat:@"Firmware update canceled, aborting transfer"]
                  atLogLevel:KeysControllerLogLevelInfo];
            
            //            if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusFailed)] == YES )
            //            {
            //                [m_delegate receivedFirmwareUpdateStatusFailed];
            //            }
            //            else
            //            {
            //                [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusFailed %@", m_delegate]
            //                      atLogLevel:KeysControllerLogLevelWarn];
            //
            //            }
            
        }
        else if ( m_firmwareCurrentPage < KEYS_CONTROLLER_MAX_FIRMWARE_PAGES )
        {
            // send the next page
            m_firmwareCurrentPage++;
            
            [self sendFirmwarePage:m_firmwareCurrentPage];
        }
        else
        {
            
            @synchronized ( self )
            {
                // we are done
                //[m_firmware release];
                
                m_firmware = nil;
                
                m_firmwareUpdating = NO;
                m_firmwareCancelation = NO;
            }
            
            if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusSucceeded)] == YES )
            {
                [m_delegate receivedFirmwareUpdateStatusSucceeded];
            }
            else
            {
                [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusSucceeded %@", m_delegate]
                      atLogLevel:KeysControllerLogLevelWarn];
                
            }
            
        }
        
    }
    else
    {
        // failure
        switch ( status )
        {
            case 0x01:
            {
                [self logMessage:[NSString stringWithFormat:@"Firmware update invalid parameter"]
                      atLogLevel:KeysControllerLogLevelError];
            } break;
                
            case 0x02:
            {
                [self logMessage:[NSString stringWithFormat:@"Firmware update out of memory"]
                      atLogLevel:KeysControllerLogLevelError];
            } break;
                
            case 0x03:
            {
                [self logMessage:[NSString stringWithFormat:@"Firmware update will not fit in flash"]
                      atLogLevel:KeysControllerLogLevelError];
            } break;
                
            default:
            case 0x04:
            {
                [self logMessage:[NSString stringWithFormat:@"Firmware update unknown error"]
                      atLogLevel:KeysControllerLogLevelError];
            } break;
        }
        
        @synchronized ( self )
        {
            //[m_firmware release];
            
            m_firmware = nil;
            
            m_firmwareUpdating = NO;
            m_firmwareCancelation = NO;
        }
        
        if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusFailed)] == YES )
        {
            [m_delegate receivedFirmwareUpdateStatusFailed];
        }
        else
        {
            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusFailed %@", m_delegate]
                  atLogLevel:KeysControllerLogLevelWarn];
            
        }
        
    }
    
    
}

- (BOOL)sendFirmwarePage:(int)page
{
    
    if ( [m_firmware length] == 0 ) {
        [self logMessage:@"SendFirmwarePAge: Firmware length 0" atLogLevel:KeysControllerLogLevelWarn];
        return false;
    }
    
    unsigned char * firmwareBytes = (unsigned char *)[m_firmware bytes];
    unsigned char checksum = 0;
    
    // Sum all the bytes
    for ( int i = 0; i < KEYS_CONTROLLER_PAGE_SIZE; i++ )
        checksum += firmwareBytes[(page*KEYS_CONTROLLER_PAGE_SIZE) + i];
    
    BOOL result = [m_coreMidiInterface sendFirmwarePackagePage:(firmwareBytes + (page * KEYS_CONTROLLER_PAGE_SIZE))
                                                   andPageSize:KEYS_CONTROLLER_PAGE_SIZE
                                               andFirmwareSize:[m_firmware length]
                                                  andPageCount:KEYS_CONTROLLER_MAX_FIRMWARE_PAGES
                                                andCurrentPage:page
                                                   andChecksum:checksum];
    
    return result;
}

#pragma mark - Serial Number management

- (BOOL) InitiateSerialNumberRequest {
    // Initiate sequence, delegate called when we've gotten through all the bytes
    if(m_pendingSerialByte == -1) {
        // First initialize all to zero
        [self InterruptSerialNumberRequest];
        
        m_pendingSerialByte = 0;
        [self sendRequestSerialNumber];
        
        return TRUE;
    }
    else
        return FALSE;
}

- (BOOL) InterruptSerialNumberRequest {
    for(int i = 0; i < 16; i++)
        m_serialNumber[i] = 0;
    
    m_pendingSerialByte = -1;
    return TRUE;
}

- (NSString *) GetSerialNumber {
    NSString * hexString = @"";
    BOOL isAllZero = YES;
    
    // Build a hex string
    for(int i = 0; i < 16; i++){
        hexString = [hexString stringByAppendingFormat:@"%x",m_serialNumber[i]];
        
        if(m_serialNumber[i] > 0){
            isAllZero = NO;
        }
    }
    
    if(isAllZero){
        //return nil;
    }
    
    // Convert to long string
    NSScanner * pScanner = [NSScanner scannerWithString:hexString];
    
    unsigned long long iValue;
    [pScanner scanHexLongLong:&iValue];
    
    NSString * serialString = [NSString stringWithFormat:@"%016qu",iValue];
    
    return serialString;
}

- (NSString *) GetSerialNumberLower {
    NSString * serialNumber = [self GetSerialNumber];
    return [serialNumber substringFromIndex:8];
}

- (NSString *) GetSerialNumberUpper {
    NSString * serialNumber = [self GetSerialNumber];
    return [serialNumber substringToIndex:7];
}

#pragma mark - Observer management

// Observers should ultimately replace the delegate paradigm we have going.
// I didn't want to rip out the delegate functionality since we use it all
// over the place, but new stuff will need to use the observer model.
- (KeysControllerStatus)addObserver:(id<KeysControllerObserver>)observer
{
    
    // We don't want the observers to be retained to prevent circular dependendcies.
    // We need to make sure we dont retain the object.
    NSValue * nonretainedObserver = [NSValue valueWithNonretainedObject:observer];
    
    if ( observer == nil )
    {
        [self logMessage:@"Added observer is nil"
              atLogLevel:KeysControllerLogLevelWarn];
        
        return KeysControllerStatusOk;
    }
    
    //
    // We don't want to add the same observer twice.
    //
    if ( [m_observerList containsObject:nonretainedObserver] == NO )
    {
        
        [self logMessage:@"Added observer"
              atLogLevel:KeysControllerLogLevelInfo];
        
        [m_observerList addObject:nonretainedObserver];
        
        // If the guitar is already connected, we should let this new guy know.
        if ( m_connected == YES && [observer respondsToSelector:@selector(keysConnected)] == YES )
        {
            [observer keysConnected];
        }
        
    }
    else
    {
        [self logMessage:@"Added observer is already observing"
              atLogLevel:KeysControllerLogLevelWarn];
        
    }
    
    return KeysControllerStatusOk;
    
}

- (KeysControllerStatus)removeObserver:(id<KeysControllerObserver>)observer
{
    
    // We don't want the observers to be retained to prevent circular dependendcies.
    // We need to make sure we dont retain the object.
    NSValue * nonretainedObserver = [NSValue valueWithNonretainedObject:observer];
    
    if ( nonretainedObserver == nil )
    {
        [self logMessage:@"Removed observer is nil"
              atLogLevel:KeysControllerLogLevelWarn];
        
        return KeysControllerStatusOk;
    }
    
    if ( [m_observerList containsObject:nonretainedObserver] == YES )
    {
        
        [self logMessage:@"Removed observer"
              atLogLevel:KeysControllerLogLevelInfo];
        
        [m_observerList removeObject:nonretainedObserver];
    }
    else
    {
        [self logMessage:@"Removed observer is not observing"
              atLogLevel:KeysControllerLogLevelWarn];
    }
    
    return KeysControllerStatusOk;
    
}

#pragma mark - LED manipulation

- (KeysControllerStatus)turnOffAllLeds
{
    
    [self logMessage:@"Turning off all LEDs"
          atLogLevel:KeysControllerLogLevelInfo];
    
    KeysControllerStatus status = KeysControllerStatusOk;
    
    if ( m_spoofed == YES )
    {
        
        [self logMessage:@"turnOffAllLeds: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        
        status = KeysControllerStatusOk;
        
    }
    else if ( m_connected == NO )
    {
        
        [self logMessage:@"turnOffAllLeds: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        
        status = KeysControllerStatusNotConnected;
        
    }
    else if ( m_coreMidiInterface == nil )
    {
        
        [self logMessage:@"turnOffAllLeds: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        
        status = KeysControllerStatusError;
        
    }
    else
    {
        
        BOOL result = [m_coreMidiInterface sendSetLedStateKey:0 andRed:0 andGreen:0 andBlue:0 andMessage:0];
        
        if ( result == NO )
        {
            [self logMessage:@"turnOffAllLeds: Setting LED state failed"
                  atLogLevel:KeysControllerLogLevelError];
            
            status = KeysControllerStatusError;
        }
        
    }
    
    return status;
    
}

- (KeysControllerStatus)turnOffLedAtPosition:(KeyPosition)position
{
    KeysControllerStatus status = KeysControllerStatusOk;
    
    if ( m_spoofed == YES )
    {
        
        [self logMessage:@"turnOffLedAtString: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        
        status = KeysControllerStatusOk;
        
    }
    else if ( m_connected == NO )
    {
        
        [self logMessage:@"turnOffLedAtString: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        
        status = KeysControllerStatusNotConnected;
        
    }
    else if ( m_coreMidiInterface == nil )
    {
        
        [self logMessage:@"turnOffLedAtString: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        
        status = KeysControllerStatusError;
        
    }
    else
    {
        
        BOOL result = [m_coreMidiInterface sendSetLedStateKey:position andRed:0 andGreen:0 andBlue:0 andMessage:0];
        
        if ( result == NO )
        {
            [self logMessage:@"turnOffLedAtString: Setting LED state failed"
                  atLogLevel:KeysControllerLogLevelError];
            
            status = KeysControllerStatusError;
        }
        
    }
    
    return status;
    
}

- (KeysControllerStatus)turnOnLedAtPosition:(KeyPosition)position withColor:(KeysLedColor)color {
    
    KeysLedIntensity red = color.red;
    KeysLedIntensity green = color.green;
    KeysLedIntensity blue = color.blue;
    
    KeysControllerStatus status = KeysControllerStatusOk;
    
    if ( m_spoofed == YES ) {
        [self logMessage:@"turnOnLedAtKey: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        
        status = KeysControllerStatusOk;
    }
    else if ( m_connected == NO ){
        [self logMessage:@"turnOnLedAtKey: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        
        status = KeysControllerStatusNotConnected;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"turnOnLedAtKey: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        
        status = KeysControllerStatusError;
    }
    else {
        BOOL result = [m_coreMidiInterface sendSetLedStateKey:position andRed:red andGreen:green andBlue:blue andMessage:0];
        
        if ( result == NO ) {
            [self logMessage:@"turnOnLedAtKey: Setting LED state failed"
                  atLogLevel:KeysControllerLogLevelError];
            
            status = KeysControllerStatusError;
        }
    }
    
    return status;
    
}

- (KeysControllerStatus)turnOnLedAtPositionWithColorMap:(KeyPosition)position
{
    KeysControllerStatus status = KeysControllerStatusOk;
    
    if ( m_spoofed == YES )
    {
        
        [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        
        status = KeysControllerStatusOk;
        
    }
    else if ( m_connected == NO )
    {
        
        [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        
        status = KeysControllerStatusNotConnected;
        
    }
    else if ( m_coreMidiInterface == nil )
    {
        
        [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        
        status = KeysControllerStatusError;
        
    }
    else
    {
        
       /* if( oct == 0 )
        {
            
            // TODO: how does this apply to Keys?
            
            // turn on all strings using their specified color mapping
            
            [self logMessage:@"Turn on all strings using their specified color mapping - sendSetLedStateKey:andOctave:andRed..." atLogLevel:KeysControllerLogLevelAll];
            
            for( int oct = KeysMinOctave; oct < KeysMaxOctave; oct++ )
            {
                
                BOOL result = [m_coreMidiInterface sendSetLedStateKey:key andOctave:(oct+1)
                                                                andRed:m_colorMap.keyColor[key].red
                                                              andGreen:m_colorMap.keyColor[key].green
                                                               andBlue:m_colorMap.keyColor[key].blue
                                                            andMessage:0];
                
                if ( result == NO )
                {
                    [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: Setting LED state failed"
                          atLogLevel:KeysControllerLogLevelError];
                    
                    status = KeysControllerStatusError;
                    break;
                }
            }
        }
        else
        {*/
            KeyPosition key = position % KeysPerOctaveCount;
            
            // subtract one to zero-base the string
            BOOL result = [m_coreMidiInterface sendSetLedStateKey:position
                                                            andRed:m_colorMap.keyColor[key].red
                                                          andGreen:m_colorMap.keyColor[key].green
                                                           andBlue:m_colorMap.keyColor[key].blue
                                                        andMessage:0];
            
            if ( result == NO )
            {
                [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: Setting LED state failed"
                      atLogLevel:KeysControllerLogLevelError];
                
                status = KeysControllerStatusError;
            }
        //}
        
    }
    
    return status;
    
}


- (BOOL)sendNoteMessage:(unsigned char)midiVal
                channel:(unsigned char)channel
           withVelocity:(unsigned char)midiVel
                andType:(const char *)type
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"sendNoteMsg: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"sendNoteMsg: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"sendNoteMsg: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendNoteMessageOnChannel:channel
                                                   andMidiValue:midiVal
                                                     andMidiVel:midiVel
                                                        andType:type];
    
    if ( result == NO )
    {
        [self logMessage:@"sendNoteMsg: SendNoteMsg failed!"
              atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

#if 0
/*
 // Commenting out unsued code for now
 
 #pragma mark - CC Style LED Manipulation
 
 - (RESULT)ccTurnOffAllLeds
 {
 
 if ( m_spoofed == YES )
 {
 [self logMessage:@"ccTurnOffAllLeds: Connection spoofed, no-op"
 atLogLevel:KeysControllerLogLevelInfo];
 return NO;
 }
 else if ( m_connected == NO )
 {
 [self logMessage:@"ccTurnOffAllLeds: Not connected"
 atLogLevel:KeysControllerLogLevelWarn];
 return NO;
 }
 else if ( m_coreMidiInterface == nil )
 {
 [self logMessage:@"ccTurnOffAllLeds: KeysCoreMidiInterface is invalid"
 atLogLevel:KeysControllerLogLevelError];
 return NO;
 }
 
 RESULT result = m_coreMidiInterface->SendCCSetLEDState(0,0,0,0,0,0);
 
 if ( CHECK_ERR( result ) )
 {
 [self logMessage:@"ccTurnOffAllLeds: SendSetLEDState failed"
 atLogLevel:KeysControllerLogLevelError];
 }
 
 return result;
 }
 
 - (RESULT)ccTurnOffLedAtString:(KeysString)str andFret:(KeysFret)fret
 {
 
 if ( m_spoofed == YES )
 {
 [self logMessage:@"ccTurnOffLedAtStr: Connection spoofed, no-op"
 atLogLevel:KeysControllerLogLevelInfo];
 return R_ERROR;
 }
 else if ( m_connected == NO )
 {
 [self logMessage:@"ccTurnOffLedAtStr: Not connected"
 atLogLevel:KeysControllerLogLevelWarn];
 return R_ERROR;
 }
 else if ( m_coreMidiInterface == nil )
 {
 [self logMessage:@"ccTurnOffLedAtStr: KeysCoreMidiInterface is invalid"
 atLogLevel:KeysControllerLogLevelError];
 return R_ERROR;
 }
 
 RESULT result = m_coreMidiInterface->SendCCSetLEDState(fret, str,0,0,0,0);
 
 if ( CHECK_ERR( result ) )
 {
 [self logMessage:@"ccTurnOffLedAtStr: SendSetLEDState failed"
 atLogLevel:KeysControllerLogLevelError];
 }
 
 return result;
 }
 
 - (RESULT)ccTurnOnLedAtString:(KeysString)str andFret:(KeysFret)fret withRed:(char)red andGreen:(char)green andBlue:(char)blue
 {
 
 if ( m_spoofed == YES )
 {
 [self logMessage:@"ccTurnOnLedAtStr: Connection spoofed, no-op"
 atLogLevel:KeysControllerLogLevelInfo];
 return R_ERROR;
 }
 else if ( m_connected == NO )
 {
 [self logMessage:@"ccTurnOnLedAtStr: Not connected"
 atLogLevel:KeysControllerLogLevelWarn];
 return R_ERROR;
 }
 else if ( m_coreMidiInterface == nil )
 {
 [self logMessage:@"ccTurnOnLedAtStr: KeysCoreMidiInterface is invalid"
 atLogLevel:KeysControllerLogLevelError];
 return R_ERROR;
 }
 
 RESULT result = m_coreMidiInterface->SendCCSetLEDState(fret, str, red, green, blue, 0);
 
 if ( CHECK_ERR( result ) )
 {
 [self logMessage:@"ccTurnOnLedAtStr: SendSetLEDState failed"
 atLogLevel:KeysControllerLogLevelError];
 }
 
 return result;
 }
 
 - (RESULT)ccTurnOnLedWithColorMappingAtString:(KeysString)str andFret:(KeysFret)fret
 {
 
 if ( m_spoofed == YES )
 {
 [self logMessage:@"ccTurnOnLedWithColorMappingAtString: Connection spoofed, no-op"
 atLogLevel:KeysControllerLogLevelInfo];
 return R_ERROR;
 }
 else if ( m_connected == NO )
 {
 [self logMessage:@"ccTurnOnLedWithColorMappingAtString: Not connected"
 atLogLevel:KeysControllerLogLevelWarn];
 return R_ERROR;
 }
 else if ( m_coreMidiInterface == nil )
 {
 [self logMessage:@"ccTurnOnLedWithColorMappingAtString: KeysCoreMidiInterface is invalid"
 atLogLevel:KeysControllerLogLevelError];
 return R_ERROR;
 }
 
 RESULT result = m_coreMidiInterface->SendCCSetLEDState(fret, str,
 m_stringColorMapping[str-1][0],
 m_stringColorMapping[str-1][1],
 m_stringColorMapping[str-1][2],
 0);
 
 if ( CHECK_ERR( result ) )
 {
 [self logMessage:@"ccTurnOnLedAtStr: SendSetLEDState failed"
 atLogLevel:KeysControllerLogLevelError];
 }
 
 return result;
 }
 */
#endif


- (BOOL)sendRequestCommitUserspace {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendRequestCommitUserspace: Connection spoofed, no-op" atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendRequestCommitUserspace: Not connected" atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendRequestCommitUserspace: KeysCoreMidiInterface is invalid" atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestCommitUserspace];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendRequestCommitUserspace: sendRequestCommitUserspace failed" atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestResetUserspace {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendRequestResetUserspace: Connection spoofed, no-op" atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendRequestResetUserspace: Not connected" atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendRequestResetUserspace: KeysCoreMidiInterface is invalid" atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestResetUserspace];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendRequestResetUserspace: sendRequestResetUserspace failed" atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

#pragma mark - Requests

- (BOOL)sendRequestKeysRange
{
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestKeysRange: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestKeysRange: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestKeysRange: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestKeysRange];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestKeysRange: SendRequestKeysRange failed"
              atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendRequestBatteryStatus
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestBatteryStatus: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestBatteryStatus: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestBatteryStatus: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestBatteryStatus];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestBatteryStatus: SendRequestBatteryStatus failed"
              atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendRequestSerialNumber {
    if( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestSerialNumber: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestSerialNumber: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestSerialNumber: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestSerialNumber:m_pendingSerialByte];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestSerialNumber: SendRequestBatteryStatus failed"
              atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendEnableDebug
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendEnableDebug: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendEnableDebug: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendEnableDebug: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendEnableDebug];
    
    if ( result == NO )
    {
        [self logMessage:@"SendEnableDebug: SendEnableDebug failed"
              atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
    
}

- (BOOL)sendDisableDebug
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendDisableDebug: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendDisableDebug: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendDisableDebug: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendDisableDebug];
    
    if ( result == NO )
    {
        [self logMessage:@"SendDisableDebug: SendDisableDebug failed"
              atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
    
}

- (BOOL)sendRequestCertDownload
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestCertDownload: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestCertDownload: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestCertDownload: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestCertDownload];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestCertDownload: SendDisableDebug failed"
              atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
    
}

- (BOOL)sendRequestFirmwareVersion
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestFirmwareVersion: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestFirmwareVersion: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestFirmwareVersion: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestFirmwareVersion];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestFirmwareVersion: SendRequestFirmwareVersion failed"
              atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}


- (BOOL)sendFirmwareUpdate:(NSData*)firmware {
    
    if ( m_spoofed == YES ) {
        [self logMessage:@"SendFirmwareUpdate: Connection spoofed, no-op"
              atLogLevel:KeysControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"SendFirmwareUpdate: Not connected"
              atLogLevel:KeysControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"SendFirmwareUpdate: KeysCoreMidiInterface is invalid"
              atLogLevel:KeysControllerLogLevelError];
        return NO;
    }
    
    @synchronized ( self ) {
        
        if ( m_firmwareCancelation == YES ) {
            [self logMessage:@"SendFirmwareUpdate: Cancellation in progress"
                  atLogLevel:KeysControllerLogLevelWarn];
            return NO;
        }
        
        if ( m_firmwareUpdating == YES ) {
            [self logMessage:@"SendFirmwareUpdate: Firmware update in progress"
                  atLogLevel:KeysControllerLogLevelWarn];
            return NO;
        }
        
        //[m_firmware release];
        m_firmware = [[NSData alloc] initWithData:firmware];
        
        m_firmwareCurrentPage = 0;
        m_firmwareUpdating = YES;
        BOOL result = [self sendFirmwarePage:m_firmwareCurrentPage];
        
        if ( result == NO ) {
            [self logMessage:@"SendFirmwareUpdate: Failed to send firmware package page"
                  atLogLevel:KeysControllerLogLevelError];
        }
        
        return result;
    }
}

- (BOOL)sendFirmwareUpdateCancelation {
    @synchronized ( self ) {
        // Only cancel if we are updating a firmware
        if ( m_firmwareUpdating == YES ) {
            
            [self logMessage:@"Canceling firmware update"
                  atLogLevel:KeysControllerLogLevelInfo];
            
            m_firmwareCancelation = YES;
            m_firmwareUpdating = NO;
            
            //[m_firmware release];
            
            m_firmware = nil;
            
            if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusFailed)] == YES ) {
                [m_delegate receivedFirmwareUpdateStatusFailed];
            }
            else {
                [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusFailed %@", m_delegate]
                      atLogLevel:KeysControllerLogLevelWarn];
            }
        }
        else {
            [self logMessage:@"Cannot cancel, no firmware update in progress"
                  atLogLevel:KeysControllerLogLevelWarn];
        }
    }
    return YES;
}

#pragma mark - Effect handling

- (KeysControllerStatus)turnOffAllEffects
{
    
    KeysControllerStatus status = KeysControllerStatusOk;
    
    [self logMessage:@"Turning off all effects"
          atLogLevel:KeysControllerLogLevelInfo];
    
    BOOL result;
    
    result = [m_coreMidiInterface sendSetKeyFollowRed:0 andGreen:0 andBlue:0];
    
    if ( result == NO )
    {
        [self logMessage:@"turnOffAllEffects: SendSetFretFollow failed!"
              atLogLevel:KeysControllerLogLevelError];
        
        status = KeysControllerStatusError;
    }
    
    result = [m_coreMidiInterface sendSetNoteActiveRed:0 andGreen:0 andBlue:0];
    
    if ( result == NO )
    {
        [self logMessage:@"turnOffAllEffects: SendSetNoteActive failed!"
              atLogLevel:KeysControllerLogLevelError];
        
        status = KeysControllerStatusError;
    }
    
    return status;
    
}

- (KeysControllerStatus)turnOnEffect:(KeysControllerEffect)effect withColor:(KeysLedColor)color {
    char red = color.red;
    char green = color.green;
    char blue = color.blue;
    
    KeysControllerStatus status = KeysControllerStatusOk;
    
    switch ( effect )  {
        case KeysControllerEffectKeyFollow:
        {
            // Enable FF mode
            BOOL result = [m_coreMidiInterface sendSetKeyFollowRed:red andGreen:green andBlue:blue];
            
            if ( result == NO )
            {
                [self logMessage:@"setEffectColor: SendSetFretFollow failed!"
                      atLogLevel:KeysControllerLogLevelError];
                
                status = KeysControllerStatusError;
            }
            
        } break;
            
        case KeysControllerEffectNoteActive:
        {
            // Enable NA mode
            BOOL result = [m_coreMidiInterface sendSetNoteActiveRed:red andGreen:green andBlue:blue];
            
            if ( result == NO )
            {
                [self logMessage:@"setEffectColor: SendSetNoteActive failed!"
                      atLogLevel:KeysControllerLogLevelError];
                
                status = KeysControllerStatusError;
            }
            
        } break;
            
        case KeysControllerEffectKeyFollowNoteActive:
        {
            // Enable FF mode
            BOOL result;
            
            result = [m_coreMidiInterface sendSetKeyFollowRed:red andGreen:green andBlue:blue];
            
            if ( result == NO )
            {
                [self logMessage:@"setEffectColor: SendSetFretFollow failed!"
                      atLogLevel:KeysControllerLogLevelError];
                
                status = KeysControllerStatusError;
            }
            
            // Enable NA mode
            result = [m_coreMidiInterface sendSetNoteActiveRed:red andGreen:green andBlue:blue];
            
            if ( result == NO )
            {
                [self logMessage:@"setEffectColor: SendSetNoteActive failed!"
                      atLogLevel:KeysControllerLogLevelError];
                
                status = KeysControllerStatusError;
            }
            
        } break;
            
        case KeysControllerEffectNone:
        default:
        {
            
            // nothing
            
        } break;
    }
    
    return status;
    
}

@end
