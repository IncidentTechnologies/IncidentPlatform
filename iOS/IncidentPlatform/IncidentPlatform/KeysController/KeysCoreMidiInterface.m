//
//  KeysCoreMidiInterface.m
//  KeysController
//
//  Created by Kate Schnippering on 10/23/14.
//  Copyright (c) 2014 Incident Technologies. All rights reserved.
//

#import "KeysCoreMidiInterface.h"

#import "KeysControllerInternal.h"

#import <UIKit/UIKit.h>

@implementation KeysCoreMidiInterface

@synthesize m_keysController;
@synthesize m_connected;

- (id)initWithKeysController:(KeysController*)keysController
{
    self = [super init];
    
    if ( self ) {
        m_keysController = keysController;
        [m_keysController logMessage:[NSString stringWithFormat:@"KeysCoreMidiInterface initializing"]
                          atLogLevel:KeysControllerLogLevelInfo];
        
        m_fPendingMatrixValue = false;
        m_PendingMatrixValueRow = 0;
        m_PendingMatrixValueColumn = 0;
        
        m_fPendingSensString = false;
        m_PendingSensString = 0;
        
        // Create arrays
        m_midiSources = [[NSMutableArray alloc] init];
        m_midiDestinations = [[NSMutableArray alloc] init];
        m_sendQueue = [[NSMutableArray alloc] init];
        
        m_connected = NO;
        
        // Create the midi client
        OSStatus oss = MIDIClientCreate(CFSTR("AE Midi Client"), KeysMIDIStateChangedHandler, (__bridge void *)(self), &m_pMidiClient);
        
        if( oss != 0 )
        {
            [m_keysController logMessage:[NSString stringWithFormat:@"Failed to create AE Midi Client: Error %ld", oss]
                              atLogLevel:KeysControllerLogLevelError];
            
        }
        
        // Set up the Input Port
        oss = MIDIInputPortCreate(m_pMidiClient, (CFStringRef)@"Midi Client Input Port", KeysMIDIReadHandler, (__bridge void *)(self), &m_pMidiInputPort);
        
        if( oss != 0 )
        {
            [m_keysController logMessage:[NSString stringWithFormat:@"Failed to create input port: Error %ld", oss]
                              atLogLevel:KeysControllerLogLevelError];
        }
        
        // Set up the Output Port
        oss = MIDIOutputPortCreate(m_pMidiClient, (CFStringRef)@"MidiClient Output Port", &m_pMidiOutputPort);
        
        if( oss != 0 )
        {
            [m_keysController logMessage:[NSString stringWithFormat:@"Failed to create output port: Error %ld", oss]
                              atLogLevel:KeysControllerLogLevelError];
        }
        
        // Wait for foreground notifications
        //        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateEndpoints) name:UIApplicationWillEnterForegroundNotification object:nil];
        
        [self updateEndpoints];
        
    }
    
    return self;
}

- (void)dealloc
{
    
    if ( m_pMidiClient != NULL )
    {
        OSStatus oss = MIDIClientDispose(m_pMidiClient);
        
        if ( oss != 0 )
        {
            [m_keysController logMessage:[NSString stringWithFormat:@"Failed to dispose client: Error code %ld", oss]
                              atLogLevel:KeysControllerLogLevelError];
        }
        
        m_pMidiClient = NULL;
    }
    
    //[m_midiSources release];
    
    //[m_midiDestinations release];
    
    // We have to nullify the requests that have already been sent out.
    @synchronized( m_sendQueue )
    {
        for ( NSValue * sendPtr in m_sendQueue )
        {
            MIDISysexSendRequest * sendRequest = (MIDISysexSendRequest*)[sendPtr pointerValue];
            
            sendRequest->completionRefCon = NULL;
        }
    }
    
    //[m_sendQueue release];
    
    //[super dealloc];
}

#pragma mark - Send Queue Management
- (void)processSendQueue
{
    
    @synchronized( m_sendQueue )
    {
        
        NSValue * ptr = [m_sendQueue objectAtIndex:0];
        
        // We are done with this pointer
        MIDISysexSendRequest * sendRequest = (MIDISysexSendRequest *)[ptr pointerValue];
        
        // This will apply to the data buffer as well since they are alloc'd in one malloc
        free(sendRequest);
        
        [m_sendQueue removeObject:ptr];
        
        // See if we have anything else queued up
        if ( [m_sendQueue count] > 0 )
        {
            sendRequest = (MIDISysexSendRequest*)[[m_sendQueue objectAtIndex:0] pointerValue];
            
            OSStatus oss = MIDISendSysex(sendRequest);
            
            if ( oss == -1 )
            {
                [m_keysController logMessage:[NSString stringWithFormat:@"SendSysExBuffer: MIDISend failed with status 0x%lx", oss]
                                  atLogLevel:KeysControllerLogLevelError];
            }
        }
        
    }
    
}

#pragma mark - Endpoint management

- (void)updateEndpoints
{
    NSLog( @"Updating endpoints" );
    BOOL previousConnected = m_connected;
    
    int sourceCount = [self updateSources];
    int destinationCount = [self updateDestinations];
    
    [m_keysController logMessage:[NSString stringWithFormat:@"Found %d sources and %d destinations", sourceCount, destinationCount]
                      atLogLevel:KeysControllerLogLevelInfo];
    
    // Are we connected to something
    m_connected = (m_sourceConnected && m_destinationConnected);
    
    // See if the connection status changed from connected to !connected or vice versa
    if ( previousConnected == YES )
    {
        if ( m_connected == NO)
        {
            //            [m_keysController logMessage:[NSString stringWithFormat:@"Keys Disconnected"]
            //                              atLogLevel:KeysControllerLogLevelInfo];
            
            [m_keysController midiConnectionHandler:NO];
        }
    }
    else
    {
        if ( m_connected == YES )
        {
            //            [m_keysController logMessage:[NSString stringWithFormat:@"Keys Connected"]
            //                              atLogLevel:KeysControllerLogLevelInfo];
            
            [m_keysController midiConnectionHandler:YES];
        }
    }
    
}

- (int)updateSources
{
    
    int sourceCount = MIDIGetNumberOfSources();
    
    NSLog(@"%i sources",sourceCount);
    
    [m_midiSources removeAllObjects];
    
    m_sourceConnected = NO;
    
    for ( int i = 0; i < sourceCount; i++)
    {
        
        MIDIEndpointRef sourceEndpoint = MIDIGetSource(i);
        
        if ( sourceEndpoint != NULL )
        {
            
            CFStringRef sourceName = nil;
            
            if ( MIDIObjectGetStringProperty( sourceEndpoint, kMIDIPropertyDisplayName, &sourceName) == noErr )
            {
                [m_keysController logMessage:[NSString stringWithFormat:@"Found Source: %@", (__bridge NSString*)sourceName]
                                  atLogLevel:KeysControllerLogLevelInfo];
                
                /*
                UIAlertView * _alertView = [[UIAlertView alloc] initWithTitle:@"Source"
                                                                      message:[NSString stringWithFormat:@"s=%@",((__bridge NSString*)sourceName)]
                                                                     delegate:self
                                                            cancelButtonTitle:@"OK"
                                                            otherButtonTitles:nil];
                [_alertView show];
                */
                
                // Only connect the 'Keys' device for now
                // nanoKEY KEYBOARD
                // Akai MPK25 Port 1
                if ( [((__bridge NSString*)sourceName) isEqualToString:@"Akai MPK25 Port 1"] == YES )
                {
                
                    // connect source
                    OSStatus oss = MIDIPortConnectSource(m_pMidiInputPort, sourceEndpoint, (__bridge void *)(self));
                    
                    if ( oss != 0 )
                    {
                        [m_keysController logMessage:[NSString stringWithFormat:@"Failed to connect Keys source: Error code %ld", oss]
                                          atLogLevel:KeysControllerLogLevelError];
                    }
                    else
                    {
                        [m_midiSources addObject:[NSValue valueWithPointer:sourceEndpoint]];
                        
                        [m_keysController logMessage:[NSString stringWithFormat:@"Keys source connected"]
                                          atLogLevel:KeysControllerLogLevelInfo];
                        
                        m_sourceConnected = YES;
                    }
                }
            }
        }
    }
    
    return sourceCount;
}

- (int)updateDestinations
{
    int destinationCount = MIDIGetNumberOfDestinations();
    
    [m_midiDestinations removeAllObjects];
    
    m_destinationConnected = NO;
    
    for ( int i = 0; i < destinationCount; i++)
    {
        
        MIDIEndpointRef destinationEndpoint = MIDIGetDestination(i);
        
        if ( destinationEndpoint != NULL )
        {
            
            CFStringRef destinationName = nil;
            
            if ( MIDIObjectGetStringProperty(destinationEndpoint, kMIDIPropertyDisplayName, &destinationName) == noErr )
            {
                [m_keysController logMessage:[NSString stringWithFormat:@"Found Destination: %@", (__bridge NSString*)destinationName]
                                  atLogLevel:KeysControllerLogLevelInfo];
                
                /*
                UIAlertView * _alertView = [[UIAlertView alloc] initWithTitle:@"Destination"
                                                                      message:[NSString stringWithFormat:@"s=%@",((__bridge NSString*)destinationName)]
                                                                     delegate:self
                                                            cancelButtonTitle:@"OK"
                                                            otherButtonTitles:nil];
                [_alertView show];
                */
                
                // Only connect the 'Keys' destination for now
                // nanoKEY CTRL
                // Akai MPK25 Port 2
                if ( [((__bridge NSString*)destinationName) isEqualToString:@"Akai MPK25 Port 2"] == YES )
                {
                    [m_midiDestinations addObject:[NSValue valueWithPointer:destinationEndpoint]];
                    
                    [m_keysController logMessage:[NSString stringWithFormat:@"Keys destination connected"]
                                      atLogLevel:KeysControllerLogLevelInfo];
                    
                    m_destinationConnected = YES;
                }
            }
        }
    }
    
    return destinationCount;
}

#pragma mark - C-style callbacks

void KeysMIDIStateChangedHandler(const MIDINotification *message, void *refCon)
{
    
    KeysCoreMidiInterface * coreMidiInterface = (__bridge KeysCoreMidiInterface *)(refCon);
    
    switch ( message->messageID )
    {
        case kMIDIMsgSetupChanged:
        {
            [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Setup Changed"]
                                                atLogLevel:KeysControllerLogLevelInfo];
            
            [coreMidiInterface updateEndpoints];
        } break;
            
        case kMIDIMsgObjectAdded:
        {
            MIDIObjectAddRemoveNotification * messageAdd = (MIDIObjectAddRemoveNotification*)(message);
            
            if ( messageAdd->childType == kMIDIObjectType_Source )
            {
                [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Source Object Added"]
                                                    atLogLevel:KeysControllerLogLevelInfo];
            }
            else if ( messageAdd->childType == kMIDIObjectType_Destination )
            {
                [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Destination Object Added"]
                                                    atLogLevel:KeysControllerLogLevelInfo];
            }
            
        } break;
            
        case kMIDIMsgObjectRemoved:
        {
            MIDIObjectAddRemoveNotification * messageRemove = (MIDIObjectAddRemoveNotification*)(message);
            
            if ( messageRemove->childType == kMIDIObjectType_Source )
            {
                [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Source Object Removed"]
                                                    atLogLevel:KeysControllerLogLevelInfo];
            }
            else if( messageRemove->childType == kMIDIObjectType_Destination )
            {
                [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Destination Object Removed"]
                                                    atLogLevel:KeysControllerLogLevelInfo];
            }
            
        } break;
            
        case kMIDIMsgPropertyChanged:
        {
            [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Property Changed"]
                                                atLogLevel:KeysControllerLogLevelInfo];
        } break;
            
        case kMIDIMsgThruConnectionsChanged:
        {
            [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Thru Connections Changed"]
                                                atLogLevel:KeysControllerLogLevelInfo];
        } break;
            
        case kMIDIMsgSerialPortOwnerChanged:
        {
            [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Serial Port Owner Changed"]
                                                atLogLevel:KeysControllerLogLevelInfo];
        } break;
            
        case kMIDIMsgIOError:
        {
            [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"IO Error"]
                                                atLogLevel:KeysControllerLogLevelError];
        } break;
    }
}

void KeysMIDIReadHandler(const MIDIPacketList *pPacketList, void *pReadProcCon, void *pSrcConnCon)
{
    
    KeysCoreMidiInterface * coreMidiInterface = (__bridge KeysCoreMidiInterface *)(pSrcConnCon);
    
    for ( int i = 0; i < pPacketList->numPackets; i++ )
    {
        
        MIDIPacket packet = pPacketList->packet[i];
        
        if ( packet.length == 3 || packet.length == 4 )
        {
            [coreMidiInterface.m_keysController midiCallbackHandler:(char*)packet.data];
        }
        else
        {
            [coreMidiInterface.m_keysController logMessage:[NSString stringWithFormat:@"Invalid Midi packet size: %u", packet.length]
                                                atLogLevel:KeysControllerLogLevelError];
        }
    }
}

// This is called when a Send has completed
void KeysMIDICompletionHander(MIDISysexSendRequest *request)
{
    
    // If the interface gets torn down below us, this is set to null
    if(request->completionRefCon == NULL)
    {
        // This will apply to the data buffer as well since they are alloc'd in one malloc
        free(request);
        
        return;
    }
    
    KeysCoreMidiInterface * coreMidiInterface = (__bridge KeysCoreMidiInterface *)(request->completionRefCon);
    
    // Send any pending packets
    [coreMidiInterface processSendQueue];
    
}

#pragma mark - Send Primitives

- (BOOL)sendBuffer:(unsigned char*)buffer withLength:(int)bufferLength {
    // Create the packet
    MIDIPacket packet;
    
    packet.timeStamp = 0;
    packet.length = bufferLength;
    
    for ( int i = 0; i < bufferLength; i++ )
    {
        packet.data[i] = buffer[i];
    }
    
    // Create the packet list
    MIDIPacketList packetList;
    
    packetList.numPackets = 1;
    packetList.packet[0] = packet;
    
    // Broadcast it on all endpoints for now
    for ( NSValue * destPtr in m_midiDestinations )
    {
        
        MIDIEndpointRef endpoint = (MIDIEndpointRef)[destPtr pointerValue];
        
        // Send the packet list
        OSStatus oss = MIDISend(m_pMidiOutputPort, endpoint, &packetList);
        
        if ( oss == -1 )
        {
            [m_keysController logMessage:[NSString stringWithFormat:@"SendBuffer: MIDISend failed with status 0x%lx", oss]
                              atLogLevel:KeysControllerLogLevelError];
            
            return NO;
        }
    }
    
    return YES;
}

- (BOOL)sendSyncSysExBuffer:(unsigned char*)buffer withLength:(int)bufferLength {
    
    // There is currently only a single destination in this loop
    for ( NSValue * destPtr in m_midiDestinations ) {
        // Allocate the packet and the buffer simultaneously
        unsigned char * allocationBuffer = (unsigned char*)malloc(sizeof(MIDISysexSendRequest) + bufferLength*sizeof(char));
        MIDISysexSendRequest * sendRequest = (MIDISysexSendRequest*)allocationBuffer;
        unsigned char * sendBuffer = (unsigned char*)&sendRequest[1];
        
        // Each packet needs it's own copy -- in the future we could remove this step
        // by referencing the same buffer, but multiple destinations can't even occur
        // in the current implementation. Also the copy is cheap and we need to alloc
        // the packet anyways.
        memcpy(sendBuffer, buffer, bufferLength);
        
        sendRequest->destination = (MIDIEndpointRef)[destPtr pointerValue];
        sendRequest->data = sendBuffer;
        sendRequest->bytesToSend = bufferLength;
        sendRequest->complete = false;
        
        sendRequest->completionProc = KeysMIDICompletionHander;
        sendRequest->completionRefCon = (__bridge void *)(self);
        
        NSValue * sendPtr = [NSValue valueWithPointer:sendRequest];
        
        [m_sendQueue addObject:sendPtr];
        
        // If nothing else is on the queue, send this now
        if ( [m_sendQueue count] == 1 ) {
            OSStatus oss = MIDISendSysex(sendRequest);
            
            if ( oss == -1 ) {
                [m_keysController logMessage:[NSString stringWithFormat:@"SendSysExBuffer: MIDISend failed with status 0x%lx", oss]
                                  atLogLevel:KeysControllerLogLevelError];
                
                return NO;
            }
        }
    }
    
    
    return YES;
}

- (BOOL)sendSysExBuffer:(unsigned char*)buffer withLength:(int)bufferLength {
    @synchronized(m_sendQueue) {
        // There is currently only a single destination in this loop
        for ( NSValue * destPtr in m_midiDestinations ) {
            // Allocate the packet and the buffer simultaneously
            unsigned char * allocationBuffer = (unsigned char*)malloc(sizeof(MIDISysexSendRequest) + bufferLength*sizeof(char));
            
            MIDISysexSendRequest * sendRequest = (MIDISysexSendRequest*)allocationBuffer;
            
            unsigned char * sendBuffer = (unsigned char*)&sendRequest[1];
            
            // Each packet needs it's own copy -- in the future we could remove this step
            // by referencing the same buffer, but multiple destinations can't even occur
            // in the current implementation. Also the copy is cheap and we need to alloc
            // the packet anyways.
            memcpy(sendBuffer, buffer, bufferLength);
            
            sendRequest->destination = (MIDIEndpointRef)[destPtr pointerValue];
            sendRequest->data = sendBuffer;
            sendRequest->bytesToSend = bufferLength;
            sendRequest->complete = false;
            
            sendRequest->completionProc = KeysMIDICompletionHander;
            sendRequest->completionRefCon = (__bridge void *)(self);
            
            NSValue * sendPtr = [NSValue valueWithPointer:sendRequest];
            
            [m_sendQueue addObject:sendPtr];
            
            // If nothing else is on the queue, send this now
            if ( [m_sendQueue count] == 1 ) {
                OSStatus oss = MIDISendSysex(sendRequest);
                
                if ( oss == -1 ) {
                    UIAlertView *err = [[UIAlertView alloc] initWithTitle:@"error" message:@"sendsysex failed" delegate:self cancelButtonTitle:@"ok" otherButtonTitles:NULL, nil];
                    [err show];
                    
                    [m_keysController logMessage:[NSString stringWithFormat:@"SendSysExBuffer: MIDISend failed with status 0x%lx", oss]
                                      atLogLevel:KeysControllerLogLevelError];
                    
                    return NO;
                }
            }
        }
    }
    
    return YES;
}

#pragma mark - Set Hardware Effects

// Hardware effects
- (BOOL)sendSetNoteActiveRed:(unsigned char)red andGreen:(unsigned char) green andBlue:(unsigned char)blue
{
    
    int sendBufferLength = 5;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_SET_NOTE_ACTIVE;
    sendBuffer[3] = [self encodeValueWithRed:red andGreen:green andBlue:blue andMessage:0];
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendSetNoteActive: Failed to send SysEx Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendSetKeyFollowRed:(unsigned char)red
                    andGreen:(unsigned char)green
                     andBlue:(unsigned char)blue
{
    
    int sendBufferLength = 5;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_SET_FRET_FOLLOW;
    sendBuffer[3] = [self encodeValueWithRed:red andGreen:green andBlue:blue andMessage:0];
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    if ( result == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"SendSetFretFollow: Failed to send SysEx Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

#pragma mark - State Requests

- (BOOL)sendRequestBatteryStatus {
    
    int sendBufferLength = 3;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_REQ_BAT_STATUS;
    //sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"SendRequestBatteryStatus: Failed to send SysEx Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestSerialNumber:(unsigned char)byte
{
    unsigned char SysExBuffer[] = {
        0xF0,
        KEYS_DEVICE_ID,
        KEYS_REQUEST_SERIAL_NUMBER,
        (unsigned char)byte, // ByteNum
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    [m_keysController SetPendingRequest];
    
    BOOL result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n];
    
    if ( result == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"SendRequestSerialNumber: Failed to send SysEx Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestKeysRange
{
    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_REQUEST_KEYS_RANGE;
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    if ( result == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"SendRequestKeysRange: Failed to request Keys Range"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

- (BOOL)sendEnableDebug {
    int sendBufferLength = 3;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_DBG_ENABLE;
    //sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendEnableDebug: Failed to send SysEx Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
    
}

- (BOOL)sendDisableDebug {
    int sendBufferLength = 3;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_DBG_DISABLE;
    //sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"SendDisableDebug: Failed to send SysEx Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestCommitUserspace {
    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_COMMIT_USERSPACE;
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    [m_keysController SetPendingRequest];
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"sendRequestCommitUserspace: Failed to send SysEx Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestResetUserspace {
    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_RESET_USERSPACE;
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    [m_keysController SetPendingRequest];
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    if ( result == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"sendRequestResetUserspace: Failed to send SysEx Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

#pragma mark - Firmware and Certs

- (BOOL)sendRequestCertDownload
{
    
    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_REQ_CERT_DOWNLOAD;
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendRequestCertDownload: Failed to send SysEx Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendRequestFirmwareVersion
{
    
    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_REQ_FW_VERSION;
    //pSendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendRequestFirmwareVersion: Failed to send SysEx Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendFirmwarePackagePage:(unsigned char*)page
                    andPageSize:(int)pageSize
                andFirmwareSize:(int)firmwareSize
                   andPageCount:(int)pageCount
                 andCurrentPage:(int)currentPage
                    andChecksum:(unsigned char)checksum
{
    
    // All data bytes must be converted to midi data bytes which have a zero for the MSB
    int j = 0;
    unsigned char tempBuffer[pageSize*2];
    
    memset(tempBuffer, 0, pageSize * 2);
    
    signed char startCounter = 1;
    signed char endCounter = 6;
    
    for ( int i = 0; i < pageSize; i++ )
    {
        // add current fragment and begining of next
        tempBuffer[j] += (page[i] >> startCounter) & 0x7F;
        tempBuffer[j + 1] = (0x7F & (page[i] << endCounter));
        
        // var upkeep
        j += 1;
        startCounter += 1;
        endCounter -= 1;
        
        // boundary check
        if ( startCounter == 8 && endCounter < 0 )
        {
            j++;
            startCounter = 1;
            endCounter = 6;
        }
    }
    
    j++;
    
    unsigned char sendBuffer[pageSize*2];
    
    int sendBufferLength = j + 14;
    
    memset(sendBuffer, 0, pageSize);
    
    sendBuffer[0] = 0xF0; //SysEx
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_DWLD_FW_PAGE;
    sendBuffer[3] = (firmwareSize & 0xFF0000) >> 16;
    sendBuffer[4] = (firmwareSize & 0x00FF00) >> 8;
    sendBuffer[5] = (firmwareSize & 0x0000FF) >> 0;
    sendBuffer[6] = (unsigned char)pageCount;
    sendBuffer[7] = (unsigned char)currentPage;
    sendBuffer[8] = 0;
    sendBuffer[9] = (j & 0xFF00) >> 8;
    sendBuffer[10] = (j & 0x00FF);
    sendBuffer[11] = 0x00;
    
    memcpy(sendBuffer + 12, tempBuffer, j);
    
    sendBuffer[j + 12] = (checksum >> 1) & 0x7F;;
    sendBuffer[j + 13] = 0xF7;
    
    NSLog(@"%x %x %x   %x %x %x   %x %x %x   %x %x %x",
          sendBuffer[0], sendBuffer[1], sendBuffer[2], sendBuffer[3],
          sendBuffer[4], sendBuffer[5], sendBuffer[6], sendBuffer[7],
          sendBuffer[8], sendBuffer[9], sendBuffer[10], sendBuffer[11] );
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendFirmwarePackagePage: Failed to send SysEx Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

#pragma mark - Piezo Functions
/*
- (BOOL)sendRequestPiezoSensitivityString:(unsigned char)str {
    BOOL result = false;
    
    if([m_keysController IsPendingSensitivityValue]) {
        [m_keysController logMessage:[NSString stringWithFormat:@"sendRequestPiezoSensitivityString: String sens currently pending"] atLogLevel:KeysControllerLogLevelError];
        return false;
    }
    
    unsigned char SysExBuffer[] = {
        0xF0,
        KEYS_DEVICE_ID,
        KEYS_GET_PIEZO_SENSITIVITY,
        (unsigned char)(str),
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    [m_keysController SetPendingSensitivityValueString:str];
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"sendRequestPiezoSensitivityString: Failed to send Buffer"] atLogLevel:KeysControllerLogLevelError];
    else
        [m_keysController logMessage:[NSString stringWithFormat:@"sendRequestPiezoSensitivityString: Sent Request"] atLogLevel:KeysControllerLogLevelInfo];
    
    return result;
}

- (BOOL)sendRequestPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column {
    BOOL result = false;
    
    if([m_keysController IsPendingMatrixValue]) {
        [m_keysController logMessage:[NSString stringWithFormat:@"sendPiezoCrossTalkMatrixRow: Ratio currently pending"] atLogLevel:KeysControllerLogLevelError];
        return false;
    }
    
    unsigned char RowColValue = ((row & 0x0F) << 4) + (column & 0x0F);
    
    [m_keysController SetPendingMatrixValueRow:row col:column];
    
    unsigned char SysExBuffer[] = {
        0xF0,
        KEYS_DEVICE_ID,
        KEYS_GET_PIEZO_CT_MATRIX,
        (unsigned char)(RowColValue),
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"sendPiezoCrossTalkMatrixRow: Failed to send Buffer"] atLogLevel:KeysControllerLogLevelError];
    else
        [m_keysController logMessage:[NSString stringWithFormat:@"sendPiezoCrossTalkMatrixRow: Sent Request"] atLogLevel:KeysControllerLogLevelInfo];
    
    return result;
}

- (BOOL)sendRequestPiezoWindowIndex:(unsigned char)index {
    BOOL result = false;
    
    unsigned char SysExBuffer[] = {
        0xF0,
        KEYS_DEVICE_ID,
        KEYS_GET_PIEZO_WINDOW,
        (unsigned char)(index),
        0xF7
    };
    
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"sendRequestPiezoWindowIndex: Failed to send Buffer"] atLogLevel:KeysControllerLogLevelError];
    else
        [m_keysController logMessage:[NSString stringWithFormat:@"sendRequestPiezoWindowIndex: Sent Request"] atLogLevel:KeysControllerLogLevelInfo];
    
    return result;
}

- (BOOL)sendPiezoSensitivityString:(unsigned char)str thresh:(unsigned char)thresh {
    BOOL result = false;
    
    unsigned char SysExBuffer[] = {
        0xF0,
        KEYS_DEVICE_ID,
        KEYS_SET_PIEZO_SENSITIVITY,
        (unsigned char)(str),
        (unsigned char)(thresh),
        0xF7
    };
    
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"sendPiezoSensitivityString: Failed to send Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

-(BOOL)sendPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column value:(unsigned char)value {
    BOOL result = false;
    
    unsigned char RowColValue = ((row & 0x0F) << 4) + (column & 0x0F);
    unsigned char SysExBuffer[] = {
        0xF0,
        KEYS_DEVICE_ID,
        KEYS_SET_PIEZO_CT_MATRIX,
        (unsigned char)(RowColValue),
        (unsigned char)(value),
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"sendPiezoCrossTalkMatrixRow: Failed to send Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}

-(BOOL) sendPiezoWindowIndex:(unsigned char)index value:(unsigned char)value {
    BOOL result = false;
    
    unsigned char SysExBuffer[] = {
        0xF0,
        KEYS_DEVICE_ID,
        KEYS_SET_PIEZO_WINDOW,
        (unsigned char)(index),
        (unsigned char)(value),
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_keysController logMessage:[NSString stringWithFormat:@"sendPiezoWindowIndex: Failed to send Buffer"] atLogLevel:KeysControllerLogLevelError];
    
    return result;
}
*/

#pragma mark - Set LED State

- (BOOL)sendCCSetLedStatusKey:(unsigned char)key
                        andRed:(unsigned char)red
                      andGreen:(unsigned char)green
                       andBlue:(unsigned char)blue
                    andMessage:(unsigned char)message
{
    
    int sendBufferLength = 2; //3;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xB0 + (key & 0xF);
    sendBuffer[1] = 0x33;
    //sendBuffer[2] = fret;
    
    BOOL result = [self sendBuffer:sendBuffer withLength:sendBufferLength];
    
    if( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendCCSetLEDState: Failed to send Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
        
        return result;
    }
    
    sendBuffer[0] = 0xB0 + (key & 0xF);
    sendBuffer[1] = 0x34;
    sendBuffer[2] = [self encodeValueWithRed:red andGreen:green andBlue:blue andMessage:message];
    
    result = [self sendBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendCCSetLEDState: Failed to send Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendSetLedStateKey:(unsigned char)key
                     andRed:(unsigned char)red
                   andGreen:(unsigned char)green
                    andBlue:(unsigned char)blue
                 andMessage:(unsigned char)message
{
    
    int sendBufferLength = 7;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = KEYS_DEVICE_ID;
    sendBuffer[2] = (unsigned char)KEYS_MSG_SET_LED;
    sendBuffer[3] = (unsigned char)key;
//    sendBuffer[4] = (unsigned char)fret;
    sendBuffer[5] = [self encodeValueWithRed:red andGreen:green andBlue:blue andMessage:message];
    sendBuffer[6] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendSetLEDState: Failed to send SysEx Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendNoteMessageOnChannel:(unsigned char)channel
                    andMidiValue:(unsigned char)midiVal
                      andMidiVel:(unsigned char)midiVel
                         andType:(const char*)type
{
    
    int sendBufferLength = 3;
    unsigned char sendBuffer[sendBufferLength];
    
    // Type is either "off" or "on"
    if ( strcmp( type, "on") == 0 )
    {
        sendBuffer[0] = 0x90;
    }
    else
    {
        sendBuffer[0] = 0x80;
    }
    
    sendBuffer[0] += (channel & 0xF);
    sendBuffer[1] = midiVal;
    sendBuffer[2] = midiVel;
    
    BOOL result = [self sendBuffer:sendBuffer withLength:sendBufferLength];
    
    if( result == NO )
    {
        [m_keysController logMessage:[NSString stringWithFormat:@"SendNoteMsg: Failed to send Buffer"]
                          atLogLevel:KeysControllerLogLevelError];
    }
    
    return result;
}

#pragma mark - Helpers

- (unsigned char)encodeValueWithRed:(unsigned char)red
                           andGreen:(unsigned char)green
                            andBlue:(unsigned char)blue
                         andMessage:(unsigned char)message
{
    unsigned char retVal = 0;
    
    retVal += ((red & 0x3) << 6);
    retVal += ((green & 0x3) << 4);
    retVal += ((blue & 0x3) << 2);
    retVal += ((message & 0x3) << 0);
    
    return retVal;
}

@end
