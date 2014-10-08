//
//  CloudResponse.h
//  gTarAppCore
//
//  Created by Marty Greenia on 4/24/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

@class UserProfile;
@class UserProfiles;
@class UserSong;
@class UserSongs;
@class UserSongSession;
@class UserSongSessions;
@class NewsTicker;
@class StoreFeatureCollection;
@class XmlDom;
@class CloudRequest;

typedef enum
{
	CloudResponseStatusUnknown = 0,
	CloudResponseStatusSuccess,
	CloudResponseStatusFailure,
	CloudResponseStatusAuthenticationError,
	CloudResponseStatusConnectionError,
	CloudResponseStatusServerError,
    CloudResponseStatusItunesServerError,
    CloudResponseStatusOffline
} CloudResponseStatus;

typedef enum
{
    OphoCloudResponseStatusUnknown = 0,
    OphoCloudResponseStatusSuccess,
    OphoCloudResponseStatusUserError,
    OphoCloudResponseStatusServerError,
    OphoCloudResponseStatusInvalidParameter,
    OphoCloudResponseStatusInvalidUrl,
    OphoCloudResponseStatusUnauthorized,
    OphoCloudResponseStatusPaypalError,
    OphoCloudResponseStatusEmailError,
    OphoCloudResponseStatusDatabaseError,
    OphoCloudResponseStatusEmailWarning
} OphoCloudResponseStatus;

@interface CloudResponse : NSObject
{
    
    CloudRequest * m_cloudRequest;
    
    // Response info
	CloudResponseStatus m_status;
    OphoCloudResponseStatus m_opho_status;
    
    NSInteger m_statusCode;
    NSString * m_mimeType;
    NSString * m_statusText;
    
    BOOL m_loggedIn;
	
    // Response data
    NSMutableData * m_receivedData;
    NSString * m_receivedDataString;
    XmlDom * m_responseXmlDom;

    // Response objects
    NSInteger m_responseFileId;
    NSInteger m_responseUserId;
    NSNumber * m_responseUserCredits;
    UserProfile * m_responseUserProfile;
	UserProfiles * m_responseUserProfiles;
	UserProfiles * m_responseUserProfilesFollows;
	UserProfiles * m_responseUserProfilesFollowedBy;
    UserSongs * m_responseUserSongs;
    UserSongSession * m_responseUserSongSession;
    UserSongSessions * m_responseUserSongSessions;
    StoreFeatureCollection * m_responseStoreFeatureCollection;
    NSArray * m_responseProductIds;
    NSInteger m_responseFirmwareMajorVersion;
    NSInteger m_responseFirmwareMinorVersion;
    
}

@property (nonatomic, readonly) CloudRequest * m_cloudRequest;

@property (nonatomic, assign) CloudResponseStatus m_status;
@property (nonatomic, assign) OphoCloudResponseStatus m_opho_status;
@property (nonatomic, assign) NSInteger m_statusCode;
@property (nonatomic, strong) NSString * m_mimeType;
@property (nonatomic, strong) NSString * m_statusText;
@property (nonatomic, assign) BOOL m_loggedIn;

@property (nonatomic, strong) NSMutableData * m_receivedData;
@property (nonatomic, strong) NSString * m_receivedDataString;
@property (nonatomic, strong) XmlDom * m_responseXmlDom;

@property (nonatomic, assign) NSInteger m_responseFileId;
@property (nonatomic, assign) NSInteger m_responseUserId;
@property (nonatomic, strong) NSNumber * m_responseUserCredits;
@property (nonatomic, strong) UserProfile * m_responseUserProfile;
@property (nonatomic, strong) UserProfiles * m_responseUserProfiles;
@property (nonatomic, strong) UserProfiles * m_responseUserProfilesFollows;
@property (nonatomic, strong) UserProfiles * m_responseUserProfilesFollowedBy;
@property (nonatomic, strong) UserSongs * m_responseUserSongs;
@property (nonatomic, strong) UserSongSession * m_responseUserSongSession;
@property (nonatomic, strong) UserSongSessions * m_responseUserSongSessions;
@property (nonatomic, strong) StoreFeatureCollection * m_responseStoreFeatureCollection;
@property (nonatomic, strong) NSArray * m_responseProductIds;
@property (nonatomic, assign) NSInteger m_responseFirmwareMajorVersion;
@property (nonatomic, assign) NSInteger m_responseFirmwareMinorVersion;

- (id)initWithCloudRequest:(CloudRequest*)cloudRequest;

@end
