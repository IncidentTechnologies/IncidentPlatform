//
//  GtarController.m
//  GtarController
//
//  Created by Marty Greenia on 5/24/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//

#import "GtarControllerInternal.h"
#import "CoreMidiInterface.h"

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

@implementation GtarController

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
        
        m_info = @"GtarController v1.0.5";
        
        m_spoofed = NO;
        m_connected = NO;
        
        m_logLevel = GtarControllerLogLevelAll;
        
        m_responseThread = GtarControllerThreadMain;
        
        // Set a default color map
        GtarLedColorMap colorMap;
        
        colorMap.stringColor[0].red = 3;
        colorMap.stringColor[0].green = 0;
        colorMap.stringColor[0].blue = 0;
        
        colorMap.stringColor[1].red = 2;
        colorMap.stringColor[1].green = 1;
        colorMap.stringColor[1].blue = 0;
        
        colorMap.stringColor[2].red = 3;
        colorMap.stringColor[2].green = 3;
        colorMap.stringColor[2].blue = 0;
        
        colorMap.stringColor[3].red = 0;
        colorMap.stringColor[3].green = 3;
        colorMap.stringColor[3].blue = 0;
        
        colorMap.stringColor[4].red = 0;
        colorMap.stringColor[4].green = 0;
        colorMap.stringColor[4].blue = 3;
        
        colorMap.stringColor[5].red = 2;
        colorMap.stringColor[5].green = 0;
        colorMap.stringColor[5].blue = 2;
        
        self.colorMap = colorMap;
        
        // Initialize the Serial Number
        [self InterruptSerialNumberRequest];
        
        // Initialize the previous pluck times to zero
        for ( GtarString str = 0; str < GtarStringCount; str++ )
            for ( GtarFret fret = 0; fret < GtarFretCount; fret++ )
                m_previousPluckTime[str][fret] = 0;
        
        [self logMessage:@"GtarController initializing"
              atLogLevel:GtarControllerLogLevelInfo];

        // Create the midi interface
        m_coreMidiInterface = [[CoreMidiInterface alloc] initWithGtarController:self];
        m_observerList = [[NSMutableArray alloc] init];
        
        if ( m_coreMidiInterface == nil ) {
            [self logMessage:@"CoreMidiInterface failed to init"
                  atLogLevel:GtarControllerLogLevelError];
            
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

+ (GtarController *)sharedInstance {
    static GtarController *sharedSingleton;
    
    @synchronized(self) {
        if (!sharedSingleton) {
            sharedSingleton = [[GtarController alloc] init];
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

- (GtarControllerStatus)debugSpoofConnected {
    
    // Pretend we are connected for debug purposes
    
    [self logMessage:@"Spoofing device connected"
          atLogLevel:GtarControllerLogLevelInfo];
    
    m_connected = YES;
    m_spoofed = YES;
    
    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
    
    [responseDictionary setValue:@"notifyObserversGtarConnected:" forKey:@"Selector"];
    
    [self midiCallbackDispatch:responseDictionary];
    
    //[responseDictionary release];
    
    return GtarControllerStatusOk;
    
}

- (GtarControllerStatus)debugSpoofDisconnected {
    // Pretend we are connected for debug purposes
    
    [self logMessage:@"Spoofing device disconnected"
          atLogLevel:GtarControllerLogLevelInfo];
    
    m_connected = NO;
    m_spoofed = NO;
    
    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
    
    [responseDictionary setValue:@"notifyObserversGtarDisconnected:" forKey:@"Selector"];
    
    [self midiCallbackDispatch:responseDictionary];
    
    //[responseDictionary release];
    
    return GtarControllerStatusOk;
    
}

#pragma mark - Internal Functions

- (BOOL)checkNoteInterarrivalTime:(double)time forFret:(GtarFret)fret andString:(GtarString)str {
    // zero base the string
    str--;
    
    if ( (time - m_previousPluckTime[str][fret]) >= m_minimumInterarrivalTime ) {
        m_previousPluckTime[str][fret] = time;
        
        return YES;
    }
    else {
        [self logMessage:[NSString stringWithFormat:@"Dropping double-triggered note: %f secs", (time - m_previousPluckTime[str][fret])]
              atLogLevel:GtarControllerLogLevelInfo];
        
        return NO;
    }
}

- (void)logMessage:(NSString*)str atLogLevel:(GtarControllerLogLevel)level {
    NSString *outputString = NULL;
    
    if (level <= m_logLevel) {
        switch (level) {
            case GtarControllerLogLevelError: {
                outputString = [[NSString alloc] initWithFormat:@"GtarController: Error: %@", str];
            } break;
                
            case GtarControllerLogLevelWarn: {
                outputString = [[NSString alloc] initWithFormat:@"GtarController: Warning: %@", str];
            } break;
                
            case GtarControllerLogLevelInfo: {
                outputString = [[NSString alloc] initWithFormat:@"GtarController: Info: %@", str];
            } break;
                
            default: {
                outputString = [[NSString alloc] initWithFormat:@"GtarController: %@", str];
            } break;
                
        }
    }
    
    if(AmIBeingDebugged()) {
        NSLog(@"%@", outputString);
    }
#ifdef GTAR_ALERT_LOG
    else {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Gtar Log Message"
                                                        message:outputString
                                                       delegate:nil
                                              cancelButtonTitle:@"Thanks gTar!"
                                              otherButtonTitles:nil];
        [alert show];
    }
#endif
}

- (int)getFretFromMidiNote:(int)midiNote andString:(int)str {
    
    if ( str < 0 || str > 5 )
        return -1;
    
    int fret = midiNote - (40 + 5 * str);
    
    if (str > 3 )
        fret += 1;
    
    return fret;
}

#pragma mark - Internal MIDI functions

- (void)midiConnectionHandler:(BOOL)connected
{
    
    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
    
    // update the delegate as to what has happened
    if ( connected == true ) {
        [self logMessage:@"Gtar Midi device connected"
              atLogLevel:GtarControllerLogLevelInfo];
        
        m_connected = YES;
        m_spoofed = NO;
        
        [responseDictionary setValue:@"notifyObserversGtarConnected:" forKey:@"Selector"];
    }
    else {
        [self logMessage:@"Gtar Midi device disconnected"
              atLogLevel:GtarControllerLogLevelInfo];
        
        m_connected = NO;
        m_spoofed = NO;
        
        if ( m_firmwareUpdating == YES )
        {
            // HACK there is a firmware bug. it doesn't ack the last page.
            // It just resets itself.
            if ( m_firmwareCurrentPage == (GTAR_CONTROLLER_MAX_FIRMWARE_PAGES-1) )
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
                          atLogLevel:GtarControllerLogLevelWarn];
                    
                }
            }
            else
            {
                // Otherwise, straight up cancel this.
                [self sendFirmwareUpdateCancelation];
                m_firmwareCancelation = NO;
            }
        }
        
        [responseDictionary setValue:@"notifyObserversGtarDisconnected:" forKey:@"Selector"];
    }
    
    [self midiCallbackDispatch:responseDictionary];
    
    //[responseDictionary release];
}

- (void)midiCallbackHandler:(char*)data {
    
    unsigned char msgType = (data[0] & 0xF0) >> 4;
    unsigned char str = (data[0] & 0xF);
    double currentTime = [NSDate timeIntervalSinceReferenceDate];
    
    // For debugging
    //NSLog(@"rx 0x%x msg", msgType);
    
    switch (msgType) {
        case 0x8: {
            // This is the Note Off event.
            
            unsigned char fret = [self getFretFromMidiNote:data[1] andString:(str-1)];
            
            NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
            
            [responseDictionary setValue:@"notifyObserversGtarNoteOff:" forKey:@"Selector"];
            [responseDictionary setValue:[[NSNumber alloc] initWithChar:fret] forKey:@"Fret"];
            [responseDictionary setValue:[[NSNumber alloc] initWithChar:str] forKey:@"String"];
            
            [self midiCallbackDispatch:responseDictionary];
            
            //[responseDictionary release];
            
        } break;
            
        // This is the Note On event.
        case 0x9: {
            
            unsigned char fret = [self getFretFromMidiNote:data[1] andString:(str-1)];
            unsigned char velocity = data[2];
            
            // Filter out any messages that arrive within a certain time
            if ( [self checkNoteInterarrivalTime:currentTime forFret:fret andString:str] == YES )
            {
                NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                
                [responseDictionary setValue:@"notifyObserversGtarNoteOn:" forKey:@"Selector"];
                [responseDictionary setValue:[[NSNumber alloc] initWithChar:fret] forKey:@"Fret"];
                [responseDictionary setValue:[[NSNumber alloc] initWithChar:str] forKey:@"String"];
                [responseDictionary setValue:[[NSNumber alloc] initWithChar:velocity] forKey:@"Velocity"];
                
                [self midiCallbackDispatch:responseDictionary];
                
                //[responseDictionary release];
            }
            
        } break;
            
            // Control Channel Message
        case 0xB: {
            unsigned char gTarMsgType = data[1];
            
            switch (gTarMsgType) {
                case RX_FRET_UP: {
                    // Fret Up
                    unsigned char fret = data[2];
                    
                    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                    
                    [responseDictionary setValue:@"notifyObserversGtarFretUp:" forKey:@"Selector"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:fret] forKey:@"Fret"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:str] forKey:@"String"];
                    
                    [self midiCallbackDispatch:responseDictionary];
                    
                    //[responseDictionary release];
                    
                } break;
                    
                case RX_FRET_DOWN: {
                    // Fret Down
                    unsigned char fret = data[2];
                    
                    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                    
                    [responseDictionary setValue:@"notifyObserversGtarFretDown:" forKey:@"Selector"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:fret] forKey:@"Fret"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:str] forKey:@"String"];
                    
                    [self midiCallbackDispatch:responseDictionary];
                    
                    //[responseDictionary release];
                    
                } break;
                    
                case RX_FW_VERSION: {
                    // Current Version Number
                    unsigned char majorVersion = (data[2] & 0xF0) >> 4;
                    unsigned char minorVersion = (data[2] & 0x0F);
                    
                    m_firmwareMajorVersion = majorVersion;
                    m_firmwareMinorVersion = minorVersion;
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedFirmwareMajorVersion:andMinorVersion:)] == YES ) {
                        [m_delegate receivedFirmwareMajorVersion:(int)majorVersion andMinorVersion:(int)minorVersion];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareMajorVersion:andMinorVersion: %@", m_delegate]
                              atLogLevel:GtarControllerLogLevelWarn];
                        
                    }

                } break;
                    
                case RX_FW_UPDATE_ACK: {
                    // Firmware Ack
                    unsigned char status = data[2];
                    [self firmwareResponseHandler:status];
                } break;
                    
                case RX_PIEZO_FW_UPDATE_ACK: {
                    // Piezo Firmware Ack
                    unsigned char status = data[2];
                    [self piezoFirmwareResponseHandler:status];
                } break;
                    
                case RX_BATTERY_STATUS: {
                    // Battery status Ack
                    unsigned char battery = data[2];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedBatteryStatus:)] == YES ) {
                        [m_delegate receivedBatteryStatus:(BOOL)battery];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxBatteryStatus %@", m_delegate]
                              atLogLevel:GtarControllerLogLevelWarn];
                        
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
                              atLogLevel:GtarControllerLogLevelWarn];
                    }
                } break;
                    
                case RX_GET_PIEZO_CT_MATRIX_ACK: {
                    unsigned char ratio = data[2];
                    unsigned char row = m_PendingMatrixValueRow;
                    unsigned char col = m_PendingMatrixValueColumn;
                    
                    [self ReleasePendingMatrixValue];
                    
                    m_ctmatrix[row][col] = ratio;
                    if ( [m_delegate respondsToSelector:@selector(receivedCTMatrixValue:row:col:)] == YES ) {
                        [m_delegate receivedCTMatrixValue:ratio row:row col:col];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxCTMatrixRatio %@", m_delegate]
                              atLogLevel:GtarControllerLogLevelWarn];
                    }
                } break;
                
                    // TODO
                case RX_GET_PIEZO_SENSITIVITY_ACK:{
                    unsigned char value = data[2];
                    unsigned char string = m_PendingSensString;
                    
                    [self ReleasePendingSensitivityValue];
                    
                    m_sensitivity[str] = value;
                    if ( [m_delegate respondsToSelector:@selector(receivedSensitivityValue:string:)] == YES ) {
                        [m_delegate receivedSensitivityValue:value string:string];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxSensitivityValue %@", m_delegate]
                              atLogLevel:GtarControllerLogLevelWarn];
                    }
                } break;
                    
                case RX_GET_PIEZO_WINDOW_ACK: {
                    unsigned char window = data[2];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedPiezoWindow:)] == YES ) {
                        [m_delegate receivedPiezoWindow:window];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxPiezoWindow %@", m_delegate]
                              atLogLevel:GtarControllerLogLevelWarn];
                    }
                } break;
                    
                case GTAR_SERIAL_NUMBER_ACK: {
                    unsigned char number = data[2];
                
                    if(m_pendingSerialByte >= 0 && m_pendingSerialByte < 16)
                        m_serialNumber[m_pendingSerialByte] = number;
                
                    m_pendingSerialByte += 1;
                    
                    [self ReleasePendingRequest];
                    
                    if(m_pendingSerialByte < 16) {
                        BOOL request = [self sendRequestSerialNumber];
                        
                        if(request == FALSE)
                            [self logMessage:[NSString stringWithFormat:@"Failed to request serial number byte %d", m_pendingSerialByte] atLogLevel:GtarControllerLogLevelError];
                    }
                    else {
                        m_pendingSerialByte = -1;
                        
                        // Delegate callback
                        if ( [m_delegate respondsToSelector:@selector(receivedSerialNumber:)] == YES ) {
                            [m_delegate receivedSerialNumber:m_serialNumber];
                        }
                        else {
                            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxSerialNumber %@", m_delegate]
                                  atLogLevel:GtarControllerLogLevelWarn];
                        }
                    }
                } break;
                    
                case GTAR_COMMIT_USERSPACE_ACK: {
                    unsigned char status = data[2];
                    
                    [self ReleasePendingRequest];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedCommitUserspaceAck:)] == YES ) {
                        [m_delegate receivedCommitUserspaceAck:status];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxCommitUserspaceAck%@", m_delegate]
                              atLogLevel:GtarControllerLogLevelWarn];
                    }
                } break;
                    
                case GTAR_RESET_USERSPACE_ACK: {
                    unsigned char status = data[2];
                    
                    [self ReleasePendingRequest];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedResetUserspaceAck:)] == YES ) {
                        [m_delegate receivedResetUserspaceAck:status];
                    }
                    else {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxResetUserspaceAck%@", m_delegate]
                              atLogLevel:GtarControllerLogLevelWarn];
                    }
                } break;
                    
                default:
                {
                    
                    [self logMessage:[NSString stringWithFormat:@"Unhandled control msg of type 0x%x", gTarMsgType]
                          atLogLevel:GtarControllerLogLevelError];
                    
                } break;

            }
            
        } break;
            
        default:
        {
            
            [self logMessage:[NSString stringWithFormat:@"Unhandled midi msg of type 0x%x", msgType]
                  atLogLevel:GtarControllerLogLevelError];
            
        } break;
    }
    
}

- (unsigned char)GetSensitivityString:(unsigned char)str {
    return m_sensitivity[str];
}

- (unsigned char)GetCTMatrixRow:(unsigned char)row Column:(unsigned char)col {
    return m_ctmatrix[row][col];
}

- (void)midiCallbackDispatch:(NSDictionary*)dictionary {
    
    // Hold onto the dictionary before we queue up a worker thread.
    // This is primarily important for the async case.
    //[dictionary retain];

    if ( m_responseThread == GtarControllerThreadMain ) {
        // This queues up request asynchronously
        [self performSelectorOnMainThread:@selector(midiCallbackWorkerThread:) withObject:dictionary waitUntilDone:NO];
    }
    else {
        [self performSelector:@selector(midiCallbackWorkerThread:) withObject:dictionary];
    }
}

- (void)midiCallbackWorkerThread:(NSDictionary*)dictionary {
    
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
- (void)notifyObserversGtarFretDown:(NSDictionary*)dictionary {
    
    NSNumber * fretNumber = [dictionary objectForKey:@"Fret"];
    NSNumber * stringNumber = [dictionary objectForKey:@"String"];
    
    GtarPosition gtarPosition;
    
    gtarPosition.fret = [fretNumber integerValue];
    gtarPosition.string = [stringNumber integerValue];
    
    //[fretNumber release];
    //[stringNumber release];
    
    for ( NSValue * nonretainedObserver in m_observerList ) {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarFretDown:)] == YES )
            [observer gtarFretDown:gtarPosition];
    }
}

- (void)notifyObserversGtarFretUp:(NSDictionary*)dictionary {
    
    NSNumber * fretNumber = [dictionary objectForKey:@"Fret"];
    NSNumber * stringNumber = [dictionary objectForKey:@"String"];
    
    GtarPosition gtarPosition;
    
    gtarPosition.fret = [fretNumber integerValue];
    gtarPosition.string = [stringNumber integerValue];
    
    //[fretNumber release];
    //[stringNumber release];
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarFretUp:)] == YES )
        {
            [observer gtarFretUp:gtarPosition];
        }
    }
}

- (void)notifyObserversGtarNoteOn:(NSDictionary*)dictionary {
    
    NSNumber * fretNumber = [dictionary objectForKey:@"Fret"];
    NSNumber * stringNumber = [dictionary objectForKey:@"String"];
    NSNumber * velocityNumber = [dictionary objectForKey:@"Velocity"];
    
    GtarPluck gtarPluck;
    
    if ( m_scaleVelocity == YES )
    {
        float buggedMax = GtarMaxPluckVelocity * 0.6;
        float adjusted = [velocityNumber floatValue] / buggedMax * GtarMaxPluckVelocity;
        gtarPluck.velocity = MIN(GtarMaxPluckVelocity, adjusted);
    }
    else
    {
        gtarPluck.velocity = [velocityNumber integerValue];
    }
    
    gtarPluck.position.fret = [fretNumber integerValue];
    gtarPluck.position.string = [stringNumber integerValue];
    
    //[fretNumber release];
    //[stringNumber release];
    //[velocityNumber release];
    
    for ( NSValue * nonretainedObserver in m_observerList ) {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarNoteOn:)] == YES )
            [observer gtarNoteOn:gtarPluck];
    }
}

- (void)notifyObserversGtarNoteOff:(NSDictionary*)dictionary {
    
    NSNumber * fretNumber = [dictionary objectForKey:@"Fret"];
    NSNumber * stringNumber = [dictionary objectForKey:@"String"];
    
    GtarPosition gtarPosition;
    
    gtarPosition.fret = [fretNumber integerValue];
    gtarPosition.string = [stringNumber integerValue];
    
    //[fretNumber release];
    //[stringNumber release];
    
    for ( NSValue * nonretainedObserver in m_observerList ) {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarNoteOff:)] == YES )
            [observer gtarNoteOff:gtarPosition];
    }
}

- (void)notifyObserversGtarConnected:(NSDictionary*)dictionary {
    
    // The dictionary will be nil and unused
    
    for ( NSValue * nonretainedObserver in m_observerList ) {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarConnected)] == YES )
            [observer gtarConnected];
    }
}

- (void)notifyObserversGtarDisconnected:(NSDictionary*)dictionary {
    // The dictionary will be nil and unused
    for ( NSValue * nonretainedObserver in m_observerList ) {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarDisconnected)] == YES )
            [observer gtarDisconnected];
    }
}

#pragma mark - Internal firmware handling

- (void)firmwareResponseHandler:(unsigned char)status {
    if ( status == 0x00 ) {
        // success
        if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateProgress:)] == YES )
            [m_delegate receivedFirmwareUpdateProgress:((m_firmwareCurrentPage+1)*100)/GTAR_CONTROLLER_MAX_FIRMWARE_PAGES];
        else
            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateProgress: %@", m_delegate] atLogLevel:GtarControllerLogLevelWarn];
        
        if ( m_firmwareCancelation == YES || m_firmwareUpdating == NO) {
            
            @synchronized ( self ) {
                m_firmwareUpdating = NO;
                m_firmwareCancelation = NO;
            }
            
            // Cancel the transfer, abort now.
            [self logMessage:[NSString stringWithFormat:@"Firmware update canceled, aborting transfer"]
                  atLogLevel:GtarControllerLogLevelInfo];
            
//            if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusFailed)] == YES )
//            {
//                [m_delegate receivedFirmwareUpdateStatusFailed];
//            }
//            else
//            {
//                [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusFailed %@", m_delegate]
//                      atLogLevel:GtarControllerLogLevelWarn];
//                
//            }
            
        }
        else if ( m_firmwareCurrentPage < GTAR_CONTROLLER_MAX_FIRMWARE_PAGES ) {
            // send the next page
            m_firmwareCurrentPage++;
            
            [self sendFirmwarePage:m_firmwareCurrentPage];
        }
        else {
            @synchronized ( self ) {
                // we are done
                //[m_firmware release];
                
                m_firmware = nil;
                
                m_firmwareUpdating = NO;
                m_firmwareCancelation = NO;
            }
            
            if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusSucceeded)] == YES )
                [m_delegate receivedFirmwareUpdateStatusSucceeded];
            else
                [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusSucceeded %@", m_delegate] atLogLevel:GtarControllerLogLevelWarn];
        }
        
    }
    else {
        // Failure
        switch ( status ) {
            case 0x01: {
                [self logMessage:[NSString stringWithFormat:@"Firmware update invalid parameter"] atLogLevel:GtarControllerLogLevelError];
            } break;
                
            case 0x02: {
                [self logMessage:[NSString stringWithFormat:@"Firmware update out of memory"] atLogLevel:GtarControllerLogLevelError];
            } break;
                
            case 0x03: {
                [self logMessage:[NSString stringWithFormat:@"Firmware update will not fit in flash"] atLogLevel:GtarControllerLogLevelError];
            } break;
                
            default:
            case 0x04: {
                [self logMessage:[NSString stringWithFormat:@"Firmware update unknown error"] atLogLevel:GtarControllerLogLevelError];
            } break;
        }
        
        @synchronized ( self ) {
            //[m_firmware release];
            
            m_firmware = nil;
            
            m_firmwareUpdating = NO;
            m_firmwareCancelation = NO;
        }
        
        if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusFailed)] == YES )
            [m_delegate receivedFirmwareUpdateStatusFailed];
        else
            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusFailed %@", m_delegate] atLogLevel:GtarControllerLogLevelWarn];
    }
}

- (void)piezoFirmwareResponseHandler:(unsigned char)status {
    if ( status == 0x00 ) {
        // success
        if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateProgress:)] == YES )
            [m_delegate receivedFirmwareUpdateProgress:((m_firmwareCurrentPage + 1) * 100) / GTAR_CONTROLLER_MAX_FIRMWARE_PAGES];
        else
            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateProgress: %@", m_delegate] atLogLevel:GtarControllerLogLevelWarn];
        
        if ( m_firmwareCancelation == YES || m_firmwareUpdating == NO) {
            
            @synchronized ( self ) {
                m_firmwareUpdating = NO;
                m_firmwareCancelation = NO;
            }
            
            // Cancel the transfer, abort now.
            [self logMessage:[NSString stringWithFormat:@"Piezo Firmware update canceled, aborting transfer"] atLogLevel:GtarControllerLogLevelInfo];
        }
        else if ( m_piezoFirmwareCurrentPage < GTAR_CONTROLLER_MAX_FIRMWARE_PAGES ) {
            // send the next page
            m_piezoFirmwareCurrentPage++;
            [self sendPiezoFirmwarePage:m_piezoFirmwareCurrentPage];
        }
        else {
            @synchronized ( self ) {
                // We are done
                //[m_firmware release];
                
                m_piezoFirmware = nil;
                m_firmwareUpdating = NO;
                m_firmwareCancelation = NO;
            }
            
            if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusSucceeded)] == YES )
                [m_delegate receivedFirmwareUpdateStatusSucceeded];
            else
                [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusSucceeded %@", m_delegate] atLogLevel:GtarControllerLogLevelWarn];
        }
        
    }
    else {
        // Failure
        switch ( status ) {
            case 0x01: {
                [self logMessage:[NSString stringWithFormat:@"Piezo Firmware update invalid parameter"] atLogLevel:GtarControllerLogLevelError];
            } break;
                
            case 0x02: {
                [self logMessage:[NSString stringWithFormat:@"Piezo Firmware update out of memory"] atLogLevel:GtarControllerLogLevelError];
            } break;
                
            case 0x03: {
                [self logMessage:[NSString stringWithFormat:@"Piezo Firmware update will not fit in flash"] atLogLevel:GtarControllerLogLevelError];
            } break;
                
            default:
            case 0x04: {
                [self logMessage:[NSString stringWithFormat:@"Piezo Firmware update unknown error"] atLogLevel:GtarControllerLogLevelError];
            } break;
        }
        
        @synchronized ( self ) {
            //[m_firmware release];
            
            m_piezoFirmware = nil;
            m_firmwareUpdating = NO;
            m_firmwareCancelation = NO;
        }
        
        if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusFailed)] == YES )
            [m_delegate receivedFirmwareUpdateStatusFailed];
        else
            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusFailed %@", m_delegate] atLogLevel:GtarControllerLogLevelWarn];
    }
}

- (BOOL) sendPiezoFirmwarePage:(int)page {
    if ( [m_piezoFirmware length] == 0 ) {
        [self logMessage:@"SendFirmwarePAge: Firmware length 0" atLogLevel:GtarControllerLogLevelWarn];
        return false;
    }
    
    unsigned char * firmwareBytes = (unsigned char *)[m_piezoFirmware bytes];
    unsigned char checksum = 0;
    
    // Sum all the bytes
    for ( int i = 0; i < GTAR_CONTROLLER_PAGE_SIZE; i++ )
        checksum += firmwareBytes[(page * GTAR_CONTROLLER_PAGE_SIZE) + i];
    
    BOOL result = [m_coreMidiInterface sendPiezoFirmwarePackagePage:(firmwareBytes + (page * GTAR_CONTROLLER_PAGE_SIZE))
                                                   andPageSize:GTAR_CONTROLLER_PAGE_SIZE
                                               andFirmwareSize:[m_piezoFirmware length]
                                                  andPageCount:GTAR_CONTROLLER_MAX_FIRMWARE_PAGES
                                                andCurrentPage:page
                                                   andChecksum:checksum];
    
    return result;
}

- (BOOL)sendFirmwarePage:(int)page {
    
    if ( [m_firmware length] == 0 ) {
        [self logMessage:@"SendFirmwarePAge: Firmware length 0" atLogLevel:GtarControllerLogLevelWarn];
        return false;
    }
    
    unsigned char * firmwareBytes = (unsigned char *)[m_firmware bytes];
    unsigned char checksum = 0;
    
    // Sum all the bytes
    for ( int i = 0; i < GTAR_CONTROLLER_PAGE_SIZE; i++ )
        checksum += firmwareBytes[(page*GTAR_CONTROLLER_PAGE_SIZE) + i];
    
    BOOL result = [m_coreMidiInterface sendFirmwarePackagePage:(firmwareBytes + (page * GTAR_CONTROLLER_PAGE_SIZE))
                                                   andPageSize:GTAR_CONTROLLER_PAGE_SIZE
                                               andFirmwareSize:[m_firmware length]
                                                  andPageCount:GTAR_CONTROLLER_MAX_FIRMWARE_PAGES
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
    else {
        return FALSE;
    }
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
- (GtarControllerStatus)addObserver:(id<GtarControllerObserver>)observer {
    
    // We don't want the observers to be retained to prevent circular dependendcies.
    // We need to make sure we dont retain the object.
    NSValue * nonretainedObserver = [NSValue valueWithNonretainedObject:observer];
    
    if ( observer == nil ) {
        [self logMessage:@"Added observer is nil"
              atLogLevel:GtarControllerLogLevelWarn];

        return GtarControllerStatusOk;
    }
    
    // We don't want to add the same observer twice.
    if ( [m_observerList containsObject:nonretainedObserver] == NO ) {
        
        [self logMessage:@"Added observer"
              atLogLevel:GtarControllerLogLevelInfo];
        
        [m_observerList addObject:nonretainedObserver];
        
        // If the guitar is already connected, we should let this new guy know.
        if ( m_connected == YES && [observer respondsToSelector:@selector(gtarConnected)] == YES ) {
            [observer gtarConnected];
        }
        
    }
    else {
        [self logMessage:@"Added observer is already observing"
              atLogLevel:GtarControllerLogLevelWarn];
        
    }
    
    return GtarControllerStatusOk;
}

- (GtarControllerStatus)removeObserver:(id<GtarControllerObserver>)observer {
    // We don't want the observers to be retained to prevent circular dependendcies.
    // We need to make sure we dont retain the object.
    NSValue * nonretainedObserver = [NSValue valueWithNonretainedObject:observer];
    
    if ( nonretainedObserver == nil ) {
        [self logMessage:@"Removed observer is nil"
              atLogLevel:GtarControllerLogLevelWarn];
        
        return GtarControllerStatusOk;
    }
    
    if ( [m_observerList containsObject:nonretainedObserver] == YES ) {
        [self logMessage:@"Removed observer"
              atLogLevel:GtarControllerLogLevelInfo];

        [m_observerList removeObject:nonretainedObserver];
    }
    else {
        [self logMessage:@"Removed observer is not observing"
              atLogLevel:GtarControllerLogLevelWarn];
    }
    
    return GtarControllerStatusOk;
}

#pragma mark - LED manipulation

- (GtarControllerStatus)turnOffAllLeds {
    
    [self logMessage:@"Turning off all LEDs"
          atLogLevel:GtarControllerLogLevelInfo];
    
    GtarControllerStatus status = GtarControllerStatusOk;
    
    if ( m_spoofed == YES )
    {
        
        [self logMessage:@"turnOffAllLeds: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        
        status = GtarControllerStatusOk;
        
    }
    else if ( m_connected == NO )
    {
        
        [self logMessage:@"turnOffAllLeds: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        
        status = GtarControllerStatusNotConnected;
        
    }
    else if ( m_coreMidiInterface == nil )
    {
        
        [self logMessage:@"turnOffAllLeds: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        
        status = GtarControllerStatusError;
        
    }
    else
    {
        
        BOOL result = [m_coreMidiInterface sendSetLedStateFret:0 andString:0 andRed:0 andGreen:0 andBlue:0 andMessage:0];
        
        if ( result == NO )
        {
            [self logMessage:@"turnOffAllLeds: Setting LED state failed"
                  atLogLevel:GtarControllerLogLevelError];
            
            status = GtarControllerStatusError;
        }
        
    }
    
    return status;
    
}
        
- (GtarControllerStatus)turnOffLedAtPosition:(GtarPosition)position
{
    
    GtarFret fret = position.fret;
    GtarString str = position.string;
    
    GtarControllerStatus status = GtarControllerStatusOk;
    
    if ( m_spoofed == YES )
    {
        
        [self logMessage:@"turnOffLedAtString: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        
        status = GtarControllerStatusOk;
        
    }
    else if ( m_connected == NO )
    {
        
        [self logMessage:@"turnOffLedAtString: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        
        status = GtarControllerStatusNotConnected;
        
    }
    else if ( m_coreMidiInterface == nil )
    {
        
        [self logMessage:@"turnOffLedAtString: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        
        status = GtarControllerStatusError;
        
    }
    else
    {
        
        BOOL result = [m_coreMidiInterface sendSetLedStateFret:fret andString:str andRed:0 andGreen:0 andBlue:0 andMessage:0];
        
        if ( result == NO )
        {
            [self logMessage:@"turnOffLedAtString: Setting LED state failed"
                  atLogLevel:GtarControllerLogLevelError];
            
            status = GtarControllerStatusError;
        }
        
    }
    
    return status;
    
}

- (GtarControllerStatus)turnOnLedAtPosition:(GtarPosition)position withColor:(GtarLedColor)color {
    
    GtarFret fret = position.fret;
    GtarString str = position.string;
    
    GtarLedIntensity red = color.red;
    GtarLedIntensity green = color.green;
    GtarLedIntensity blue = color.blue;
    
    GtarControllerStatus status = GtarControllerStatusOk;
    
    if ( m_spoofed == YES ) {
        [self logMessage:@"turnOnLedAtStr: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        
        status = GtarControllerStatusOk;
    }
    else if ( m_connected == NO ){
        [self logMessage:@"turnOnLedAtStr: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        
        status = GtarControllerStatusNotConnected;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"turnOnLedAtStr: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        
        status = GtarControllerStatusError;
    }
    else {
        BOOL result = [m_coreMidiInterface sendSetLedStateFret:fret andString:str andRed:red andGreen:green andBlue:blue andMessage:0];
        
        if ( result == NO ) {
            [self logMessage:@"turnOnLedAtStr: Setting LED state failed"
                  atLogLevel:GtarControllerLogLevelError];
            
            status = GtarControllerStatusError;
        }
    }
    
    return status;
    
}

- (GtarControllerStatus)turnOnLedAtPositionWithColorMap:(GtarPosition)position
{

    GtarFret fret = position.fret;
    GtarString str = position.string;
    
    GtarControllerStatus status = GtarControllerStatusOk;
    
    if ( m_spoofed == YES )
    {
        
        [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        
        status = GtarControllerStatusOk;
        
    }
    else if ( m_connected == NO )
    {
        
        [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        
        status = GtarControllerStatusNotConnected;
        
    }
    else if ( m_coreMidiInterface == nil )
    {
        
        [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        
        status = GtarControllerStatusError;
        
    }
    else
    {
                
        if( str == 0 )
        {
            
            // turn on all strings using their specified color mapping
            
            for( int str = 0; str < GtarStringCount; str++ )
            {
                
                BOOL result = [m_coreMidiInterface sendSetLedStateFret:fret andString:(str+1)
                                                                andRed:m_colorMap.stringColor[str].red
                                                              andGreen:m_colorMap.stringColor[str].green
                                                               andBlue:m_colorMap.stringColor[str].blue
                                                            andMessage:0];
                
                if ( result == NO )
                {
                    [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: Setting LED state failed"
                          atLogLevel:GtarControllerLogLevelError];
                    
                    status = GtarControllerStatusError;
                    break;
                }
            }
        }
        else
        {
            
            // subtract one to zero-base the string
            BOOL result = [m_coreMidiInterface sendSetLedStateFret:fret andString:str
                                                            andRed:m_colorMap.stringColor[str-1].red
                                                          andGreen:m_colorMap.stringColor[str-1].green
                                                           andBlue:m_colorMap.stringColor[str-1].blue
                                                        andMessage:0];

            if ( result == NO )
            {
                [self logMessage:@"turnOnLedWithColorMappingAtFret:andString: Setting LED state failed"
                      atLogLevel:GtarControllerLogLevelError];
                
                status = GtarControllerStatusError;
            }
        }
        
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
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"sendNoteMsg: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"sendNoteMsg: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendNoteMessageOnChannel:channel
                                                   andMidiValue:midiVal
                                                     andMidiVel:midiVel
                                                      andType:type];
    
    if ( result == NO )
    {
        [self logMessage:@"sendNoteMsg: SendNoteMsg failed!"
              atLogLevel:GtarControllerLogLevelError];
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
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"ccTurnOffAllLeds: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"ccTurnOffAllLeds: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    RESULT result = m_coreMidiInterface->SendCCSetLEDState(0,0,0,0,0,0);
    
    if ( CHECK_ERR( result ) )
    {
        [self logMessage:@"ccTurnOffAllLeds: SendSetLEDState failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

- (RESULT)ccTurnOffLedAtString:(GtarString)str andFret:(GtarFret)fret
{

    if ( m_spoofed == YES )
    {
        [self logMessage:@"ccTurnOffLedAtStr: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return R_ERROR;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"ccTurnOffLedAtStr: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return R_ERROR;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"ccTurnOffLedAtStr: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return R_ERROR;
    }
    
    RESULT result = m_coreMidiInterface->SendCCSetLEDState(fret, str,0,0,0,0);
    
    if ( CHECK_ERR( result ) )
    {
        [self logMessage:@"ccTurnOffLedAtStr: SendSetLEDState failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

- (RESULT)ccTurnOnLedAtString:(GtarString)str andFret:(GtarFret)fret withRed:(char)red andGreen:(char)green andBlue:(char)blue
{

    if ( m_spoofed == YES )
    {
        [self logMessage:@"ccTurnOnLedAtStr: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return R_ERROR;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"ccTurnOnLedAtStr: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return R_ERROR;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"ccTurnOnLedAtStr: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return R_ERROR;
    }
    
    RESULT result = m_coreMidiInterface->SendCCSetLEDState(fret, str, red, green, blue, 0);
    
    if ( CHECK_ERR( result ) )
    {
        [self logMessage:@"ccTurnOnLedAtStr: SendSetLEDState failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

- (RESULT)ccTurnOnLedWithColorMappingAtString:(GtarString)str andFret:(GtarFret)fret
{

    if ( m_spoofed == YES )
    {
        [self logMessage:@"ccTurnOnLedWithColorMappingAtString: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return R_ERROR;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"ccTurnOnLedWithColorMappingAtString: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return R_ERROR;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"ccTurnOnLedWithColorMappingAtString: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
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
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}
*/
#endif

#define WAIT_INT 0.25f

#pragma mark - Piezo Set Values

- (BOOL)sendPiezoSensitivityString:(unsigned char)str thresh:(unsigned char)thresh {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendPiezoSensitivityString: Connection spoofed, no-op" atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendPiezoSensitivityString: Not connected" atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendPiezoSensitivityString: CoreMidiInterface is invalid" atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    m_sensitivity[str] = thresh;
    BOOL result = [m_coreMidiInterface sendPiezoSensitivityString:str thresh:m_sensitivity[str]];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendPiezoSensitivityString: sendRequestPiezoSensitivityString failed" atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column value:(unsigned char)value {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendPiezoCrossTalkMatrixRow: Connection spoofed, no-op" atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendPiezoCrossTalkMatrixRow: Not connected" atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendPiezoCrossTalkMatrixRow: CoreMidiInterface is invalid" atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    m_ctmatrix[row][column] = value;
    BOOL result = [m_coreMidiInterface sendPiezoCrossTalkMatrixRow:row Column:column value:m_ctmatrix[row][column]];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendPiezoCrossTalkMatrixRow: SendRequestBatteryStatus failed" atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendPiezoWindowIndex:(unsigned char)index value:(unsigned char)value {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendPiezoWindowIndex: Connection spoofed, no-op" atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendPiezoWindowIndex: Not connected" atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendPiezoWindowIndex: CoreMidiInterface is invalid" atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendPiezoWindowIndex:index value:value];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendPiezoWindowIndex: SendRequestBatteryStatus failed" atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

#pragma mark - Piezo Requests

- (BOOL)sendRequestPiezoSensitivityString:(unsigned char)str {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendRequestPiezoSensitivityString: Connection spoofed, no-op" atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendRequestPiezoSensitivityString: Not connected" atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendRequestPiezoSensitivityString: CoreMidiInterface is invalid" atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestPiezoSensitivityString:str];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendRequestPiezoSensitivityString: sendRequestPiezoSensitivityString failed" atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendRequestPiezoCrossTalkMatrixRow: Connection spoofed, no-op" atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendRequestPiezoCrossTalkMatrixRow: Not connected" atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendRequestPiezoCrossTalkMatrixRow: CoreMidiInterface is invalid" atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestPiezoCrossTalkMatrixRow:row Column:column];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendRequestPiezoCrossTalkMatrixRow: SendRequestBatteryStatus failed" atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestPiezoWindowIndex:(unsigned char)index {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendRequestPiezoWindowIndex: Connection spoofed, no-op" atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendRequestPiezoWindowIndex: Not connected" atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendRequestPiezoWindowIndex: CoreMidiInterface is invalid" atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestPiezoWindowIndex:index];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendRequestPiezoWindowIndex: SendRequestBatteryStatus failed" atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestCommitUserspace {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendRequestCommitUserspace: Connection spoofed, no-op" atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendRequestCommitUserspace: Not connected" atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendRequestCommitUserspace: CoreMidiInterface is invalid" atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestCommitUserspace];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendRequestCommitUserspace: sendRequestCommitUserspace failed" atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestResetUserspace {
    if ( m_spoofed == YES ) {
        [self logMessage:@"sendRequestResetUserspace: Connection spoofed, no-op" atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"sendRequestResetUserspace: Not connected" atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"sendRequestResetUserspace: CoreMidiInterface is invalid" atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestResetUserspace];
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        [NSThread sleepForTimeInterval:WAIT_INT];
    }];
    
    if ( result == NO )
        [self logMessage:@"sendRequestResetUserspace: sendRequestResetUserspace failed" atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

#pragma mark - Requests

- (BOOL)sendRequestBatteryStatus
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestBatteryStatus: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestBatteryStatus: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestBatteryStatus: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestBatteryStatus];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestBatteryStatus: SendRequestBatteryStatus failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendRequestSerialNumber {
    if( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestSerialNumber: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestSerialNumber: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestSerialNumber: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestSerialNumber:m_pendingSerialByte];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestSerialNumber: SendRequestBatteryStatus failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendEnableDebug
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendEnableDebug: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendEnableDebug: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendEnableDebug: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendEnableDebug];
    
    if ( result == NO )
    {
        [self logMessage:@"SendEnableDebug: SendEnableDebug failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;

}

- (BOOL)sendDisableDebug
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendDisableDebug: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendDisableDebug: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendDisableDebug: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendDisableDebug];
    
    if ( result == NO )
    {
        [self logMessage:@"SendDisableDebug: SendDisableDebug failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;

}

- (BOOL)sendRequestCertDownload
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestCertDownload: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestCertDownload: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestCertDownload: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestCertDownload];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestCertDownload: SendDisableDebug failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;

}

- (BOOL)sendRequestFirmwareVersion
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendRequestFirmwareVersion: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendRequestFirmwareVersion: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendRequestFirmwareVersion: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    BOOL result = [m_coreMidiInterface sendRequestFirmwareVersion];
    
    if ( result == NO )
    {
        [self logMessage:@"SendRequestFirmwareVersion: SendRequestFirmwareVersion failed"
              atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

//- (BOOL)sendFirmwarePackagePage:(void*)buffer
//                     bufferSize:(int)bufferLength
//                         fwSize:(int)fwSize 
//                        fwPages:(int)fwPages
//                        curPage:(int)curPage
//                   withCheckSum:(unsigned char)checkSum
//{
//
//    if ( m_spoofed == YES )
//    {
//        [self logMessage:@"SendRequestFirmwareVersion: Connection spoofed, no-op"
//              atLogLevel:GtarControllerLogLevelInfo];
//        return NO;
//    }
//    else if ( m_connected == NO )
//    {
//        [self logMessage:@"SendRequestFirmwareVersion: Not connected"
//              atLogLevel:GtarControllerLogLevelWarn];
//        return NO;
//    }
//    else if ( m_coreMidiInterface == nil )
//    {
//        [self logMessage:@"SendRequestFirmwareVersion: CoreMidiInterface is invalid"
//              atLogLevel:GtarControllerLogLevelError];
//        return NO;
//    }
//    
//    BOOL result = [m_coreMidiInterface sendFirmwarePackagePage:(unsigned char *)buffer
//                                                    withLength:bufferLength
//                                                       andSize:fwSize
//                                                      andPages:fwPages
//                                                    andCurPage:curPage
//                                                   andCheckSum:checkSum];
//    
//    if ( result == NO )
//    {
//        [self logMessage:@"SendFirmwarePackagePage: Failed to send firmware package page"
//              atLogLevel:GtarControllerLogLevelError];
//    }
//    
//    return result; 
//}

- (BOOL)sendPiezoFirmwareUpdate:(NSData*)firmware {
    if ( m_spoofed == YES ) {
        [self logMessage:@"SendPiezoFirmwareUpdate: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"SendPiezoFirmwareUpdate: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"SendPiezoFirmwareUpdate: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    @synchronized ( self ) {
        
        if ( m_firmwareCancelation == YES ) {
            [self logMessage:@"SendPiezoFirmwareUpdate: Cancellation in progress"
                  atLogLevel:GtarControllerLogLevelWarn];
            return NO;
        }
        
        if ( m_firmwareUpdating == YES ) {
            [self logMessage:@"SendPiezoFirmwareUpdate: Firmware update in progress"
                  atLogLevel:GtarControllerLogLevelWarn];
            return NO;
        }
        
        //[m_firmware release];
        m_piezoFirmware = [[NSData alloc] initWithData:firmware];
        m_piezoFirmwareCurrentPage = 0;
        m_firmwareUpdating = YES;
        
        BOOL result = [self sendPiezoFirmwarePage:m_piezoFirmwareCurrentPage];
        
        if ( result == NO )
            [self logMessage:@"SendPiezoFirmwareUpdate: Failed to send firmware package page" atLogLevel:GtarControllerLogLevelError];
        
        return result;
    }
}

- (BOOL)sendFirmwareUpdate:(NSData*)firmware {
    
    if ( m_spoofed == YES ) {
        [self logMessage:@"SendFirmwareUpdate: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO ) {
        [self logMessage:@"SendFirmwareUpdate: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil ) {
        [self logMessage:@"SendFirmwareUpdate: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    @synchronized ( self ) {
        
        if ( m_firmwareCancelation == YES ) {
            [self logMessage:@"SendFirmwareUpdate: Cancellation in progress"
                  atLogLevel:GtarControllerLogLevelWarn];
            return NO;
        }
        
        if ( m_firmwareUpdating == YES ) {
            [self logMessage:@"SendFirmwareUpdate: Firmware update in progress"
                  atLogLevel:GtarControllerLogLevelWarn];
            return NO;
        }
        
        //[m_firmware release];
        m_firmware = [[NSData alloc] initWithData:firmware];
        
        m_firmwareCurrentPage = 0;
        m_firmwareUpdating = YES;
        BOOL result = [self sendFirmwarePage:m_firmwareCurrentPage];
        
        if ( result == NO )
            [self logMessage:@"SendFirmwareUpdate: Failed to send firmware package page" atLogLevel:GtarControllerLogLevelError];
        
        return result;
    }
}

- (BOOL)sendFirmwareUpdateCancelation {
    @synchronized ( self ) {
        // Only cancel if we are updating a firmware
        if ( m_firmwareUpdating == YES ) {
            
            [self logMessage:@"Canceling firmware update"
                  atLogLevel:GtarControllerLogLevelInfo];
            
            m_firmwareCancelation = YES;
            m_firmwareUpdating = NO;
            
            //[m_firmware release];
            
            m_firmware = nil;
            
            if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusFailed)] == YES ) {
                [m_delegate receivedFirmwareUpdateStatusFailed];
            }
            else {
                [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusFailed %@", m_delegate]
                      atLogLevel:GtarControllerLogLevelWarn];
            }
        }
        else {
            [self logMessage:@"Cannot cancel, no firmware update in progress"
                  atLogLevel:GtarControllerLogLevelWarn];
        }
    }
    return YES;
}

#pragma mark - Color mapping manipulation

// Currently the only way to change the color mapping is by setting 'colorMap' directly.
// I think that is sufficient.

//- (GtarControllerStatus)setStringsColorMapping:(char**)colorMap
//{
//    
//    for ( GtarString str = 0; str < GtarStringCount; str++ )
//    {
//        [self setStringColorMapping:str toRed:colorMap[str][0] andGreen:colorMap[str][1] andBlue:colorMap[str][2]];
//    }
//    
//    return GtarControllerStatusOk;
//    
//}

//- (GtarControllerStatus)setStringColorMapping:(GtarString)str toRed:(GtarLedIntensity)red andGreen:(GtarLedIntensity)green andBlue:(GtarLedIntensity)blue
//{
//    // Sanity check arguments. We could chose to return 
//    // a GtarControllerStatusInvalidParamter status, but its a lot
//    // friendlier to just fix it.
//    if ( red > GtarMaxLedIntensity ) 
//    {
//        red = GtarMaxLedIntensity;
//    }
//    
//    if ( green > GtarMaxLedIntensity )
//    {
//        green = GtarMaxLedIntensity;
//    }
//    
//    if ( blue > GtarMaxLedIntensity )
//    {
//        blue = GtarMaxLedIntensity;
//    }
//    
//    m_colorMap.stringColor[str].red = red;
//    m_colorMap.stringColor[str].green = green;
//    m_colorMap.stringColor[str].blue = blue;
//    
//    return GtarControllerStatusOk;
//    
//}

#pragma mark - Effect handling

- (GtarControllerStatus)turnOffAllEffects
{
    
    GtarControllerStatus status = GtarControllerStatusOk;
    
    [self logMessage:@"Turning off all effects"
          atLogLevel:GtarControllerLogLevelInfo];
    
    BOOL result;
    
    result = [m_coreMidiInterface sendSetFretFollowRed:0 andGreen:0 andBlue:0];
    
    if ( result == NO )
    {
        [self logMessage:@"turnOffAllEffects: SendSetFretFollow failed!"
              atLogLevel:GtarControllerLogLevelError];
        
        status = GtarControllerStatusError;
    }
    
    result = [m_coreMidiInterface sendSetNoteActiveRed:0 andGreen:0 andBlue:0];
    
    if ( result == NO )
    {
        [self logMessage:@"turnOffAllEffects: SendSetNoteActive failed!"
              atLogLevel:GtarControllerLogLevelError];
        
        status = GtarControllerStatusError;
    }
    
    return status;
    
}

- (GtarControllerStatus)turnOnEffect:(GtarControllerEffect)effect withColor:(GtarLedColor)color {
    char red = color.red;
    char green = color.green;
    char blue = color.blue;
    
    GtarControllerStatus status = GtarControllerStatusOk;
    
    switch ( effect )  {
        case GtarControllerEffectFretFollow:
        {
            // Enable FF mode
            BOOL result = [m_coreMidiInterface sendSetFretFollowRed:red andGreen:green andBlue:blue];
            
            if ( result == NO )
            {
                [self logMessage:@"setEffectColor: SendSetFretFollow failed!"
                      atLogLevel:GtarControllerLogLevelError];
                
                status = GtarControllerStatusError;
            }
            
        } break;
            
        case GtarControllerEffectNoteActive:
        {
            // Enable NA mode
            BOOL result = [m_coreMidiInterface sendSetNoteActiveRed:red andGreen:green andBlue:blue];
            
            if ( result == NO )
            {
                [self logMessage:@"setEffectColor: SendSetNoteActive failed!"
                      atLogLevel:GtarControllerLogLevelError];
                
                status = GtarControllerStatusError;
            }
            
        } break;
            
        case GtarControllerEffectFretFollowNoteActive:
        {
            // Enable FF mode
            BOOL result;
            
            result = [m_coreMidiInterface sendSetFretFollowRed:red andGreen:green andBlue:blue];
            
            if ( result == NO )
            {
                [self logMessage:@"setEffectColor: SendSetFretFollow failed!"
                      atLogLevel:GtarControllerLogLevelError];
                
                status = GtarControllerStatusError;
            }
            
            // Enable NA mode
            result = [m_coreMidiInterface sendSetNoteActiveRed:red andGreen:green andBlue:blue];
            
            if ( result == NO )
            {
                [self logMessage:@"setEffectColor: SendSetNoteActive failed!"
                      atLogLevel:GtarControllerLogLevelError];
                
                status = GtarControllerStatusError;
            }

        } break;
            
        case GtarControllerEffectNone:
        default:
        {
            
            // nothing
            
        } break;
    }
    
    return status;
    
}

@end
