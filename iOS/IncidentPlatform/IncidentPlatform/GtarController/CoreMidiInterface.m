//
//  CoreMidiInterface.m
//  GtarController
//
//  Created by Marty Greenia on 9/26/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//

#import "CoreMidiInterface.h"

#import "GtarControllerInternal.h"

#import <UIKit/UIKit.h>

@implementation CoreMidiInterface

@synthesize m_gtarController;
@synthesize m_connected;

- (id)initWithGtarController:(GtarController*)gtarController
{
    self = [super init];
    
    if ( self ) {
        m_gtarController = gtarController;
        [m_gtarController logMessage:[NSString stringWithFormat:@"CoreMidiInterface initializing"]
                          atLogLevel:GtarControllerLogLevelInfo];
        
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
        OSStatus oss = MIDIClientCreate(CFSTR("AE Midi Client"), MIDIStateChangedHandler, (__bridge void *)(self), &m_pMidiClient);
        
        if( oss != 0 )
        {
            [m_gtarController logMessage:[NSString stringWithFormat:@"Failed to create AE Midi Client: Error %ld", oss]
                              atLogLevel:GtarControllerLogLevelError];

        }
        
        // Set up the Input Port
        oss = MIDIInputPortCreate(m_pMidiClient, (CFStringRef)@"Midi Client Input Port", MIDIReadHandler, (__bridge void *)(self), &m_pMidiInputPort);
        
        if( oss != 0 )
        {
            [m_gtarController logMessage:[NSString stringWithFormat:@"Failed to create input port: Error %ld", oss]
                              atLogLevel:GtarControllerLogLevelError];
        }
        
        // Set up the Output Port
        oss = MIDIOutputPortCreate(m_pMidiClient, (CFStringRef)@"MidiClient Output Port", &m_pMidiOutputPort);
        
        if( oss != 0 )
        {
            [m_gtarController logMessage:[NSString stringWithFormat:@"Failed to create output port: Error %ld", oss]
                              atLogLevel:GtarControllerLogLevelError];
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
            [m_gtarController logMessage:[NSString stringWithFormat:@"Failed to dispose client: Error code %ld", oss]
                              atLogLevel:GtarControllerLogLevelError];
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
                [m_gtarController logMessage:[NSString stringWithFormat:@"SendSysExBuffer: MIDISend failed with status 0x%lx", oss]
                                                    atLogLevel:GtarControllerLogLevelError];
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
    
    [m_gtarController logMessage:[NSString stringWithFormat:@"Found %d sources and %d destinations", sourceCount, destinationCount]
                      atLogLevel:GtarControllerLogLevelInfo];
    
    // Are we connected to something
    m_connected = (m_sourceConnected && m_destinationConnected);
    
    // See if the connection status changed from connected to !connected or vice versa
    if ( previousConnected == YES )
    {
        if ( m_connected == NO)
        {
//            [m_gtarController logMessage:[NSString stringWithFormat:@"Gtar Disconnected"]
//                              atLogLevel:GtarControllerLogLevelInfo];
            
            [m_gtarController midiConnectionHandler:NO];
        }
    }
    else
    {
        if ( m_connected == YES )
        {
//            [m_gtarController logMessage:[NSString stringWithFormat:@"Gtar Connected"]
//                              atLogLevel:GtarControllerLogLevelInfo];
            
            [m_gtarController midiConnectionHandler:YES];
        }
    }
    
}

- (int)updateSources
{
    
    int sourceCount = MIDIGetNumberOfSources();
    
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
                [m_gtarController logMessage:[NSString stringWithFormat:@"Found Source: %@", (__bridge NSString*)sourceName]
                                  atLogLevel:GtarControllerLogLevelInfo];
                
                // Only connect the 'gTar' device for now
                if ( [((__bridge NSString*)sourceName) isEqualToString:@"gTar"] == YES )
                {
                    
                    // connect source
                    OSStatus oss = MIDIPortConnectSource(m_pMidiInputPort, sourceEndpoint, (__bridge void *)(self));
                    
                    if ( oss != 0 )
                    {
                        [m_gtarController logMessage:[NSString stringWithFormat:@"Failed to connect gTar source: Error code %ld", oss]
                                          atLogLevel:GtarControllerLogLevelError];
                    }
                    else
                    {
                        [m_midiSources addObject:[NSValue valueWithPointer:sourceEndpoint]];
                        
                        [m_gtarController logMessage:[NSString stringWithFormat:@"Gtar source connected"]
                                          atLogLevel:GtarControllerLogLevelInfo];
                        
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
                [m_gtarController logMessage:[NSString stringWithFormat:@"Found Destination: %@", (__bridge NSString*)destinationName]
                                  atLogLevel:GtarControllerLogLevelInfo];
                
                // Only connect the 'gTar' destination for now
                if ( [((__bridge NSString*)destinationName) isEqualToString:@"gTar"] == YES )
                {
                    [m_midiDestinations addObject:[NSValue valueWithPointer:destinationEndpoint]];
                    
                    [m_gtarController logMessage:[NSString stringWithFormat:@"Gtar destination connected"]
                                      atLogLevel:GtarControllerLogLevelInfo];
                    
                    m_destinationConnected = YES;
                }
            }
        }
    }
    
    return destinationCount;
}

#pragma mark - C-style callbacks

void MIDIStateChangedHandler(const MIDINotification *message, void *refCon)
{
    
    CoreMidiInterface * coreMidiInterface = (__bridge CoreMidiInterface *)(refCon);
    
    switch ( message->messageID )
    {
        case kMIDIMsgSetupChanged:
        {
            [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Setup Changed"]
                                                atLogLevel:GtarControllerLogLevelInfo];
            
            [coreMidiInterface updateEndpoints];
        } break;
            
        case kMIDIMsgObjectAdded:
        {
            MIDIObjectAddRemoveNotification * messageAdd = (MIDIObjectAddRemoveNotification*)(message);
            
            if ( messageAdd->childType == kMIDIObjectType_Source )
            {
                [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Source Object Added"]
                                                    atLogLevel:GtarControllerLogLevelInfo];
            }
            else if ( messageAdd->childType == kMIDIObjectType_Destination )
            {
                [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Destination Object Added"]
                                                    atLogLevel:GtarControllerLogLevelInfo];
            }
            
        } break;
            
        case kMIDIMsgObjectRemoved:
        {
            MIDIObjectAddRemoveNotification * messageRemove = (MIDIObjectAddRemoveNotification*)(message);
            
            if ( messageRemove->childType == kMIDIObjectType_Source )
            {
                [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Source Object Removed"]
                                                    atLogLevel:GtarControllerLogLevelInfo];
            }
            else if( messageRemove->childType == kMIDIObjectType_Destination )
            {
                [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Destination Object Removed"]
                                                    atLogLevel:GtarControllerLogLevelInfo];
            }
            
        } break;
            
        case kMIDIMsgPropertyChanged:
        {
            [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Property Changed"]
                                                atLogLevel:GtarControllerLogLevelInfo];
        } break;
            
        case kMIDIMsgThruConnectionsChanged:
        {
            [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Thru Connections Changed"]
                                                atLogLevel:GtarControllerLogLevelInfo];
        } break;
            
        case kMIDIMsgSerialPortOwnerChanged:
        {
            [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Serial Port Owner Changed"]
                                                atLogLevel:GtarControllerLogLevelInfo];
        } break;
            
        case kMIDIMsgIOError:
        {
            [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"IO Error"]
                                                atLogLevel:GtarControllerLogLevelError];
        } break;
    }
}

void MIDIReadHandler(const MIDIPacketList *pPacketList, void *pReadProcCon, void *pSrcConnCon)
{
    
    CoreMidiInterface * coreMidiInterface = (__bridge CoreMidiInterface *)(pSrcConnCon);
    
    for ( int i = 0; i < pPacketList->numPackets; i++ )
    {
        
        MIDIPacket packet = pPacketList->packet[i];
        
        if ( packet.length == 3 || packet.length == 4 )
        {
            [coreMidiInterface.m_gtarController midiCallbackHandler:(char*)packet.data];
        }
        else
        {
            [coreMidiInterface.m_gtarController logMessage:[NSString stringWithFormat:@"Invalid Midi packet size: %u", packet.length]
                                                atLogLevel:GtarControllerLogLevelError];
        }
    }
}

// This is called when a Send has completed
void MIDICompletionHander(MIDISysexSendRequest *request)
{
    
    // If the interface gets torn down below us, this is set to null
    if(request->completionRefCon == NULL)
    {
        // This will apply to the data buffer as well since they are alloc'd in one malloc
        free(request);
        
        return;
    }
    
    CoreMidiInterface * coreMidiInterface = (__bridge CoreMidiInterface *)(request->completionRefCon);
    
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
            [m_gtarController logMessage:[NSString stringWithFormat:@"SendBuffer: MIDISend failed with status 0x%lx", oss]
                                                atLogLevel:GtarControllerLogLevelError];
            
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
        
        sendRequest->completionProc = MIDICompletionHander;
        sendRequest->completionRefCon = (__bridge void *)(self);
        
        NSValue * sendPtr = [NSValue valueWithPointer:sendRequest];
        
        [m_sendQueue addObject:sendPtr];
        
        // If nothing else is on the queue, send this now
        if ( [m_sendQueue count] == 1 ) {
            OSStatus oss = MIDISendSysex(sendRequest);
            
            if ( oss == -1 ) {
                [m_gtarController logMessage:[NSString stringWithFormat:@"SendSysExBuffer: MIDISend failed with status 0x%lx", oss]
                                  atLogLevel:GtarControllerLogLevelError];
                
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
            
            sendRequest->completionProc = MIDICompletionHander;
            sendRequest->completionRefCon = (__bridge void *)(self);
            
            NSValue * sendPtr = [NSValue valueWithPointer:sendRequest];
            
            [m_sendQueue addObject:sendPtr];
            
            // If nothing else is on the queue, send this now
            if ( [m_sendQueue count] == 1 ) {
                OSStatus oss = MIDISendSysex(sendRequest);
                
                if ( oss == -1 ) {
                    UIAlertView *err = [[UIAlertView alloc] initWithTitle:@"error" message:@"sendsysex failed" delegate:self cancelButtonTitle:@"ok" otherButtonTitles:NULL, nil];
                    [err show];
                    
                    [m_gtarController logMessage:[NSString stringWithFormat:@"SendSysExBuffer: MIDISend failed with status 0x%lx", oss]
                                      atLogLevel:GtarControllerLogLevelError];
                    
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
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_MSG_SET_NOTE_ACTIVE;
    sendBuffer[3] = [self encodeValueWithRed:red andGreen:green andBlue:blue andMessage:0];
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendSetNoteActive: Failed to send SysEx Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendSetFretFollowRed:(unsigned char)red
                    andGreen:(unsigned char)green
                     andBlue:(unsigned char)blue
{
    
    int sendBufferLength = 5;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_MSG_SET_FRET_FOLLOW;    
    sendBuffer[3] = [self encodeValueWithRed:red andGreen:green andBlue:blue andMessage:0];
    sendBuffer[4] = 0xF7; // End SysEx Message    
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    if ( result == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendSetFretFollow: Failed to send SysEx Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

#pragma mark - State Requests

- (BOOL)sendRequestBatteryStatus {
    
    int sendBufferLength = 3;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_MSG_REQ_BAT_STATUS;
    //sendBuffer[4] = 0xF7; // End SysEx Message  
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendRequestBatteryStatus: Failed to send SysEx Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestSerialNumber:(unsigned char)byte
{
    unsigned char SysExBuffer[] = {
        0xF0,
        GTAR_DEVICE_ID,
        GTAR_REQUEST_SERIAL_NUMBER,
        (unsigned char)byte, // ByteNum
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    [m_gtarController SetPendingRequest];
    
    BOOL result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n];
    
    if ( result == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendRequestSerialNumber: Failed to send SysEx Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendEnableDebug {
    int sendBufferLength = 3;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_MSG_DBG_ENABLE;
    //sendBuffer[4] = 0xF7; // End SysEx Message  
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendEnableDebug: Failed to send SysEx Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
    }
        
    return result;
    
}

- (BOOL)sendDisableDebug {
    int sendBufferLength = 3;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_MSG_DBG_DISABLE;
    //sendBuffer[4] = 0xF7; // End SysEx Message  
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendDisableDebug: Failed to send SysEx Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestCommitUserspace {
    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_COMMIT_USERSPACE;
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    [m_gtarController SetPendingRequest];
    
     BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendRequestCommitUserspace: Failed to send SysEx Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

- (BOOL)sendRequestResetUserspace {
    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_RESET_USERSPACE;
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    [m_gtarController SetPendingRequest];
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    if ( result == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendRequestResetUserspace: Failed to send SysEx Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

#pragma mark - Firmware and Certs

- (BOOL)sendRequestCertDownload
{

    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_MSG_REQ_CERT_DOWNLOAD;
    sendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendRequestCertDownload: Failed to send SysEx Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendRequestFirmwareVersion
{
    
    int sendBufferLength = 4;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_MSG_REQ_FW_VERSION;
    //pSendBuffer[4] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendRequestFirmwareVersion: Failed to send SysEx Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
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
    sendBuffer[1] = GTAR_DEVICE_ID;    
    sendBuffer[2] = (unsigned char)GTAR_MSG_DWLD_FW_PAGE;
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
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendFirmwarePackagePage: Failed to send SysEx Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

#pragma mark - Piezo Functions

- (BOOL)sendRequestPiezoSensitivityString:(unsigned char)str {
    BOOL result = false;
    
    if([m_gtarController IsPendingSensitivityValue]) {
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendRequestPiezoSensitivityString: String sens currently pending"] atLogLevel:GtarControllerLogLevelError];
        return false;
    }
    
    unsigned char SysExBuffer[] = {
        0xF0,
        GTAR_DEVICE_ID,
        GTAR_GET_PIEZO_SENSITIVITY,
        (unsigned char)(str),
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    [m_gtarController SetPendingSensitivityValueString:str];
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendRequestPiezoSensitivityString: Failed to send Buffer"] atLogLevel:GtarControllerLogLevelError];
    else
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendRequestPiezoSensitivityString: Sent Request"] atLogLevel:GtarControllerLogLevelInfo];
    
    return result;
}

- (BOOL)sendRequestPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column {
    BOOL result = false;
    
    if([m_gtarController IsPendingMatrixValue]) {
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendPiezoCrossTalkMatrixRow: Ratio currently pending"] atLogLevel:GtarControllerLogLevelError];
        return false;
    }
    
    unsigned char RowColValue = ((row & 0x0F) << 4) + (column & 0x0F);
    
    [m_gtarController SetPendingMatrixValueRow:row col:column];
    
    unsigned char SysExBuffer[] = {
        0xF0,
        GTAR_DEVICE_ID,
        GTAR_GET_PIEZO_CT_MATRIX,
        (unsigned char)(RowColValue),
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendPiezoCrossTalkMatrixRow: Failed to send Buffer"] atLogLevel:GtarControllerLogLevelError];
    else
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendPiezoCrossTalkMatrixRow: Sent Request"] atLogLevel:GtarControllerLogLevelInfo];
    
    return result;
}

- (BOOL)sendRequestPiezoWindowIndex:(unsigned char)index {
    BOOL result = false;
    
    unsigned char SysExBuffer[] = {
        0xF0,
        GTAR_DEVICE_ID,
        GTAR_GET_PIEZO_WINDOW,
        (unsigned char)(index),
        0xF7
    };
    
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendRequestPiezoWindowIndex: Failed to send Buffer"] atLogLevel:GtarControllerLogLevelError];
    else
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendRequestPiezoWindowIndex: Sent Request"] atLogLevel:GtarControllerLogLevelInfo];
    
    return result;
}

- (BOOL)sendPiezoSensitivityString:(unsigned char)str thresh:(unsigned char)thresh {
    BOOL result = false;
    
    unsigned char SysExBuffer[] = {
        0xF0,
        GTAR_DEVICE_ID,
        GTAR_SET_PIEZO_SENSITIVITY,
        (unsigned char)(str),
        (unsigned char)(thresh),
        0xF7
    };
    
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendPiezoSensitivityString: Failed to send Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

-(BOOL)sendPiezoCrossTalkMatrixRow:(unsigned char)row Column:(unsigned char)column value:(unsigned char)value {
    BOOL result = false;
    
    unsigned char RowColValue = ((row & 0x0F) << 4) + (column & 0x0F);
    unsigned char SysExBuffer[] = {
        0xF0,
        GTAR_DEVICE_ID,
        GTAR_SET_PIEZO_CT_MATRIX,
        (unsigned char)(RowColValue),
        (unsigned char)(value),
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendPiezoCrossTalkMatrixRow: Failed to send Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

-(BOOL) sendPiezoWindowIndex:(unsigned char)index value:(unsigned char)value {
    BOOL result = false;
    
    unsigned char SysExBuffer[] = {
        0xF0,
        GTAR_DEVICE_ID,
        GTAR_SET_PIEZO_WINDOW,
        (unsigned char)(index),
        (unsigned char)(value),
        0xF7
    };
    int SysExBuffer_n = sizeof(SysExBuffer)/sizeof(SysExBuffer[0]);
    
    if( (result = [self sendSysExBuffer:SysExBuffer withLength:SysExBuffer_n]) == NO )
        [m_gtarController logMessage:[NSString stringWithFormat:@"sendPiezoWindowIndex: Failed to send Buffer"] atLogLevel:GtarControllerLogLevelError];
    
    return result;
}

#pragma mark - Set LED State

- (BOOL)sendCCSetLedStatusFret:(unsigned char)fret
                     andString:(unsigned char)str
                        andRed:(unsigned char)red
                      andGreen:(unsigned char)green
                       andBlue:(unsigned char)blue
                    andMessage:(unsigned char)message
{
    
    int sendBufferLength = 3;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xB0 + (str & 0xF);
    sendBuffer[1] = 0x33;
    sendBuffer[2] = fret;
    
    BOOL result = [self sendBuffer:sendBuffer withLength:sendBufferLength];
    
    if( result == NO )
    {
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendCCSetLEDState: Failed to send Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
        
        return result;
    }
    
    sendBuffer[0] = 0xB0 + (str & 0xF);
    sendBuffer[1] = 0x34;
    sendBuffer[2] = [self encodeValueWithRed:red andGreen:green andBlue:blue andMessage:message];
    
    result = [self sendBuffer:sendBuffer withLength:sendBufferLength];
    
    if ( result == NO )
    {
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendCCSetLEDState: Failed to send Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
    }
    
    return result;
}

- (BOOL)sendSetLedStateFret:(unsigned char)fret
                  andString:(unsigned char)str
                     andRed:(unsigned char)red
                   andGreen:(unsigned char)green
                    andBlue:(unsigned char)blue
                 andMessage:(unsigned char)message
{

    int sendBufferLength = 7;
    unsigned char sendBuffer[sendBufferLength];
    
    sendBuffer[0] = 0xF0; // SysEx Message
    sendBuffer[1] = GTAR_DEVICE_ID;
    sendBuffer[2] = (unsigned char)GTAR_MSG_SET_LED;    
    sendBuffer[3] = (unsigned char)str;
    sendBuffer[4] = (unsigned char)fret;
    sendBuffer[5] = [self encodeValueWithRed:red andGreen:green andBlue:blue andMessage:message];
    sendBuffer[6] = 0xF7; // End SysEx Message
    
    BOOL result = [self sendSysExBuffer:sendBuffer withLength:sendBufferLength];
    
    if( result == NO )
    {
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendSetLEDState: Failed to send SysEx Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
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
        [m_gtarController logMessage:[NSString stringWithFormat:@"SendNoteMsg: Failed to send Buffer"]
                          atLogLevel:GtarControllerLogLevelError];
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
