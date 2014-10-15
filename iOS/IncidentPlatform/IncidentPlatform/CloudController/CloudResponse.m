//
//  CloudResponse.m
//  gTarAppCore
//
//  Created by Marty Greenia on 4/24/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "CloudResponse.h"

#import "UserProfile.h"
#import "UserProfiles.h"
#import "UserSongs.h"
#import "UserSong.h"
//#import "StoreFeatureCollection.h"
#import "XmlDom.h"
#import "CloudRequest.h"

@implementation CloudResponse


@synthesize m_cloudRequest;
@synthesize m_status;
@synthesize m_opho_status;
@synthesize m_statusCode;
@synthesize m_mimeType;
@synthesize m_statusText;
@synthesize m_loggedIn;

@synthesize m_receivedData;
@synthesize m_receivedDataString;
@synthesize m_responseXmlDom;

@synthesize m_responseFileId;
@synthesize m_responseUserId;
@synthesize m_responseUserCredits;
@synthesize m_responseUserProfile;
@synthesize m_responseUserProfiles;
@synthesize m_responseUserProfilesFollows;
@synthesize m_responseUserProfilesFollowedBy;
@synthesize m_responseUserSongs;
@synthesize m_responseUserSongSession;
@synthesize m_responseUserSongSessions;
@synthesize m_responseStoreFeatureCollection;
@synthesize m_responseProductIds;
@synthesize m_responseFirmwareMajorVersion;
@synthesize m_responseFirmwareMinorVersion;

@synthesize m_xmpList;
@synthesize m_xmpDom;
@synthesize m_id;

- (id)initWithCloudRequest:(CloudRequest*)cloudRequest
{
    
	self = [super init];
    
	if ( self )
	{
        // We don't retain the Response in this action to avoid loops
        cloudRequest.m_cloudResponse = self;
        
        m_cloudRequest = cloudRequest;
        
        self.m_statusText = @"Connection error";
    }
    
    return self;
    
}

@end
