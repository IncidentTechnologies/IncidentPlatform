//
//  CloudRequest3.h
//  gTarAppCore
//
//  Created by Marty Greenia on 4/24/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

@class CloudResponse;
@class UserSong;
@class UserSongSession;
@class UIImage;

typedef enum
{
	CloudRequestStatusUnknown = 0,
    CloudRequestStatusPending,
    CloudRequestStatusSent,
    CloudRequestStatusReceivingData,
    CloudRequestStatusCompleted,
    CloudRequestStatusConnectionError,
    CloudRequestStatusOffline
} CloudRequestStatus;

typedef enum {
    CloudRequestTypeUnknown = 0,
    
    CloudRequestTypeGetServerStatus,    //opho
    CloudRequestTypeGetFile,
    
    CloudRequestTypeGetUserStatus,      //opho
    CloudRequestTypeRegister,           //opho
    CloudRequestTypeActivate,           //opho
    //CloudRequestTypeLoginFacebook,
    CloudRequestTypeLogin,              //opho
    //CloudRequestTypeLoginCookie,
    CloudRequestTypeLogout,             //opho
    
    CloudRequestTypeNewXmp,             //opho
    CloudRequestTypeDeleteXmp,          //opho
    CloudRequestTypeSaveXmp,            //opho
    CloudRequestTypeGetXmp,             //opho
    CloudRequestTypeGetXmpList,         //opho
    CloudRequestTypeSetXmpFolder,       //opho
    CloudRequestTypeSetXmpPermission,   //opho
    
    CloudRequestTypeGetUserProfile,
    CloudRequestTypeEditUserProfile,
    CloudRequestTypeSearchUserProfile,
    CloudRequestTypeSearchUserProfileFacebook,
    CloudRequestTypeGetUserCredits,
    CloudRequestTypePurchaseSong,
    CloudRequestTypeVerifyItunesReceipt,
    
    CloudRequestTypeGetAllSongPids,
    CloudRequestTypeGetAllSongsList,
    CloudRequestTypeGetUserSongList,
    CloudRequestTypeGetStoreSongList,
    CloudRequestTypeGetStoreFeaturesSongList,
    
    CloudRequestTypePutUserSongSession,
    CloudRequestTypeGetUserSongSessions,
    
    CloudRequestTypeAddUserFollows,
    CloudRequestTypeRemoveUserFollows,
    CloudRequestTypeGetUserFollowsList,
    CloudRequestTypeGetUserFollowedList,
    CloudRequestTypeGetUserFollowsSongSessions,
    CloudRequestTypeGetUserGlobalSongSessions,
    
    CloudRequestTypeRedeemCreditCode,
    CloudRequestTypePutLog,
    CloudRequestTypeGetCurrentFirmwareVersion,
    
    CloudRequestTypeRegisterGtar
    
} CloudRequestType;

@interface CloudRequest : NSObject {
    
   // CloudResponse * __weak m_cloudResponse;
    CloudResponse * m_cloudResponse;
    
    // Async callbacks 
	id m_callbackObject;
	SEL m_callbackSelector;
    
    CloudRequestType m_type;
	CloudRequestStatus m_status;
    
    BOOL m_isSynchronous;
    
    //
    // Request specific parameters
    //
    
    // CloudRequestTypeLoginFacebook
    NSString * m_facebookAccessToken;
    
    // CloudRequestTypeLogin, CloudRequestTypeRegister
    NSString * m_username;
    NSString * m_password;
    NSString * m_email;
    NSHTTPCookie * m_cookie;
    
    // CloudRequestTypeGetFile
    NSInteger m_fileId;
    
    // CloudRequestTypeGetUserProfile, CloudRequestTypeGetUserSongSessions,
    // CloudRequestTypeAddUserFollows, CloudRequestTypeRemoveUserFollows,
    // CloudRequestTypeGetUserFollowedList, CloudRequestTypeGetUserFollowsSongSessions,
    NSInteger m_userId;
    NSInteger m_page;
    
    // CloudRequestTypeSearchUserProfile
    NSString * m_searchString;
    
    // CloudRequestTypeRedeemCreditCode
    NSString * m_creditCode;
    
    // CloudRequestTypePurchaseSong
    UserSong * m_userSong;
    
    // CloudRequestTypePutUserSongSession
    UserSongSession * m_userSongSession;
    
    // CloudRequestTypeVerifyItunesReceipt
    NSData * m_itunesReceipt;
    
    // CloudRequestTypeEditUserProfile 
    UIImage * m_profileImage;
    
    // CloudRequestTypePutLog
    NSString * m_logEntries;
    NSString * m_versionString;
    NSString * m_deviceString;
    NSString * m_appString;
}

@property (nonatomic, strong) CloudResponse * m_cloudResponse;

@property (nonatomic, readonly) id m_callbackObject;
@property (nonatomic, readonly) SEL m_callbackSelector;

@property (nonatomic, readonly) CloudRequestType m_type;
@property (nonatomic, assign) CloudRequestStatus m_status;
@property (nonatomic, readonly) BOOL m_isSynchronous;

@property (nonatomic, strong) NSString * m_facebookAccessToken;
@property (nonatomic, strong) NSString * m_username;
@property (nonatomic, strong) NSString * m_password;
@property (nonatomic, strong) NSString * m_email;
@property (nonatomic, strong) NSHTTPCookie * m_cookie;
@property (nonatomic, assign) NSInteger m_fileId;
@property (nonatomic, assign) NSInteger m_userId;
@property (nonatomic, assign) NSInteger m_page;
@property (nonatomic, strong) NSString * m_searchString;
@property (nonatomic, strong) NSString * m_creditCode;
@property (nonatomic, strong) UserSong * m_userSong;
@property (nonatomic, strong) UserSongSession * m_userSongSession;
@property (nonatomic, strong) NSData * m_itunesReceipt;
@property (nonatomic, strong) UIImage * m_profileImage;
@property (nonatomic, strong) NSString * m_logEntries;
@property (nonatomic, strong) NSString * m_versionString;
@property (nonatomic, strong) NSString * m_deviceString;
@property (nonatomic, strong) NSString * m_appString;

@property (nonatomic, strong) NSString *m_serial_lower;
@property (nonatomic, strong) NSString *m_serial_upper;

// Opho
@property (nonatomic, assign) NSInteger m_appId;
@property (nonatomic, strong) NSString * m_activationKey;
@property (nonatomic, strong) NSString * m_permissionLevel;
@property (nonatomic, strong) NSNumber * m_rememberLogin;
@property (nonatomic, assign) NSInteger m_xmpId;
@property (nonatomic, assign) NSInteger m_folderId;
@property (nonatomic, strong) id m_xmpFile;
@property (nonatomic, assign) NSInteger m_xmpType;
@property (nonatomic, strong) NSData * m_xmpData;
@property (nonatomic, strong) NSNumber * m_xmpOnly;


- (id)initWithType:(CloudRequestType)type;
- (id)initWithType:(CloudRequestType)type andCallbackObject:(id)obj andCallbackSelector:(SEL)sel;


@end
