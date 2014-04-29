//
//  GtarController.m
//  GtarController
//
//  Created by Marty Greenia on 5/24/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//

#import "GtarControllerInternal.h"

#import "CoreMidiInterface.h"

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

- (id)init
{
    
    self = [super init];
    
    if ( self ) 
    {
        
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
        
        // Initialize the previous pluck times to zero
        for ( GtarString str = 0; str < GtarStringCount; str++ )
        {
            for ( GtarFret fret = 0; fret < GtarFretCount; fret++ )
            {
                m_previousPluckTime[str][fret] = 0;
            }
        }
        
        [self logMessage:@"GtarController initializing"
              atLogLevel:GtarControllerLogLevelInfo];

        // Create the midi interface
        m_coreMidiInterface = [[CoreMidiInterface alloc] initWithGtarController:self];
        
        m_observerList = [[NSMutableArray alloc] init];
        
        if ( m_coreMidiInterface == nil )
        {
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

+ (GtarController *)sharedInstance
{
    static GtarController *sharedSingleton;
    
    @synchronized(self)
    {
        if (!sharedSingleton)
        {
            sharedSingleton = [[GtarController alloc] init];
        }
        
        return sharedSingleton;
    }
}

- (void)dealloc
{
    
    //[m_coreMidiInterface release];
    
    //[m_observerList release];
        
    //[m_firmware release];
    
    //[super dealloc];
    
}

#pragma mark - External debug functions

- (GtarControllerStatus)debugSpoofConnected
{
    
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

- (GtarControllerStatus)debugSpoofDisconnected
{
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

- (BOOL)checkNoteInterarrivalTime:(double)time forFret:(GtarFret)fret andString:(GtarString)str
{
    
    // zero base the string
    str--;
    
    if ( (time - m_previousPluckTime[str][fret]) >= m_minimumInterarrivalTime )
    {
        m_previousPluckTime[str][fret] = time;
        
        return YES;
    }
    else
    {
        [self logMessage:[NSString stringWithFormat:@"Dropping double-triggered note: %f secs", (time - m_previousPluckTime[str][fret])]
              atLogLevel:GtarControllerLogLevelInfo];
        
        return NO;
    }
    
}

- (void)logMessage:(NSString*)str atLogLevel:(GtarControllerLogLevel)level
{
    
    if ( level <= m_logLevel )
    {
        switch (level)
        {
            case GtarControllerLogLevelError:
            {
                NSLog(@"GtarController: Error: %@", str );
            } break;
                
            case GtarControllerLogLevelWarn:
            {
                NSLog(@"GtarController: Warning: %@", str );
            } break;
                
            case GtarControllerLogLevelInfo:
            {
                NSLog(@"GtarController: Info: %@", str );
            } break;
                
            default:
            {
                NSLog(@"GtarController: %@", str );
            } break;
                
        }
    }
    
}

- (int)getFretFromMidiNote:(int)midiNote andString:(int)str
{
    
    if ( str < 0 || str > 5 )
    {
        return -1;
    }
    
    int fret = midiNote - (40 + 5 * str);
    
    if (str > 3 )
    {
        fret += 1;
    }
    
    return fret;
}

#pragma mark - Internal MIDI functions

- (void)midiConnectionHandler:(BOOL)connected
{
    
    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
    
    // update the delegate as to what has happened
    if ( connected == true )
    {
        [self logMessage:@"Gtar Midi device connected"
              atLogLevel:GtarControllerLogLevelInfo];
        
        m_connected = YES;
        m_spoofed = NO;
        
        [responseDictionary setValue:@"notifyObserversGtarConnected:" forKey:@"Selector"];
    }
    else
    {
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

- (void)midiCallbackHandler:(char*)data
{
    
    unsigned char msgType = (data[0] & 0xF0) >> 4;
    unsigned char str = (data[0] & 0xF);
    
    double currentTime = [NSDate timeIntervalSinceReferenceDate];
    
    switch ( msgType )
    {
            // This is the Note Off event.
        case 0x8:
        {
            
            unsigned char fret = [self getFretFromMidiNote:data[1] andString:(str-1)];
            
            NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
            
            [responseDictionary setValue:@"notifyObserversGtarNoteOff:" forKey:@"Selector"];
            [responseDictionary setValue:[[NSNumber alloc] initWithChar:fret] forKey:@"Fret"];
            [responseDictionary setValue:[[NSNumber alloc] initWithChar:str] forKey:@"String"];
            
            [self midiCallbackDispatch:responseDictionary];
            
            //[responseDictionary release];
            
        } break;
            
            // This is the Note On event.
        case 0x9:
        {
            
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
        case 0xB:
        {
            
            unsigned char gTarMsgType = data[1];
            
            switch ( gTarMsgType )
            {
                case RX_FRET_UP:
                {
                    // Fret Up
                    unsigned char fret = data[2];
                    
                    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                    
                    [responseDictionary setValue:@"notifyObserversGtarFretUp:" forKey:@"Selector"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:fret] forKey:@"Fret"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:str] forKey:@"String"];
                    
                    [self midiCallbackDispatch:responseDictionary];
                    
                    //[responseDictionary release];
                    
                } break;
                    
                case RX_FRET_DOWN:
                {
                    // Fret Down
                    unsigned char fret = data[2];
                    
                    NSMutableDictionary * responseDictionary = [[NSMutableDictionary alloc] init];
                    
                    [responseDictionary setValue:@"notifyObserversGtarFretDown:" forKey:@"Selector"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:fret] forKey:@"Fret"];
                    [responseDictionary setValue:[[NSNumber alloc] initWithChar:str] forKey:@"String"];
                    
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
                              atLogLevel:GtarControllerLogLevelWarn];
                        
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
                              atLogLevel:GtarControllerLogLevelWarn];
                        
                    }

                } break;
                    
                case RX_BATTERY_CHARGE:
                {
                    // Battery charge Ack
                    unsigned char percentage = data[2];
                    
                    if ( [m_delegate respondsToSelector:@selector(receivedBatteryCharge:)] == YES )
                    {
                        [m_delegate receivedBatteryCharge:percentage];
                    }
                    else
                    {
                        [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to RxBatteryCharge %@", m_delegate]
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

- (void)midiCallbackDispatch:(NSDictionary*)dictionary
{
    
    // Hold onto the dictionary before we queue up a worker thread.
    // This is primarily important for the async case.
    //[dictionary retain];

    if ( m_responseThread == GtarControllerThreadMain )
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
- (void)notifyObserversGtarFretDown:(NSDictionary*)dictionary
{
    
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
        
        if ( [observer respondsToSelector:@selector(gtarFretDown:)] == YES )
        {
            [observer gtarFretDown:gtarPosition];
        }
    }
}

- (void)notifyObserversGtarFretUp:(NSDictionary*)dictionary
{
    
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

- (void)notifyObserversGtarNoteOn:(NSDictionary*)dictionary
{
    
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
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarNoteOn:)] == YES )
        {
            [observer gtarNoteOn:gtarPluck];
        }
    }
}

- (void)notifyObserversGtarNoteOff:(NSDictionary*)dictionary
{
    
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
        
        if ( [observer respondsToSelector:@selector(gtarNoteOff:)] == YES )
        {
            [observer gtarNoteOff:gtarPosition];
        }
    }
}

- (void)notifyObserversGtarConnected:(NSDictionary*)dictionary
{
    
    // The dictionary will be nil and unused
    
    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarConnected)] == YES )
        {
            [observer gtarConnected];
        }
    }
}

- (void)notifyObserversGtarDisconnected:(NSDictionary*)dictionary
{
    
    // The dictionary will be nil and unused

    for ( NSValue * nonretainedObserver in m_observerList )
    {
        id observer = [nonretainedObserver nonretainedObjectValue];
        
        if ( [observer respondsToSelector:@selector(gtarDisconnected)] == YES )
        {
            [observer gtarDisconnected];
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
            [m_delegate receivedFirmwareUpdateProgress:((m_firmwareCurrentPage+1)*100)/GTAR_CONTROLLER_MAX_FIRMWARE_PAGES];
        }
        else
        {
            [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateProgress: %@", m_delegate]
                  atLogLevel:GtarControllerLogLevelWarn];
            
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
        else if ( m_firmwareCurrentPage < GTAR_CONTROLLER_MAX_FIRMWARE_PAGES )
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
                      atLogLevel:GtarControllerLogLevelWarn];
                
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
                      atLogLevel:GtarControllerLogLevelError];
            } break;
                
            case 0x02:
            {
                [self logMessage:[NSString stringWithFormat:@"Firmware update out of memory"]
                      atLogLevel:GtarControllerLogLevelError];
            } break;
                
            case 0x03:
            {
                [self logMessage:[NSString stringWithFormat:@"Firmware update will not fit in flash"]
                      atLogLevel:GtarControllerLogLevelError];
            } break;
                
            default:
            case 0x04:
            {
                [self logMessage:[NSString stringWithFormat:@"Firmware update unknown error"]
                      atLogLevel:GtarControllerLogLevelError];
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
                  atLogLevel:GtarControllerLogLevelWarn];
            
        }
        
    }
    
    
}

- (BOOL)sendFirmwarePage:(int)page
{
    
    if ( [m_firmware length] == 0 )
    {
        return false;
    }
    
    unsigned char * firmwareBytes = (unsigned char *)[m_firmware bytes];
    
    unsigned char checksum = 0;
    
    // Sum all the bytes
    for ( int i = 0; i < GTAR_CONTROLLER_PAGE_SIZE; i++ )
    {
        checksum += firmwareBytes[(page*GTAR_CONTROLLER_PAGE_SIZE) + i];
    }
    
    BOOL result = [m_coreMidiInterface sendFirmwarePackagePage:(firmwareBytes + (page * GTAR_CONTROLLER_PAGE_SIZE))
                                                   andPageSize:GTAR_CONTROLLER_PAGE_SIZE
                                               andFirmwareSize:[m_firmware length]
                                                  andPageCount:GTAR_CONTROLLER_MAX_FIRMWARE_PAGES
                                                andCurrentPage:page
                                                   andChecksum:checksum];
    
    return result;
}


#pragma mark - Observer management

// Observers should ultimately replace the delegate paradigm we have going.
// I didn't want to rip out the delegate functionality since we use it all
// over the place, but new stuff will need to use the observer model.
- (GtarControllerStatus)addObserver:(id<GtarControllerObserver>)observer
{
    
    // We don't want the observers to be retained to prevent circular dependendcies.
    // We need to make sure we dont retain the object.
    NSValue * nonretainedObserver = [NSValue valueWithNonretainedObject:observer];
    
    if ( observer == nil )
    {
        [self logMessage:@"Added observer is nil"
              atLogLevel:GtarControllerLogLevelWarn];

        return GtarControllerStatusOk;
    }
    
    //
    // We don't want to add the same observer twice.
    //
    if ( [m_observerList containsObject:nonretainedObserver] == NO )
    {
        
        [self logMessage:@"Added observer"
              atLogLevel:GtarControllerLogLevelInfo];
        
        [m_observerList addObject:nonretainedObserver];
        
        // If the guitar is already connected, we should let this new guy know.
        if ( m_connected == YES && [observer respondsToSelector:@selector(gtarConnected)] == YES )
        {
            [observer gtarConnected];
        }
        
    }
    else
    {
        [self logMessage:@"Added observer is already observing"
              atLogLevel:GtarControllerLogLevelWarn];
        
    }
    
    return GtarControllerStatusOk;
    
}

- (GtarControllerStatus)removeObserver:(id<GtarControllerObserver>)observer
{
    
    // We don't want the observers to be retained to prevent circular dependendcies.
    // We need to make sure we dont retain the object.
    NSValue * nonretainedObserver = [NSValue valueWithNonretainedObject:observer];
    
    if ( nonretainedObserver == nil )
    {
        [self logMessage:@"Removed observer is nil"
              atLogLevel:GtarControllerLogLevelWarn];
        
        return GtarControllerStatusOk;
    }
    
    if ( [m_observerList containsObject:nonretainedObserver] == YES )
    {
        
        [self logMessage:@"Removed observer"
              atLogLevel:GtarControllerLogLevelInfo];

        [m_observerList removeObject:nonretainedObserver];
    }
    else
    {
        [self logMessage:@"Removed observer is not observing"
              atLogLevel:GtarControllerLogLevelWarn];
    }
    
    return GtarControllerStatusOk;
    
}

#pragma mark - LED manipulation

- (GtarControllerStatus)turnOffAllLeds
{
    
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

- (GtarControllerStatus)turnOnLedAtPosition:(GtarPosition)position withColor:(GtarLedColor)color
{
    
    GtarFret fret = position.fret;
    GtarString str = position.string;
    
    GtarLedIntensity red = color.red;
    GtarLedIntensity green = color.green;
    GtarLedIntensity blue = color.blue;
    
    GtarControllerStatus status = GtarControllerStatusOk;
    
    if ( m_spoofed == YES )
    {
        
        [self logMessage:@"turnOnLedAtStr: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        
        status = GtarControllerStatusOk;
        
    }
    else if ( m_connected == NO )
    {
        
        [self logMessage:@"turnOnLedAtStr: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        
        status = GtarControllerStatusNotConnected;
        
    }
    else if ( m_coreMidiInterface == nil )
    {
        
        [self logMessage:@"turnOnLedAtStr: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        
        status = GtarControllerStatusError;
        
    }
    else
    {
        
        BOOL result = [m_coreMidiInterface sendSetLedStateFret:fret andString:str andRed:red andGreen:green andBlue:blue andMessage:0];
        
        if ( result == NO )
        {
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

- (BOOL)sendFirmwareUpdate:(NSData*)firmware
{
    
    if ( m_spoofed == YES )
    {
        [self logMessage:@"SendFirmwareUpdate: Connection spoofed, no-op"
              atLogLevel:GtarControllerLogLevelInfo];
        return NO;
    }
    else if ( m_connected == NO )
    {
        [self logMessage:@"SendFirmwareUpdate: Not connected"
              atLogLevel:GtarControllerLogLevelWarn];
        return NO;
    }
    else if ( m_coreMidiInterface == nil )
    {
        [self logMessage:@"SendFirmwareUpdate: CoreMidiInterface is invalid"
              atLogLevel:GtarControllerLogLevelError];
        return NO;
    }
    
    @synchronized ( self )
    {
        
        if ( m_firmwareCancelation == YES )
        {
            [self logMessage:@"SendFirmwareUpdate: Cancellation in progress"
                  atLogLevel:GtarControllerLogLevelWarn];
            return NO;
        }
        
        if ( m_firmwareUpdating == YES )
        {
            [self logMessage:@"SendFirmwareUpdate: Firmware update in progress"
                  atLogLevel:GtarControllerLogLevelWarn];
            return NO;
        }
        
        //[m_firmware release];
        
        //m_firmware = [firmware retain];
        
        m_firmwareCurrentPage = 0;
        
        m_firmwareUpdating = YES;
        
        BOOL result = [self sendFirmwarePage:m_firmwareCurrentPage];
        
        if ( result == NO )
        {
            [self logMessage:@"SendFirmwareUpdate: Failed to send firmware package page"
                  atLogLevel:GtarControllerLogLevelError];
        }
        
        return result;
    }
}

- (BOOL)sendFirmwareUpdateCancelation
{
    
    @synchronized ( self )
    {
        // Only cancel if we are updating a firmware
        if ( m_firmwareUpdating == YES )
        {
            
            [self logMessage:@"Canceling firmware update"
                  atLogLevel:GtarControllerLogLevelInfo];
            
            m_firmwareCancelation = YES;
            m_firmwareUpdating = NO;
            
            //[m_firmware release];
            
            m_firmware = nil;
            
            if ( [m_delegate respondsToSelector:@selector(receivedFirmwareUpdateStatusFailed)] == YES )
            {
                [m_delegate receivedFirmwareUpdateStatusFailed];
            }
            else
            {
                [self logMessage:[NSString stringWithFormat:@"Delegate doesn't respond to receivedFirmwareUpdateStatusFailed %@", m_delegate]
                      atLogLevel:GtarControllerLogLevelWarn];
                
            }
        }
        else
        {
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

- (GtarControllerStatus)turnOnEffect:(GtarControllerEffect)effect withColor:(GtarLedColor)color
{
    
    char red = color.red;
    char green = color.green;
    char blue = color.blue;
    
    GtarControllerStatus status = GtarControllerStatusOk;
    
    switch ( effect ) 
    {
            
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
