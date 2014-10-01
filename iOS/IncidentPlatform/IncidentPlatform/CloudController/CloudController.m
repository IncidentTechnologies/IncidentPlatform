//
//  CloudController.m
//  gTarAppCore
//
//  Created by Marty Greenia on 4/24/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "CloudController.h"

#import "CloudResponse.h"
#import "CloudRequest.h"
#import "UserProfile.h"
#import "UserProfiles.h"
#import "UserSong.h"
#import "UserSongs.h"
#import "UserSongSession.h"
#import "UserSongSessions.h"
//#import "StoreFeatureCollection.h"
//#import "FeaturedSong.h"

#import "XmlDictionary.h"
#import "XmlDom.h"

//class StatusCode
//{
//    const Ok = 0;
//    const Unknown = 1;
//    const ServerError = 2;
//    const InvalidUrl = 3;
//    const InvalidParameter = 4;
//    const Unauthorized = 5;
//    
//    static $StatusText = array( StatusCode::Ok => "Ok",
//                               StatusCode::Unknown => "Unknown", 
//                               StatusCode::ServerError => "ServerError",
//                               StatusCode::InvalidUrl => "InvalidUrl",
//                               StatusCode::InvalidParameter => "InvalidParameter",
//                               StatusCode::Unauthorized => "Unauthorized" );
//    
//}

#define SERVER_NAME_DEFAULT @"http://www.strumhub.com/v0.53"
#define SERVER_ROOT_DEFAULT @"http://www.strumhub.com/v0.53/app_iphone"
#define SERVER_NAME m_serverName
#define SERVER_ROOT m_serverRoot

#define GET_SERVER_STATUS @"Main/ServerStatus"
#define GET_ITUNES_STATUS @"Main/ItunesStatus"
#define CloudRequestTypeGetServerStatusUrl @"Main/ServerStatus"

// Files
#define CloudRequestTypeGetFileUrl @"UserFiles/GetFile"

// Users
#define CloudRequestTypeRegisterUrl @"Users/Register"
#define CloudRequestTypeLoginFacebookUrl @"Users/LoginWithFacebookAccessToken"
#define CloudRequestTypeLoginUrl @"Users/Login"
//#define CloudRequestTypeLoginCookieUrl @"Users/LoginWithCookie"
#define CloudRequestTypeLogoutUrl @"Users/Logout"
#define CloudRequestTypeGetUserProfileUrl @"Users/GetUserProfile"
#define CloudRequestTypeSearchUserProfileUrl @"Users/FindUserProfile"
#define CloudRequestTypeEditUserProfileUrl @"Users/EditUserProfile"
#define CloudRequestTypeSearchUserProfileFacebookUrl @"Users/FindFacebookFriendsUserProfile"
#define CloudRequestTypeGetUserCreditsUrl @"Users/GetUserCredits"
#define CloudRequestTypeVerifyItunesReceiptUrl @"Users/VerifyItunesPurchase"
#define CloudRequestTypeRedeemCreditCodeUrl @"Users/RedeemCreditCode"

// User Songs
#define CloudRequestTypeGetAllSongsListUrl @"UserSongs/GetAllSongsList"
#define CloudRequestTypeGetAllSongPidsUrl @"UserSongs/GetAllSongPids"
#define CloudRequestTypeGetUserSongListUrl @"UserSongs/GetUserSongsList"
#define CloudRequestTypeGetStoreSongListUrl @"UserSongs/GetStoreSongsList"
#define CloudRequestTypePurchaseSongUrl @"UserSongs/PurchaseSong"
//#define CloudRequestTypeGetStoreSongListUrl @"UserSongs/GetAllSongsList"
#define CloudRequestTypeGetStoreFeaturesSongListUrl @"UserSongs/GetFeaturedNewAndPopularSongsList"

// User Song Sessions
#define CloudRequestTypePutUserSongSessionUrl @"UserSongSessions/UploadSession"
#define CloudRequestTypeGetUserSongSessionsUrl @"UserSongSessions/GetUserSongSessions"
#define CloudRequestTypeGetUserGlobalSongSessionsUrl @"UserSongSessions/GetGlobalUserSongSessions"

// User Follows
#define CloudRequestTypeAddUserFollowsUrl @"UserFollows/AddFollows"
#define CloudRequestTypeRemoveUserFollowsUrl @"UserFollows/RemoveFollows"
#define CloudRequestTypeGetUserFollowsListUrl @"UserFollows/GetFollowsList"
#define CloudRequestTypeGetUserFollowedListUrl @"UserFollows/GetFollowedByList"
#define CloudRequestTypeGetUserFollowsSongSessionsUrl @"UserFollows/GetFollowsSessions"

// Telemtry
#define CloudRequestTypePutLogUrl @"Telemetry/UploadLog"

// gTar
#define CloudRequestTypeGetCurrentFirmwareVersionUrl @"GtarFirmwares/GetCurrentFirmwareVersion"
#define CloudRequestTypeRegisterGtarUrl @"GtarFirmwares/RegisterGtar"

// maybe append a clock() or tick() to this thing
#define POST_BOUNDARY @"------gTarPlayFormBoundary0123456789"

#define URL_REQUEST_TIMEOUT 30
//#define URL_REQUEST_TIMEOUT 0.0000001

// X number of time outs every Y seconds sends us offline
#define ERRORS_BEFORE_OFFLINE 6
#define ERROR_WINDOW 120

#define TEST_ONLINE_STATUS_PERIOD 60 // check every 1 minute

@implementation CloudController

@synthesize m_loggedIn;
@synthesize m_username;
@synthesize m_facebookAccessToken;

- (id)init {
    self = [super init];
    
	if ( self ) {
        // Assume we are online initially
        m_online = YES;
        m_loggedIn = NO;
        
		m_requestResponseDictionary = [[NSMutableDictionary alloc] init];
        m_connectionResponseDictionary = [[NSMutableDictionary alloc] init];
        m_requestQueue = [[NSMutableArray alloc] init];
        
        m_serverName = [[NSString alloc]  initWithFormat:SERVER_NAME_DEFAULT];
        m_serverRoot = [[NSString alloc]  initWithFormat:SERVER_ROOT_DEFAULT];
        
        // Set the cookie storage appropriately
//        [[NSHTTPCookieStorage sharedHTTPCookieStorage] setCookieAcceptPolicy:NSHTTPCookieAcceptPolicyAlways];
	}
	
	return self;
}

- (id)initWithServer:(NSString*)serverName {
    // Note this is 'self' not 'super'
    self = [self init];
    
    if ( self )  {
        m_serverName = serverName;
        m_serverRoot = [[NSString alloc] initWithFormat:@"%@/app_iphone", serverName];                
    }
    
    return self;
}

+ (CloudController *)sharedSingleton
{
    static CloudController *sharedSingleton;
    
    @synchronized(self)
    {
        if (!sharedSingleton)
        {
            NSString* server = kServerAddress;
            sharedSingleton = [[CloudController alloc] initWithServer:server];
        }
        
        return sharedSingleton;
    }
}


#pragma mark -
#pragma mark Misc

//- (NSHTTPCookie*)getCakePhpCookie
//{
//    
//    NSArray * cookies = [[NSHTTPCookieStorage sharedHTTPCookieStorage] cookiesForURL:[NSURL URLWithString:m_serverRoot]];
////    NSArray * cookies = [[NSHTTPCookieStorage sharedHTTPCookieStorage] cookiesForURL:[NSURL URLWithString:@"mcbookpro.local"]];
//    
//    for ( NSHTTPCookie * cookie in cookies )
//    {
//        if ( [cookie.name isEqualToString:@"CAKEPHP"] )
//        { 
//            return cookie;
//        }
//    }
//    
//    return nil;
//    
//}
//
//- (void)setCakePhpCookie:(NSHTTPCookie*)cookie
//{
//    
//    if ( cookie == nil )
//    {
//        // No cookies
//        [[NSHTTPCookieStorage sharedHTTPCookieStorage] setCookies:[NSArray array] forURL:[NSURL URLWithString:m_serverName] mainDocumentURL:nil];
//    }
//    else
//    {
//        NSArray  * cookieArray = [NSArray arrayWithObject:cookie];
//        
//        [[NSHTTPCookieStorage sharedHTTPCookieStorage] setCookies:cookieArray forURL:[NSURL URLWithString:m_serverName] mainDocumentURL:nil];
//    }
//    
//}

// This should always run on the main thread for the timer
- (void)handleError {
    
    m_errorsRecently++;
    
    if ( m_errorsRecently == 1 ) {
        [NSTimer scheduledTimerWithTimeInterval:ERROR_WINDOW target:self selector:@selector(errorWindowFinished) userInfo:nil repeats:NO];
    }
    else if ( m_errorsRecently >= ERRORS_BEFORE_OFFLINE )
    {
        // we are offline
        m_online = NO;
        
        [NSTimer scheduledTimerWithTimeInterval:TEST_ONLINE_STATUS_PERIOD target:self selector:@selector(queryOnlineStatus) userInfo:nil repeats:NO];
    }
    
}

- (void)errorWindowFinished
{
    // reset the error count
    m_errorsRecently = 0;
}

- (void)queryOnlineStatus
{
    [self requestServerStatusCallbackObj:self andCallbackSel:@selector(receiveOnlineStatus:)];
}

- (void)receiveOnlineStatus:(CloudResponse*)cloudResponse
{
    
    if ( cloudResponse.m_status == CloudResponseStatusSuccess )
    {
        // We are back online
        m_online = YES;
        m_errorsRecently = 0;
    }
    else
    {
        // Try again later
        [NSTimer scheduledTimerWithTimeInterval:TEST_ONLINE_STATUS_PERIOD target:self selector:@selector(queryOnlineStatus) userInfo:nil repeats:NO];
    }
}

#pragma mark -
#pragma mark Syncronous convenience

// These functions haven't been used in a long time and are probably dead
- (BOOL)requestServerStatus {
    NSString * urlString = [NSString stringWithFormat:@"%@/%@", SERVER_ROOT, GET_SERVER_STATUS];
    XmlDom * dom = [[XmlDom alloc] initWithXmlData:[NSData dataWithContentsOfURL:[NSURL URLWithString:urlString]]];
    NSString * response = [dom getTextFromChildWithName:@"StatusText"];
    
    BOOL status = NO;
    
    // The request succeeded, the server is there
    if ( [response isEqualToString:@"Ok"] )
        status = YES;
    else
        status = NO;
    
    return status;
}


- (BOOL)requestItunesServerStatus {
    NSString * urlString = [NSString stringWithFormat:@"%@/%@", SERVER_ROOT, GET_ITUNES_STATUS];
    XmlDom * dom = [[XmlDom alloc] initWithXmlData:[NSData dataWithContentsOfURL:[NSURL URLWithString:urlString]]];
    NSString * response = [dom getTextFromChildWithName:@"StatusText"];
    
    BOOL status = NO;
    
    // The request succeeded, the server is there
    if ( [response isEqualToString:@"Ok"] )
        status = YES;
    else
        status = NO;
    
    return status;
}

- (NSNumber*)requestUserCredits {
    // Create sync request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserCredits];
    CloudResponse * cloudResponse = [self cloudSendRequest:cloudRequest];
    NSNumber * credits = nil;
    
    if ( cloudResponse.m_status == CloudResponseStatusSuccess )
        credits = cloudResponse.m_responseUserCredits;
    
    return credits;
}


#pragma mark - Server requests

- (CloudRequest*)requestServerStatusCallbackObj:(id)obj andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetServerStatus andCallbackObject:obj andCallbackSelector:sel];
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

#pragma mark File

- (CloudRequest*)requestFile:(NSInteger)fileId andCallbackObj:(id)obj andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetFile andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_fileId = fileId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudResponse*)requestFileSync:(NSInteger)fileId
{

    // Create sync request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetFile];
    
    cloudRequest.m_fileId = fileId;
    
    CloudResponse * cloudResponse = [self cloudSendRequest:cloudRequest];
        
    return cloudResponse;
        
}

#pragma mark User

- (CloudRequest*)requestRegisterUsername:(NSString*)username
                             andPassword:(NSString*)password
                                andEmail:(NSString*)email
                          andCallbackObj:(id)obj
                          andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeRegister andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_username = username;
    cloudRequest.m_password = password;
    cloudRequest.m_email = email;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestLoginUsername:(NSString*)username
                          andPassword:(NSString*)password
                       andCallbackObj:(id)obj
                       andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeLogin andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_username = username;
    cloudRequest.m_password = password;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
	
}

- (CloudRequest*)requestFacebookLoginWithToken:(NSString*)accessToken
                                andCallbackObj:(id)obj
                                andCallbackSel:(SEL)sel
{
	
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeLoginFacebook andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_facebookAccessToken = accessToken;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

//- (CloudRequest*)requestLoginWithCookie:(NSHTTPCookie*)cookie 
//                         andCallbackObj:(id)obj
//                         andCallbackSel:(SEL)sel
//{
//    
//    // Create async request
//    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeLoginCookie andCallbackObject:obj andCallbackSelector:sel];
//    
//    cloudRequest.m_cookie = cookie;
//    
//    [self cloudSendRequest:cloudRequest];
//    
//    return [cloudRequest autorelease];
//    
//}

- (CloudRequest*)requestLogoutCallbackObj:(id)obj
                           andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeLogout andCallbackObject:obj andCallbackSelector:sel];
    
    // Also Delete local cookies for good measure
//    NSArray * cookies = [[NSHTTPCookieStorage sharedHTTPCookieStorage] cookiesForURL:[NSURL URLWithString:SERVER_NAME]];
//    
//	for ( unsigned int index = 0; index < [cookies count]; index++ )
//	{
//		
//		NSHTTPCookie * cookie = [cookies objectAtIndex:index];
//		
//		[[NSHTTPCookieStorage sharedHTTPCookieStorage] deleteCookie:cookie];
//		
//	}
    
    m_loggedIn = NO;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
	
}

- (CloudRequest*)requestUserProfile:(NSInteger)userId
                     andCallbackObj:(id)obj
                     andCallbackSel:(SEL)sel
{
	
    // Note that you must already be logged in in order for this to succeed
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserProfile andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userId = userId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestUserProfileEdit:(NSString*)name andEmail:(NSString*)email andImage:(UIImage*)profPic andCallbackObj:(id)obj andCallbackSel:(SEL)sel;
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeEditUserProfile andCallbackObject:obj andCallbackSelector:sel];
    
    // Add some other stuff here?
    cloudRequest.m_email = email;
    cloudRequest.m_profileImage = profPic;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;

}

- (CloudRequest*)requestUserProfileSearch:(NSString*)search
                           andCallbackObj:(id)obj
                           andCallbackSel:(SEL)sel
{
	
	// Note that you must already be logged in in order for this to succeed
	
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeSearchUserProfile andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_searchString = search;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestUserProfileFacebookSearch:(NSString*)accessToken
                                   andCallbackObj:(id)obj
                                   andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeSearchUserProfileFacebook andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_facebookAccessToken = accessToken;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestUserCreditsCallbackObj:(id)obj
                                andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserCredits andCallbackObject:obj andCallbackSelector:sel];
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestPurchaseSong:(UserSong*)userSong
                      andCallbackObj:(id)obj
                      andCallbackSel:(SEL)sel
{
    

    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypePurchaseSong andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userSong = userSong;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}


- (CloudRequest*)requestVerifyReceipt:(NSData*)receipt
                       andCallbackObj:(id)obj
                       andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest *cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeVerifyItunesReceipt andCallbackObject:obj andCallbackSelector:sel];
    cloudRequest.m_itunesReceipt = receipt;
    [self cloudSendRequest:cloudRequest];
    return cloudRequest;
    
}

#pragma mark UserSongs

- (CloudRequest*)requestSongListCallbackObj:(id)obj
                             andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserSongList andCallbackObject:obj andCallbackSelector:sel];
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestFeaturedSongListCallbackObj:(id)obj
                                     andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetStoreFeaturesSongList andCallbackObject:obj andCallbackSelector:sel];
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestSongStoreListCallbackObj:(id)obj
                                  andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetStoreSongList andCallbackObject:obj andCallbackSelector:sel];
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestRedeemCreditCode:(NSString*)creditCode
                          andCallbackObj:(id)obj
                          andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeRedeemCreditCode andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_creditCode = creditCode;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

#pragma mark UserSongSessions

- (CloudRequest*)requestUploadUserSongSession:(UserSongSession*)songSession
                               andCallbackObj:(id)obj
                               andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypePutUserSongSession andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userSongSession = songSession;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

#pragma mark UserFollows

- (CloudRequest*)requestUserSessions:(NSInteger)userId
                             andPage:(NSInteger)page
                      andCallbackObj:(id)obj
                      andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserSongSessions andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userId = userId;
    cloudRequest.m_page = page;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestAddFollowUser:(NSInteger)userId
                       andCallbackObj:(id)obj
                       andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeAddUserFollows andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userId = userId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestRemoveFollowUser:(NSInteger)userId
                          andCallbackObj:(id)obj
                          andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeRemoveUserFollows andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userId = userId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestFollowsList:(NSInteger)userId
                     andCallbackObj:(id)obj
                     andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserFollowsList andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userId = userId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestFollowedByList:(NSInteger)userId
                        andCallbackObj:(id)obj
                        andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserFollowedList andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userId = userId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestFollowsSessions:(NSInteger)userId
                                andPage:(NSInteger)page
                         andCallbackObj:(id)obj
                         andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserFollowsSongSessions andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_userId = userId;
    
    cloudRequest.m_page = page;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestGlobalSessionsPage:(NSInteger)page
                            andCallbackObj:(id)obj
                            andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserGlobalSongSessions andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_page = page;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
    
}

- (CloudRequest*)requestLogUpload:(NSString*)log
                       andVersion:(NSString*)version
                        andDevice:(NSString*)device
                           andApp:(NSString*)app
                   andCallbackObj:(id)obj
                   andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypePutLog andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_logEntries = log;
    cloudRequest.m_versionString = version;
    cloudRequest.m_appString = app;
    cloudRequest.m_deviceString = device;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;

}

- (CloudRequest*)requestCurrentFirmwareVersionCallbackObj:(id)obj andCallbackSel:(SEL)sel {
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetCurrentFirmwareVersion andCallbackObject:obj andCallbackSelector:sel];
    
    [self cloudSendRequest:cloudRequest];
    return cloudRequest;
}

- (CloudRequest*)requestRegisterGtarSerialUpper:(NSString*)serial_upper SerialLower:(NSString*)serial_lower andCallbackObj:(id)obj andCallbackSel:(SEL)sel {
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeRegisterGtar andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_serial_lower = [[NSString alloc] initWithString:serial_lower];
    cloudRequest.m_serial_upper = [[NSString alloc] initWithString:serial_upper];
    
    [self cloudSendRequest:cloudRequest];
    return cloudRequest;
}

#pragma mark -
#pragma mark Connection registration

- (void)registerConnection:(NSURLConnection*)connection toResponse:(CloudResponse*)cloudResponse
{
    
	// need to make the object a NSValue in order to use it as a key 
	NSValue * key = [NSValue valueWithNonretainedObject:connection];
	
	[m_connectionResponseDictionary setObject:cloudResponse forKey:key];
	
}

- (CloudResponse*)getReponseForConnection:(NSURLConnection*)connection
{

    // need to make the object a NSValue in order to use it as a key 
    NSValue * key = [NSValue valueWithNonretainedObject:connection];
    
	CloudResponse * cloudResponse = [m_connectionResponseDictionary objectForKey:key];
	
    return cloudResponse;

}

- (CloudResponse*)deregisterConnection:(NSURLConnection*)connection
{
    
	// need to make the object a NSValue in order to use it as a key 
    NSValue * key = [NSValue valueWithNonretainedObject:connection];
    
	CloudResponse * cloudResponse = [m_connectionResponseDictionary objectForKey:key];
	
	[m_connectionResponseDictionary removeObjectForKey:key];
    
    
    return cloudResponse;
    
}

#pragma mark -
#pragma mark Cloud functions

- (CloudResponse*)cloudSendRequest:(CloudRequest*)cloudRequest
{
    
    // If queue send requests
    if ( cloudRequest.m_isSynchronous == YES )
    {
        // Sync requests just go 
        return [self cloudProcessRequest:cloudRequest];
    }
    else if ( cloudRequest.m_type == CloudRequestTypeGetFile )
    {
        // bypass the request queue for file requests.
        [self cloudProcessRequest:cloudRequest];
        
        return nil;
    }
    else
    {
        // Async requests go in the queue
        @synchronized(m_requestQueue)
        {
            
            [m_requestQueue addObject:cloudRequest];
            
            if ( [m_requestQueue count] == 1 )
            {
                // If this is the only thing in the queue, send it
                [self cloudProcessRequest:cloudRequest];
            }
            
        }
        
        return nil;
        
    }
    
}

- (CloudResponse*)cloudProcessRequest:(CloudRequest*)cloudRequest
{
    // Create a response object
    CloudResponse * cloudResponse = [[CloudResponse alloc] initWithCloudRequest:cloudRequest];
    
    // If we are offline, abort now... unless we are testing server status,
    // in which case we should proceed, naturally.
    if ( m_online == NO && cloudRequest.m_type != CloudRequestTypeGetServerStatus )
    {
        if ( cloudRequest.m_isSynchronous == YES )
        {
            cloudRequest.m_status = CloudRequestStatusOffline;
            cloudResponse.m_status = CloudResponseStatusOffline;
            cloudResponse.m_statusText = @"Offline, try again in a few minutes";
            
            return cloudResponse;
        }
        else
        {
            cloudRequest.m_status = CloudRequestStatusOffline;
            cloudResponse.m_status = CloudResponseStatusOffline;
            cloudResponse.m_statusText = @"Offline, try again in a few minutes";
            
            // Return the request to sender
            [self cloudReturnResponse:cloudResponse];
            
            return nil;
        }
    }
    
    // Parse out the request parameters
    NSURLRequest * urlRequest = [self createPostForRequest:cloudRequest];
    
    // Is this a async or sync request?
    if ( cloudRequest.m_isSynchronous == YES )
    {
        
        NSURLResponse * urlResponse = nil;
        
        NSError * error = nil;
        
        NSData * responseData = [NSURLConnection sendSynchronousRequest:urlRequest
                                                      returningResponse:&urlResponse
                                                                  error:&error];
        
        // See if our connection failed
        if ( error != nil )
        {
            
            NSLog( @"Sync connection error: %@", [error localizedDescription] );
            
            cloudRequest.m_status = CloudRequestStatusConnectionError;
            
            cloudResponse.m_status = CloudResponseStatusConnectionError;
            
            cloudResponse.m_statusText = [error localizedDescription];
            
        }
        else
        {
            
            cloudResponse.m_receivedData = [responseData mutableCopy];
            
            cloudResponse.m_receivedDataString = [NSString stringWithCString:(char*)[responseData bytes] encoding:NSASCIIStringEncoding];
            
            // Extract http response info
            NSHTTPURLResponse * httpResponse = (NSHTTPURLResponse*)urlResponse;
            
            cloudResponse.m_mimeType = [httpResponse MIMEType];
            
            cloudResponse.m_statusCode = [httpResponse statusCode];
            
            cloudResponse.m_cloudRequest.m_status = CloudRequestStatusCompleted;
            
            // Parse the results
            [self parseResultsForResponse:cloudResponse];
            
        }
        
        return cloudResponse;
        
    }
    else
    {
        
        // Create a connection
        NSURLConnection * connection = [[NSURLConnection alloc] initWithRequest:urlRequest delegate:self startImmediately:NO];
        
        if ( connection == nil )
        {
            
            NSLog( @"Async connection error" );
            
            cloudRequest.m_status = CloudRequestStatusConnectionError;
            cloudResponse.m_status = CloudResponseStatusConnectionError;
            cloudResponse.m_statusText = @"Connection error";
            
            // Return the request to sender
            [self cloudReturnResponse:cloudResponse];
            
        }
        else
        {
            
            // The request has been sent
            cloudRequest.m_status = CloudRequestStatusSent;
            
            // Register this connection so we can access it later.
            [self registerConnection:connection toResponse:cloudResponse];
            
        }
        
        // Schedule it on the main thread, otherwise it won't work.
        [connection scheduleInRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
        
        [connection start];
        
        
        // The response is returned asynchronously
        return nil;
        
    }
    
}

- (void)cloudReceiveResponse:(CloudResponse*)cloudResponse
{
    // This comes in from the 'NSUrl thread' so it doesn't have a pool
    @autoreleasepool {

    // Parse the results
        [self parseResultsForResponse:cloudResponse];
        
        // Update our logged in status, based on what just came in.
        cloudResponse.m_loggedIn = self.m_loggedIn;
        
        // Return to sender
        [self cloudReturnResponse:cloudResponse];
    
    }
    
}

- (void)cloudReturnResponse:(CloudResponse*)cloudResponse
{
    
    CloudRequest * cloudRequest = cloudResponse.m_cloudRequest;
    
    id callbackObject = cloudRequest.m_callbackObject;
    SEL callbackSelector = cloudRequest.m_callbackSelector;
    
    if ( [callbackObject respondsToSelector:callbackSelector] == YES ) {
        // Send this back to the original caller
//        [callbackObject performSelector:callbackSelector withObject:cloudResponse];
        [callbackObject performSelectorOnMainThread:callbackSelector withObject:cloudResponse waitUntilDone:NO];
    }
    
    if ( cloudRequest.m_type != CloudRequestTypeGetFile )
    {
        // GetFile bypasses the request queue
        
        // Now that this request is done, we can issue another one
        @synchronized(m_requestQueue)
        {
            // Remove the object we just finished.
            [m_requestQueue removeObject:cloudRequest];
            
            // If there is anything else, send one off.
            if ( [m_requestQueue count] > 0 )
            {
                for ( CloudRequest * cloudRequest in m_requestQueue )
                {
                    // Pull off the first unsent object from the queue
                    if ( cloudRequest.m_status == CloudRequestStatusPending )
                    {
                        [self cloudProcessRequest:cloudRequest];
                        break;
                    }
                }
            }
        }
    }
    
}

#pragma mark -
#pragma mark Cloud helpers

- (NSURLRequest*)createPostForRequest:(CloudRequest*)cloudRequest {
    
    NSString * urlString = nil;
    NSArray * paramsArray = nil;
    NSArray * filesArray = nil;
    
    // Get the arguments ready based on the request type
    [self marshalArgumentsForRequest:cloudRequest withUrl:&urlString andParameters:&paramsArray andFiles:&filesArray];
    
    NSString * rootedUrlString = [NSString stringWithFormat:@"%@/%@", SERVER_ROOT, urlString];
#if TARGET_IPHONE_SIMULATOR
    NSLog(@"%@", rootedUrlString);
#endif
    // Create a POST request object
    NSMutableURLRequest * request = [[NSMutableURLRequest alloc] init];
	
    [request setURL:[NSURL URLWithString:rootedUrlString]];
    
	[request setHTTPMethod:@"POST"];
	
    [request setTimeoutInterval:URL_REQUEST_TIMEOUT];
    
    // Set the ContentType to multipart/form-data. This is what CakePHP expects
    NSString * boundary = POST_BOUNDARY;
    
	NSString * contentType = [NSString stringWithFormat:@"multipart/form-data; boundary=%@", boundary];
	
	[request addValue:contentType forHTTPHeaderField: @"Content-Type"];
	
	// Create the post body
    NSMutableData * postBodyData = [NSMutableData data];
    
    [postBodyData appendData:[[NSString stringWithFormat:@"\r\n--%@\r\n", boundary] dataUsingEncoding:NSUTF8StringEncoding]];
    
	// Add all the fields
	// Method = post
    [postBodyData appendData:[[NSString stringWithFormat:@"Content-Disposition: form-data; name=\"_method\"\r\n\r\nPOST"] dataUsingEncoding:NSUTF8StringEncoding]];
    [postBodyData appendData:[[NSString stringWithFormat:@"\r\n--%@\r\n", boundary] dataUsingEncoding:NSUTF8StringEncoding]];
    
    // Add all the parameters to the post body
    for ( NSDictionary * paramDict in paramsArray )
    {
        
        NSString * name = [paramDict objectForKey:@"Name"];
        NSString * value = [paramDict objectForKey:@"Value"];
        
        // Some params might be optional
        if ( name != nil && value != nil )
        {
            [postBodyData appendData:[[NSString stringWithFormat:@"Content-Disposition: form-data; name=\"%@\"\r\n\r\n", name] dataUsingEncoding:NSUTF8StringEncoding]];
            [postBodyData appendData:[[NSString stringWithFormat:@"%@", value] dataUsingEncoding:NSUTF8StringEncoding]];
            [postBodyData appendData:[[NSString stringWithFormat:@"\r\n--%@\r\n", boundary] dataUsingEncoding:NSUTF8StringEncoding]];
        }
        
    }
    
    // Add all the files to the post body
    for ( NSDictionary * fileDict in filesArray ) {
        NSString * name = [fileDict objectForKey:@"Name"];
        NSString * filename = [fileDict objectForKey:@"Filename"];
        NSString * contentType = nil;
        
        id fileData = [fileDict objectForKey:@"Data"];
        
        NSData * data = nil;
        
        if ( [fileData isKindOfClass:[UIImage class]] == YES )
        {
            data = UIImageJPEGRepresentation((UIImage*)fileData, 0.1 );
            contentType = @"image/jpg";
            
//          data = UIImagePNGRepresentation( (UIImage*)fileData );
//          contentType = @"image/png";
        }
        
        if ( [fileData isKindOfClass:[NSString class]] == YES )
        {
            data = [(NSString*)fileData dataUsingEncoding:NSUTF8StringEncoding];
            contentType = @"application/octet-stream";
        }
            
        // Some params might be optional
        if ( name != nil && filename != nil && data != nil )
        {
            [postBodyData appendData:[[NSString stringWithFormat:@"Content-Disposition: form-data; name=\"%@\"; filename=\"%@\"\r\n", name, filename] dataUsingEncoding:NSUTF8StringEncoding]];
            [postBodyData appendData:[[NSString stringWithFormat:@"Content-Type: %@\r\n\r\n", contentType] dataUsingEncoding:NSUTF8StringEncoding]];
            [postBodyData appendData:data];
            [postBodyData appendData:[[NSString stringWithFormat:@"\r\n--%@\r\n", boundary] dataUsingEncoding:NSUTF8StringEncoding]];

        }
        
    }
    
    // Convert the POST body to data bytes
//    NSData * postBodyData = [postBody dataUsingEncoding:NSUTF8StringEncoding];
#if TARGET_IPHONE_SIMULATOR
    NSString * postString = [[NSString alloc] initWithData:postBodyData encoding:NSUTF8StringEncoding];
    NSLog(postString);
#endif
    // Stick the post body (now as encoded bytes) into the request
	[request setHTTPBody:postBodyData];
	
	// Update content length in the header. CakePHP will barf without this field, 
    // and the iphone doesn't appear to be adding it automatically.
	[request addValue:[NSString stringWithFormat:@"%u", [postBodyData length]] forHTTPHeaderField:@"Content-Length"];
    
    return request;
    
}

- (void)marshalArgumentsForRequest:(CloudRequest*)cloudRequest withUrl:(NSString**)urlString andParameters:(NSArray**)paramsArray andFiles:(NSArray**)filesArray
{
    
    NSString * url = nil;
    NSArray * params = nil;
    NSArray * files = nil;
    
    switch ( cloudRequest.m_type )
    {
            
        case CloudRequestTypeGetServerStatus:
        {
            url = CloudRequestTypeGetServerStatusUrl;
            
        } break;
            
        case CloudRequestTypeGetFile:
        {
            
            url = CloudRequestTypeGetFileUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserFiles][id]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_fileId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeRegister:
        {
            
            url = CloudRequestTypeRegisterUrl;
            
            NSDictionary * param1 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[User][username]", @"Name",
                                     cloudRequest.m_username, @"Value", nil];
            
            NSDictionary * param2 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[User][password]", @"Name",
                                     cloudRequest.m_password, @"Value", nil];
            
            NSDictionary * param3 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[User][password_verification]", @"Name",
                                     cloudRequest.m_password, @"Value", nil];
            
            NSDictionary * param4 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[User][email]", @"Name",
                                     cloudRequest.m_email, @"Value", nil];
            
            params = [NSArray arrayWithObjects:param1, param2, param3, param4, nil];
            
        } break;
            
        case CloudRequestTypeLoginFacebook:
        {
            
            url = CloudRequestTypeLoginFacebookUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[Users][access_token]", @"Name",
                                    cloudRequest.m_facebookAccessToken, @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeLogin:
        {
            
            url = CloudRequestTypeLoginUrl;
            
            NSDictionary * param1 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[User][username]", @"Name",
                                     cloudRequest.m_username, @"Value", nil];
            
            NSDictionary * param2 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[User][password]", @"Name",
                                     cloudRequest.m_password, @"Value", nil];
            
            params = [NSArray arrayWithObjects:param1, param2, nil];
            
        } break;
            
//        case CloudRequestTypeLoginCookie:
//        {
//            
//            url = CloudRequestTypeLoginCookieUrl;
//            
//            // Update the cookies
//            [self setCakePhpCookie:cloudRequest.m_cookie];
//            
//        } break;
            
        case CloudRequestTypeLogout:
        {
            
            url = CloudRequestTypeLogoutUrl;
            
        } break;
            
        case CloudRequestTypeGetUserProfile: {
            
            url = CloudRequestTypeGetUserProfileUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[User][id]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_userId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeEditUserProfile:
        {
            
            url = CloudRequestTypeEditUserProfileUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserProfile][email]", @"Name",
                                    cloudRequest.m_email, @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
            NSDictionary * fileDict = [NSDictionary dictionaryWithObjectsAndKeys:
                                       @"data[UserProfile][fileProfilePic]", @"Name",
                                       @"profilePic.png", @"Filename",
                                       cloudRequest.m_profileImage, @"Data", nil];
            
            files = [NSArray arrayWithObject:fileDict];
            
        } break;
            
        case CloudRequestTypeSearchUserProfile:
        {
            
            url = CloudRequestTypeSearchUserProfileUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserProfile][search]", @"Name",
                                    cloudRequest.m_searchString, @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeSearchUserProfileFacebook:
        {
            
            url = CloudRequestTypeSearchUserProfileFacebookUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[User][access_token]", @"Name",
                                    cloudRequest.m_facebookAccessToken, @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeGetUserCredits:
        {
            
            url = CloudRequestTypeGetUserCreditsUrl;
            
        } break;
            
        case CloudRequestTypePurchaseSong:
        {
            
            url = CloudRequestTypePurchaseSongUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserSongs][songIdToPurchase]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_userSong.m_songId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeVerifyItunesReceipt:
        {
            url = CloudRequestTypeVerifyItunesReceiptUrl;
            
            // Need to convert the NSData to base 64 string here
            // TODO: move to helper func area?
            static char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
            NSMutableData *data = [NSMutableData dataWithLength:((cloudRequest.m_itunesReceipt.length + 2) / 3) * 4];
            uint8_t *output = (uint8_t *)data.mutableBytes;
            for (NSInteger i = 0; i < cloudRequest.m_itunesReceipt.length; i += 3) {
                NSInteger value = 0;

                for (NSInteger j = i; j < (i + 3); j++) {
                    value <<= 8;
                    if (j < cloudRequest.m_itunesReceipt.length)
                        value |= (0xFF & ((uint8_t *)(cloudRequest.m_itunesReceipt.bytes))[j]);
                }
                
                NSInteger index = (i / 3) * 4;
                output[index + 0] =                    table[(value >> 18) & 0x3F];
                output[index + 1] =                    table[(value >> 12) & 0x3F];
                output[index + 2] = (i + 1) < cloudRequest.m_itunesReceipt.length ? table[(value >> 6)  & 0x3F] : '=';
                output[index + 3] = (i + 2) < cloudRequest.m_itunesReceipt.length ? table[(value >> 0)  & 0x3F] : '=';
            }
            
            NSString *strBase64EncodedData = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
            
            /*
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[Users][receipt]", @"Name",
                                    cloudRequest.m_itunesReceipt, @"Value", nil];
            */
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[Users][receipt]", @"Name",
                                    strBase64EncodedData, @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeGetAllSongPids: url = CloudRequestTypeGetAllSongPidsUrl; break;
        case CloudRequestTypeGetAllSongsList: url = CloudRequestTypeGetAllSongsListUrl; break;
        case CloudRequestTypeGetUserSongList: url = CloudRequestTypeGetUserSongListUrl; break;
        case CloudRequestTypeGetStoreSongList:  url = CloudRequestTypeGetStoreSongListUrl; break;
        case CloudRequestTypeGetStoreFeaturesSongList:  url = CloudRequestTypeGetStoreFeaturesSongListUrl; break;
            
        case CloudRequestTypePutUserSongSession:
        {
            
            url = CloudRequestTypePutUserSongSessionUrl;
            
            NSDictionary * param1 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserSongSession][user_song_id]", @"Name",
                                     [NSNumber numberWithInteger:cloudRequest.m_userSongSession.m_userSong.m_songId], @"Value", nil];
            
            NSDictionary * param2 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserSongSession][score]", @"Name",
                                     [NSNumber numberWithInteger:cloudRequest.m_userSongSession.m_score], @"Value", nil];
            
            NSDictionary * param3 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserSongSession][maxscore]", @"Name", 
                                     [NSNumber numberWithInteger:cloudRequest.m_userSongSession.m_scoreMax], @"Value", nil];
            
            NSDictionary * param4 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserSongSession][topstreak]", @"Name",
                                     [NSNumber numberWithInteger:cloudRequest.m_userSongSession.m_combo], @"Value", nil];
            
            NSDictionary * param5 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserSongSession][notes]", @"Name",
                                     cloudRequest.m_userSongSession.m_notes, @"Value", nil];
            
            params = [NSArray arrayWithObjects:param1, param2, param3, param4, param5, nil];
            
            NSDictionary * fileDict = [NSDictionary dictionaryWithObjectsAndKeys:
                                       @"data[UserSongSession][fileSession]", @"Name",
                                       @"session.xmp", @"Filename",
                                       cloudRequest.m_userSongSession.m_xmpBlob, @"Data", nil];
            
            files = [NSArray arrayWithObject:fileDict];
            
        } break;
            
        case CloudRequestTypeGetUserSongSessions:
        {
            
            url = CloudRequestTypeGetUserSongSessionsUrl;
            
            NSDictionary * param1 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserSongSessions][user_id]", @"Name",
                                     [NSNumber numberWithInteger:cloudRequest.m_userId], @"Value", nil];
            
            NSDictionary * param2 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserSongSessions][page]", @"Name",
                                     [NSNumber numberWithInteger:cloudRequest.m_page], @"Value", nil];
            
            params = [NSArray arrayWithObjects:param1, param2, nil];
            
            
        } break;
            
        case CloudRequestTypeAddUserFollows:
        {
            
            url = CloudRequestTypeAddUserFollowsUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserFollow][user_follow_id]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_userId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeRemoveUserFollows:
        {
            
            url = CloudRequestTypeRemoveUserFollowsUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserFollow][user_follow_id]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_userId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeGetUserFollowsList:
        {
            
            url = CloudRequestTypeGetUserFollowsListUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserFollow][user_id]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_userId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeGetUserFollowedList:
        {
            
            url = CloudRequestTypeGetUserFollowedListUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserFollow][user_follow_id]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_userId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        case CloudRequestTypeGetUserFollowsSongSessions:
        {
            
            url = CloudRequestTypeGetUserFollowsSongSessionsUrl;
            
            NSDictionary * param1 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserFollow][user_id]", @"Name",
                                     [NSNumber numberWithInteger:cloudRequest.m_userId], @"Value", nil];
            
            NSDictionary * param2 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[UserFollow][page]", @"Name",
                                     [NSNumber numberWithInteger:cloudRequest.m_page], @"Value", nil];
            
            params = [NSArray arrayWithObjects:param1, param2, nil];
            
        } break;
            
        case CloudRequestTypeGetUserGlobalSongSessions:
        {
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserSongSessions][page]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_page], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
            url = CloudRequestTypeGetUserGlobalSongSessionsUrl;
        } break;
            
        case CloudRequestTypeRedeemCreditCode:
        {
            url = CloudRequestTypeRedeemCreditCodeUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserCodeRedemptions][code]", @"Name",
                                    cloudRequest.m_creditCode, @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
        } break;
            
        case CloudRequestTypePutLog:
        {
            
            url = CloudRequestTypePutLogUrl;
            
            NSDictionary * param1 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[Telemetry][app_id]", @"Name",
                                     cloudRequest.m_appString, @"Value", nil];
            
            NSDictionary * param2 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[Telemetry][version_id]", @"Name",
                                     cloudRequest.m_versionString, @"Value", nil];
            
            NSDictionary * param3 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"data[Telemetry][device_id]", @"Name",
                                     cloudRequest.m_deviceString, @"Value", nil];
            
            params = [NSArray arrayWithObjects:param1, param2, param3, nil];
            
            NSDictionary * fileDict = [NSDictionary dictionaryWithObjectsAndKeys:
                                       @"data[Telemetry][fileLog]", @"Name",
                                       @"telemetry.log", @"Filename",
                                       cloudRequest.m_logEntries, @"Data", nil];
            
            files = [NSArray arrayWithObject:fileDict];
        } break;
            
        case CloudRequestTypeGetCurrentFirmwareVersion: {
            url = CloudRequestTypeGetCurrentFirmwareVersionUrl;
        } break;
            
        case CloudRequestTypeRegisterGtar: {
            //url = CloudRequestTypeRegisterGtarUrl;
            /*NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[User][id]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_userId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];*/
            
            url = [[NSString alloc] initWithFormat:@"%@/?serial_lower=%@", CloudRequestTypeRegisterGtarUrl, cloudRequest.m_serial_lower];
        } break;
            
        default: break;
    }
    
    (*urlString) = url;
    (*paramsArray) = params;
    (*filesArray) = files;
    
}

- (void)parseResultsForResponse:(CloudResponse*)cloudResponse
{
    
    // If the request didn't finish or failed, status = failure
    if ( cloudResponse.m_cloudRequest.m_status != CloudRequestStatusCompleted )
    {
        // Since the request didn't complete fully, we can't really trust any data we have
        cloudResponse.m_status = CloudResponseStatusConnectionError;
        cloudResponse.m_statusText = @"Connection Error";
        cloudResponse.m_receivedData = nil;
        
        return;
    }
    
    // Do any pre-processing. Anything that needs special 
    // attention can be done here, and optionally return.
    switch ( cloudResponse.m_cloudRequest.m_type )
    {   
        case CloudRequestTypeGetFile:
        {
            if ( cloudResponse.m_statusCode == 200 )
            {
                // The file is already located in m_receivedData.
                cloudResponse.m_status = CloudResponseStatusSuccess;
                cloudResponse.m_statusText = @"Success";
                cloudResponse.m_responseFileId = cloudResponse.m_cloudRequest.m_fileId;
            }
            else
            {
                // e.g. 404, we can't trust the data
                cloudResponse.m_status = CloudResponseStatusFailure;
                cloudResponse.m_statusText = [NSString stringWithFormat:@"HTTP Status: %u", cloudResponse.m_statusCode];
                cloudResponse.m_receivedData = nil;
            }
            
            // Done with this response.
            return;
            
        } break;

        default:
        {
            
            if ( cloudResponse.m_statusCode == 200 )
            {
                // Do more detailed pre-processing
                [self preprocessResultsForResponse:cloudResponse];  
            }
            else
            {
                // e.g. 404
                cloudResponse.m_status = CloudResponseStatusFailure;
                cloudResponse.m_statusText = [NSString stringWithFormat:@"HTTP Status: %u", cloudResponse.m_statusCode];
                cloudResponse.m_receivedData = nil;
                
                // Done with this response
                return;
            }
        } break;
            
    }
    
    switch ( cloudResponse.m_cloudRequest.m_type )
    {
            
        case CloudRequestTypeGetServerStatus:
        {
            // If we got this far, the server is up
        } break;
            
        case CloudRequestTypeGetFile:
        {
            
            // Handled above, no-op
            
            
        } break;
            
        case CloudRequestTypeRegister:
        {
            
            
            m_username = [cloudResponse.m_responseXmlDom getTextFromChildWithName:@"Username"];
            
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            
            NSString * result = [dom getTextFromChildWithName:@"StatusText"];
            
            if ( [result isEqualToString:@"Ok"] == YES )
            {
                m_loggedIn = YES;
            }
            else
            {
                m_loggedIn = NO;
            }
            
            XmlDom * profileDom = [dom getChildWithName:@"UserProfile"];
            
            UserProfile * userProfile = [[UserProfile alloc] initWithXmlDom:profileDom];
            
            cloudResponse.m_responseUserProfile = userProfile;

            
        } break;
            
        case CloudRequestTypeLoginFacebook:
        {
            
            
            m_username = [cloudResponse.m_responseXmlDom getTextFromChildWithName:@"Username"];
            
            m_facebookAccessToken = cloudResponse.m_cloudRequest.m_facebookAccessToken;
            
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            
            NSString * result = [dom getTextFromChildWithName:@"StatusText"];
            
            if ( [result isEqualToString:@"Ok"] == YES )
            {
                m_loggedIn = YES;
            }
            else
            {
                m_loggedIn = NO;
            }
            
            XmlDom * profileDom = [dom getChildWithName:@"UserProfile"];
            
            UserProfile * userProfile = [[UserProfile alloc] initWithXmlDom:profileDom];
            
            cloudResponse.m_responseUserProfile = userProfile;
            
        } break;
            
        case CloudRequestTypeLogin:
        {
            
            
            m_username = [cloudResponse.m_responseXmlDom getTextFromChildWithName:@"Username"];
            
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            
            NSString * result = [dom getTextFromChildWithName:@"StatusText"];
            
            if ( [result isEqualToString:@"Ok"] == YES )
            {
                m_loggedIn = YES;
            }
            else
            {
                m_loggedIn = NO;
            }
            
            XmlDom * profileDom = [dom getChildWithName:@"UserProfile"];
            
            UserProfile * userProfile = [[UserProfile alloc] initWithXmlDom:profileDom];
            
            cloudResponse.m_responseUserProfile = userProfile;
            
        } break;
            
//        case CloudRequestTypeLoginCookie:
//        {
//            
//            [m_username release];
//            
//            m_username = [[cloudResponse.m_responseXmlDom getTextFromChildWithName:@"Username"] retain];
//            
//            XmlDom * dom = cloudResponse.m_responseXmlDom;
//            
//            NSString * result = [dom getTextFromChildWithName:@"StatusText"];
//            
//            if ( [result isEqualToString:@"Ok"] == YES )
//            {
//                m_loggedIn = YES;
//            }
//            else
//            {
//                m_loggedIn = NO;
//            }
//            
//            XmlDom * profileDom = [dom getChildWithName:@"UserProfile"];
//            
//            UserProfile * userProfile = [[[UserProfile alloc] initWithXmlDom:profileDom] autorelease];
//            
//            cloudResponse.m_responseUserProfile = userProfile;
//            
//        } break;
            
        case CloudRequestTypeLogout: {
            m_loggedIn = NO;
            m_facebookAccessToken = nil;
        } break;
            
        case CloudRequestTypeGetUserProfile: {
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            XmlDom * profileDom = [dom getChildWithName:@"UserProfile"];
            UserProfile * userProfile = [[UserProfile alloc] initWithXmlDom:profileDom];
            
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
            cloudResponse.m_responseUserProfile = userProfile;
            
        } break;
            
        case CloudRequestTypeEditUserProfile: {
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            XmlDom * profileDom = [dom getChildWithName:@"UserProfile"];
            UserProfile * userProfile = [[UserProfile alloc] initWithXmlDom:profileDom];
            
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
            cloudResponse.m_responseUserProfile = userProfile;
        } break;
            
        case CloudRequestTypeSearchUserProfile: {
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            XmlDom * profilesDom = [dom getChildWithName:@"UserProfiles"];
            UserProfiles * userProfiles = [[UserProfiles alloc] initWithXmlDom:profilesDom];
            
            cloudResponse.m_responseUserProfiles = userProfiles;
        } break;
            
        case CloudRequestTypeSearchUserProfileFacebook: {
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            XmlDom * profilesDom = [dom getChildWithName:@"UserProfiles"];
            UserProfiles * userProfiles = [[UserProfiles alloc] initWithXmlDom:profilesDom];
            
            cloudResponse.m_responseUserProfiles = userProfiles;
            
        } break;
            
        case CloudRequestTypeGetUserCredits: {
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            
            cloudResponse.m_responseUserCredits = [dom getNumberFromChildWithName:@"Credits"];
        } break;
            
        case CloudRequestTypePurchaseSong: {
            // Nothing else to do for this
        } break;
            
        case CloudRequestTypeVerifyItunesReceipt: {
            // Nothing else to do for this
        } break;
            
        case CloudRequestTypeGetAllSongPids: {
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            XmlDom * pidsDom = [dom getChildWithName:@"AllProductIdentifiers"];
            
            NSArray * pidDomArray = [pidsDom getChildArrayWithName:@"ProductIdentifiers"];
            NSMutableArray * pidArray = [[NSMutableArray alloc] init];
            
            // Each element in the array is in a DOM, which is kind of annoying.
            // Iterate through and pull them out of the DOM and put into an array.
            for ( XmlDom * pidDom in pidDomArray ) {
                NSString * pid = [pidDom getText];
                [pidArray addObject:pid];
            }
            
            cloudResponse.m_responseProductIds = pidArray;
        } break;
            
        case CloudRequestTypeGetAllSongsList: {
            // Note that this is effectively just an allias of 
            // the CloudRequestTypeGetStoreSongList request right now.
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            XmlDom * songsDom = [dom getChildWithName:@"AllSongsList"];
            UserSongs * userSongs = [[UserSongs alloc] initWithXmlDom:songsDom];
            
            cloudResponse.m_responseUserSongs = userSongs;
        } break;
            
        case CloudRequestTypeGetUserSongList: {
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            XmlDom * songsDom = [dom getChildWithName:@"UserSongsList"];
            UserSongs * userSongs = [[UserSongs alloc] initWithXmlDom:songsDom];
            
            cloudResponse.m_responseUserSongs = userSongs;
        } break;
            
        case CloudRequestTypeGetStoreSongList: {
            XmlDom * dom = cloudResponse.m_responseXmlDom;
            XmlDom * songsDom = [dom getChildWithName:@"StoreSongsList"];
            UserSongs * userSongs = [[UserSongs alloc] initWithXmlDom:songsDom];
            
            cloudResponse.m_responseUserSongs = userSongs;
        } break;
            
        case CloudRequestTypeGetStoreFeaturesSongList: {
//            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
//            
//            // create the collection
//            cloudResponse.m_responseStoreFeatureCollection = [[[StoreFeatureCollection alloc] initWithXmlDom:responseDom] autorelease];
        } break;
            
        case CloudRequestTypePutUserSongSession: {
            cloudResponse.m_responseUserSongSession = cloudResponse.m_cloudRequest.m_userSongSession;
        } break;
            
        case CloudRequestTypeGetUserSongSessions: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            UserSongSessions * userSongSessions = [[UserSongSessions alloc] initWithXmlDom:responseDom];
            
            cloudResponse.m_responseUserSongSessions = userSongSessions;
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
        } break;
            
        case CloudRequestTypeAddUserFollows: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            XmlDom * userProfilesFollowsDom = [responseDom getChildWithName:@"FollowsUsers"];
            XmlDom * userProfilesFollowedDom = [responseDom getChildWithName:@"FollowedByUsers"];
            
            UserProfiles * userProfilesFollows = [[UserProfiles alloc] initWithXmlDom:userProfilesFollowsDom];
            UserProfiles * userProfilesFollowedBy = [[UserProfiles alloc] initWithXmlDom:userProfilesFollowedDom];
            UserSongSessions * userSongSessions = [[UserSongSessions alloc] initWithXmlDom:responseDom];
            
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
            cloudResponse.m_responseUserProfilesFollows = userProfilesFollows;
            cloudResponse.m_responseUserProfilesFollowedBy = userProfilesFollowedBy;
            cloudResponse.m_responseUserSongSessions = userSongSessions;
        } break;
            
        case CloudRequestTypeRemoveUserFollows: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            XmlDom * userProfilesFollowsDom = [responseDom getChildWithName:@"FollowsUsers"];
            XmlDom * userProfilesFollowedDom = [responseDom getChildWithName:@"FollowedByUsers"];
            
            UserProfiles * userProfilesFollows = [[UserProfiles alloc] initWithXmlDom:userProfilesFollowsDom];
            UserProfiles * userProfilesFollowedBy = [[UserProfiles alloc] initWithXmlDom:userProfilesFollowedDom];
            UserSongSessions * userSongSessions = [[UserSongSessions alloc] initWithXmlDom:responseDom];
            
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
            cloudResponse.m_responseUserProfilesFollows = userProfilesFollows;
            cloudResponse.m_responseUserProfilesFollowedBy = userProfilesFollowedBy;
            cloudResponse.m_responseUserSongSessions = userSongSessions;
        } break;
            
        case CloudRequestTypeGetUserFollowsList: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            XmlDom * userProfilesDom = [responseDom getChildWithName:@"FollowsUsers"];
            UserProfiles * userProfiles = [[UserProfiles alloc] initWithXmlDom:userProfilesDom];
            
            cloudResponse.m_responseUserProfiles = userProfiles;
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
        } break;
            
        case CloudRequestTypeGetUserFollowedList: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            XmlDom * userProfilesDom = [responseDom getChildWithName:@"FollowedByUsers"];
            UserProfiles * userProfiles = [[UserProfiles alloc] initWithXmlDom:userProfilesDom];
            
            cloudResponse.m_responseUserProfiles = userProfiles;
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
        } break;
            
        case CloudRequestTypeGetUserFollowsSongSessions: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            UserSongSessions * userSongSessions = [[UserSongSessions alloc] initWithXmlDom:responseDom];
            
            cloudResponse.m_responseUserSongSessions = userSongSessions;
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
        } break;
            
        case CloudRequestTypeGetUserGlobalSongSessions: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            
            UserSongSessions * userSongSessions = [[UserSongSessions alloc] initWithXmlDom:responseDom];
            
            cloudResponse.m_responseUserSongSessions = userSongSessions;
            cloudResponse.m_responseUserId = cloudResponse.m_cloudRequest.m_userId;
        } break;
            
        case CloudRequestTypeRedeemCreditCode: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            
            cloudResponse.m_responseUserCredits = [responseDom getNumberFromChildWithName:@"Credits"];
        } break;
            
        case CloudRequestTypePutLog: {
            // Nothing to do
        } break;
            
        case CloudRequestTypeGetCurrentFirmwareVersion: {
            XmlDom * responseDom = cloudResponse.m_responseXmlDom;
            
            cloudResponse.m_responseFirmwareMajorVersion = [responseDom getIntegerFromChildWithName:@"MajorVersion"];
            cloudResponse.m_responseFirmwareMinorVersion = [responseDom getIntegerFromChildWithName:@"MinorVersion"];
            cloudResponse.m_responseFileId = [responseDom getIntegerFromChildWithName:@"FileId"];
        } break;
            
        case CloudRequestTypeRegisterGtar: {
            XmlDom *responseDom = cloudResponse.m_responseXmlDom;
            NSLog(@"Register Gtar Status %d: %@", cloudResponse.m_statusCode, cloudResponse.m_statusText);
        } break;
            
        default: {
            // nothing
        } break;
    }
}

// Do some generic processing of the response data
- (void)preprocessResultsForResponse:(CloudResponse*)cloudResponse {
    // Parse the received data into an XmlDom
    XmlDom * dom = [[XmlDom alloc] initWithXmlData:cloudResponse.m_receivedData];
    cloudResponse.m_responseXmlDom = dom;
    NSString * result = [dom getTextFromChildWithName:@"StatusText"];
    
    // The request connection succeeded, but did it do what we asked
    if ( [result isEqualToString:@"Ok"] )
        cloudResponse.m_status = CloudResponseStatusSuccess;
    else
        cloudResponse.m_status = CloudResponseStatusFailure;
    
    // also see if we have been logged out as a result of this
    if ( [result isEqualToString:@"Unauthorized"] == YES )
        m_loggedIn = NO;
    
    NSString * detail = [dom getTextFromChildWithName:@"StatusDetail"];
    
    // Our net code is a little inconsistent on where failure reason is reported
    if ( detail != nil )
        cloudResponse.m_statusText = detail;
    else if ( result != nil )
        cloudResponse.m_statusText = result;
    else
        cloudResponse.m_statusText = @"Response parsing issue";
}
    
#pragma mark -
#pragma mark NSURLConnection delegates

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    CloudResponse * cloudResponse = [self deregisterConnection:connection];
    cloudResponse.m_cloudRequest.m_status = CloudRequestStatusConnectionError;

    [self cloudReceiveResponse:cloudResponse];
    [self performSelectorOnMainThread:@selector(handleError) withObject:nil waitUntilDone:YES];
    
#if TARGET_IPHONE_SIMULATOR
    NSLog(@"URL error: %@", error);
#endif
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response   {
    CloudResponse * cloudResponse = [self getReponseForConnection:connection];
    NSHTTPURLResponse * httpResponse = (NSHTTPURLResponse *)response;
    
//    cloudResponse.m_status = CloudResponseStatusReceivingData;
    
    cloudResponse.m_cloudRequest.m_status = CloudRequestStatusReceivingData;
    cloudResponse.m_mimeType = [httpResponse MIMEType];
    cloudResponse.m_statusCode = [httpResponse statusCode];
    cloudResponse.m_receivedData = [[NSMutableData alloc] init];
    cloudResponse.m_receivedData.length = 0;
}   

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data   {
    CloudResponse * cloudResponse = [self getReponseForConnection:connection];
    [cloudResponse.m_receivedData appendData:data];
}  

- (void)connectionDidFinishLoading:(NSURLConnection *)connection  {
    CloudResponse * cloudResponse = [self deregisterConnection:connection];
    cloudResponse.m_receivedDataString = [NSString stringWithCString:(char*)[cloudResponse.m_receivedData bytes] encoding:NSASCIIStringEncoding];
    cloudResponse.m_cloudRequest.m_status = CloudRequestStatusCompleted;

//    [self cloudReceiveResponse:cloudResponse];
    [self performSelectorInBackground:@selector(cloudReceiveResponse:) withObject:cloudResponse];
    
#if TARGET_IPHONE_SIMULATOR
    NSLog(@"%@",cloudResponse.m_receivedDataString);
#endif
}

@end
