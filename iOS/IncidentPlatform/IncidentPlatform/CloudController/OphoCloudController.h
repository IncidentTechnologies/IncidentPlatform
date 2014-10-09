//
//  OphoCloudController.h
//  Incident Platform
//
//  Created by Kate Schnippering on 10/08/14.
//  Copyright 2014 IncidentTech. All rights reserved.
//

#import <UIKit/UIKit.h>

@class UserProfile;
@class UserSong;
@class UserSongSession;
@class CloudRequest;
@class CloudResponse;

typedef enum
{
    OphoXmpTypeSong = 1,
    OphoXmpTypeSongSession,
    OphoXmpTypeXMPSample,
    OphoXmpTypeXMPInstrument,
    OphoXmpTypeXMPClip,
    OphoXmpTypeXMPEffect,
    OphoXmpTypeSequence
} OphoXmpType;


#define PERMISSIONS_PUBLIC @"PUBLIC"
#define PERMISSIONS_PRIVATE @"PRIVATE"
#define PERMISSIONS_OPEN @"OPEN"


#define OPHO_DEV_SERVER
//#define OPHO_SERVER

#ifdef OPHO_DEV_SERVER
    #define kServerAddress @"http://api-dev.opho.com/"
#endif

#ifdef OPHO_SERVER
    #define kServerAddress @"http://api.opho.com/"
#endif


@interface OphoCloudController : NSObject {
    BOOL m_online;
    BOOL m_loggedIn;
    
    NSString * m_username;
    NSString * m_facebookAccessToken;
    
	// Request->Response mapping
	NSMutableDictionary * m_requestResponseDictionary;
    
    // Connection->Request mapping
    NSMutableDictionary * m_connectionResponseDictionary;
    
    NSMutableArray * m_requestQueue;
    
    NSString * m_serverName;
    NSString * m_serverRoot;
    
    NSInteger m_errorsRecently;
}

@property (nonatomic, readonly) BOOL m_loggedIn;
@property (nonatomic, readonly) NSString * m_username;
@property (nonatomic, readonly) NSString * m_facebookAccessToken;

- (id)initWithServer:(NSString*)serverName;
+ (OphoCloudController*)sharedSingleton;

- (void)handleError;
- (void)errorWindowFinished;
- (void)queryOnlineStatus;
- (void)receiveOnlineStatus:(CloudResponse*)cloudResponse;

// Opho Server Requests
- (CloudRequest*)requestServerStatusCallbackObj:(id)obj andCallbackSel:(SEL)sel;


// Opho User Requests
- (CloudRequest*)requestUserStatusCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestRegisterUsername:(NSString*)username andPassword:(NSString*)password andEmail:(NSString*)email andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestActivateWithKey:(NSString*)key andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestLoginUsername:(NSString*)username andPassword:(NSString*)password andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestLogoutCallbackObj:(id)obj andCallbackSel:(SEL)sel;


// Opho XMP Requests

- (CloudRequest*)requestNewXmpWithFolderId:(NSInteger)folderId andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestDeleteXmpWithId:(NSInteger)xmpId andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestSaveXmpWithId:(NSInteger)xmpId andXmpFile:(id)file andXmpData:(NSData *)data andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestGetXmpWithId:(NSInteger)xmpId isXmpOnly:(BOOL)xmpOnly andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestGetXmpListWithType:(NSInteger)type andUserId:(NSInteger)userId andAppId:(NSInteger)appId andPermission:(NSString*)permission andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestSetXmpFolderWithId:(NSInteger)xmpId andFolderId:(NSInteger)folderId andCallbackObj:(id)obj andCallbackSel:(SEL)sel;

- (CloudRequest*)requestSetXmpPermissionWithId:(NSInteger)xmpId andUserId:(NSInteger)userId andPermission:(NSString *)permission andCallbackObj:(id)obj andCallbackSel:(SEL)sel;


// Deprecated or converting //----------------------------------------------

- (CloudRequest*)requestFile:(NSInteger)fileId andCallbackObj:(id)obj andCallbackSel:(SEL)sel;


- (void)registerConnection:(NSURLConnection*)connection toResponse:(CloudResponse*)cloudResponse;
- (CloudResponse*)getReponseForConnection:(NSURLConnection*)connection;
- (CloudResponse*)deregisterConnection:(NSURLConnection*)connection;

- (CloudResponse*)cloudSendRequest:(CloudRequest*)cloudRequest;
- (CloudResponse*)cloudProcessRequest:(CloudRequest*)cloudRequest;
- (void)cloudReceiveResponse:(CloudResponse*)cloudResponse;
- (void)cloudReturnResponse:(CloudResponse*)cloudResponse;

- (NSURLRequest*)createPostForRequest:(CloudRequest*)cloudRequest;
- (void)marshalArgumentsForRequest:(CloudRequest*)cloudRequest withUrl:(NSString**)urlString andParameters:(NSArray**)paramsArray andFiles:(NSArray**)filesArray;
- (void)parseResultsForResponse:(CloudResponse*)cloudResponse;
- (void)preprocessResultsForResponse:(CloudResponse*)cloudResponse;

@end
