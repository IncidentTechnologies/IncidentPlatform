//
//  CloudRequest3.m
//  gTarAppCore
//
//  Created by Marty Greenia on 4/24/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "CloudRequest.h"


@implementation CloudRequest

@synthesize m_cloudResponse;
@synthesize m_callbackObject;
@synthesize m_callbackSelector;
@synthesize m_type;
@synthesize m_status;
@synthesize m_isSynchronous;
@synthesize m_facebookAccessToken;
@synthesize m_username;
@synthesize m_password;
@synthesize m_email;
@synthesize m_cookie;
@synthesize m_fileId;
@synthesize m_userId;
@synthesize m_page;
@synthesize m_searchString;
@synthesize m_creditCode;
@synthesize m_userSong;
@synthesize m_userSongSession;
@synthesize m_itunesReceipt;
@synthesize m_profileImage;
@synthesize m_logEntries;
@synthesize m_versionString;
@synthesize m_deviceString;
@synthesize m_appString;

@synthesize m_appId;
@synthesize m_activationKey;
@synthesize m_permissionLevel;
@synthesize m_rememberLogin;
@synthesize m_xmpId;
@synthesize m_folderId;
@synthesize m_xmpFile;
@synthesize m_xmpType;
@synthesize m_xmpData;
@synthesize m_xmpOnly;

@synthesize m_serial_lower;
@synthesize m_serial_upper;

- (id)initWithType:(CloudRequestType)type
{
    
    self = [super init];
    
	if ( self )
	{

        m_status = CloudRequestStatusPending;
        m_type = type;
        m_isSynchronous = YES;
        
    }
    
    return self;
    
}

- (id)initWithType:(CloudRequestType)type andCallbackObject:(id)obj andCallbackSelector:(SEL)sel
{
    
    self = [super init];
    
	if ( self )
	{
        m_status = CloudRequestStatusPending;
        m_type = type;
        
        // Nil obj/sel are assumed to mean sync
        if ( obj == nil || sel == nil )
        {
            m_isSynchronous = YES;
        }
        else
        {
            m_isSynchronous = NO;
            
            m_callbackObject = obj;
            m_callbackSelector = sel;
        }

    }
    
    return self;
    
}

- (void)dealloc
{
	
	//[m_callbackObject release];
    
    
	m_status = CloudRequestStatusUnknown;
	
    /*[m_facebookAccessToken release];
    [m_username release];
    [m_password release];
    [m_email release];
    [m_cookie release];
    [m_searchString release];
    [m_creditCode release];
    [m_userSong release];
    [m_userSongSession release];
    [m_itunesReceipt release];
    [m_profileImage release];
    [m_logEntries release];
    [m_versionString release];
    [m_deviceString release];
    [m_appString release];
    
	[super dealloc];
	*/
}

@end
