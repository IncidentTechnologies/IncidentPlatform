//
//  OphoCloudController.m
//  Incident Platform
//
//  Created by Kate Schnippering on 10/08/14.
//  Copyright 2014 IncidentTech. All rights reserved.
//

#import "OphoCloudController.h"

#import "CloudResponse.h"
#import "CloudRequest.h"
#import "UserProfile.h"
#import "UserProfiles.h"

#import "XmlDictionary.h"
#import "XmlDom.h"

#define SERVER_NAME_DEFAULT @"http://www.strumhub.com/v0.53"
#define SERVER_ROOT_DEFAULT @"http://www.strumhub.com/v0.53/app_iphone"
#define SERVER_NAME m_serverName
#define SERVER_ROOT m_serverRoot

#define GET_SERVER_STATUS @"Main/ServerStatus"
#define GET_ITUNES_STATUS @"Main/ItunesStatus"

#define CloudRequestTypeGetFileUrl @"UserFiles/GetFile"

// Opho Server
#define CloudRequestTypeGetServerStatusUrl @"server/status/"

// Opho User
#define CloudRequestTypeGetUserStatusUrl @"user/status/"
#define CloudRequestTypeRegisterUrl @"user/register/"
#define CloudRequestTypeActivateUrl @"user/activate/"
#define CloudRequestTypeLoginUrl @"user/login/"
#define CloudRequestTypeLogoutUrl @"user/logout/"

// Opho XMP
#define CloudRequestTypeNewXmpUrl @"xmp/new/"
#define CloudRequestTypeDeleteXmpUrl @"xmp/delete/"
#define CloudRequestTypeSaveXmpUrl @"xmp/save/"
#define CloudRequestTypeGetXmpUrl @"xmp/get/"
#define CloudRequestTypeGetXmpListUrl @"xmp/getlist/"
#define CloudRequestTypeSetXmpFolderUrl @"xmp/setfolder/"
#define CloudRequestTypeSetXmpPermissionUrl @"xmp/setpermission/"

// maybe append a clock() or tick() to this thing
#define POST_BOUNDARY @"------gTarPlayFormBoundary0123456789"

#define URL_REQUEST_TIMEOUT 30
//#define URL_REQUEST_TIMEOUT 0.0000001

// X number of time outs every Y seconds sends us offline
#define ERRORS_BEFORE_OFFLINE 6
#define ERROR_WINDOW 120

#define TEST_ONLINE_STATUS_PERIOD 60 // check every 1 minute

@implementation OphoCloudController

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
        
	}
	
	return self;
}

- (id)initWithServer:(NSString*)serverName {
    // Note this is 'self' not 'super'
    self = [self init];
    
    if ( self )  {
        m_serverName = serverName;
        m_serverRoot = serverName;
    }
    
    return self;
}

+ (OphoCloudController *)sharedSingleton
{
    static OphoCloudController *sharedSingleton;
    
    @synchronized(self)
    {
        if (!sharedSingleton)
        {
            NSString* server = kServerAddress;
            sharedSingleton = [[OphoCloudController alloc] initWithServer:server];
        }
        
        return sharedSingleton;
    }
}


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
/*
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
*/

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

/*
- (CloudResponse*)requestFileSync:(NSInteger)fileId
{

    // Create sync request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetFile];
    
    cloudRequest.m_fileId = fileId;
    
    CloudResponse * cloudResponse = [self cloudSendRequest:cloudRequest];
        
    return cloudResponse;
        
}
*/

#pragma mark Opho User

- (CloudRequest*)requestUserStatusCallbackObj:(id)obj
                               andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetUserStatus andCallbackObject:obj andCallbackSelector:sel];
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
}

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

- (CloudRequest*)requestActivateWithKey:(NSString *)key
                         andCallbackObj:(id)obj
                         andCallbackSel:(SEL)sel
{
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeActivate andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_activationKey = key;
    
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
    cloudRequest.m_rememberLogin = [NSNumber numberWithBool:YES];
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
	
}

- (CloudRequest*)requestLogoutCallbackObj:(id)obj
                           andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeLogout andCallbackObject:obj andCallbackSelector:sel];
    
    m_loggedIn = NO;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
	
}


#pragma mark Opho XMP

- (CloudRequest*)requestNewXmpWithFolderId:(NSInteger)folderId
                            andCallbackObj:(id)obj
                            andCallbackSel:(SEL)sel
{
    
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeNewXmp andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_folderId = folderId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
}

- (CloudRequest*)requestDeleteXmpWithId:(NSInteger)xmpId
                         andCallbackObj:(id)obj
                         andCallbackSel:(SEL)sel
{
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeDeleteXmp andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_xmpId = xmpId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
}

- (CloudRequest*)requestSaveXmpWithId:(NSInteger)xmpId
                           andXmpFile:(id)file
                           andXmpData:(NSData *)data
                       andCallbackObj:(id)obj
                       andCallbackSel:(SEL)sel
{
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeSaveXmp andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_xmpId = xmpId;
    cloudRequest.m_xmpFile = file;
    cloudRequest.m_xmpData = data;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
}

- (CloudRequest*)requestGetXmpWithId:(NSInteger)xmpId
                           isXmpOnly:(BOOL)xmpOnly
                      andCallbackObj:(id)obj
                      andCallbackSel:(SEL)sel
{
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetXmp andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_xmpId = xmpId;
    cloudRequest.m_xmpOnly = [NSNumber numberWithBool:xmpOnly];
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
}

- (CloudRequest*)requestGetXmpListWithType:(NSInteger)type
                                 andUserId:(NSInteger)userId
                                  andAppId:(NSInteger)appId
                             andPermission:(NSString *)permission
                            andCallbackObj:(id)obj
                            andCallbackSel:(SEL)sel
{
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeGetXmpList andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_xmpType = type;
    cloudRequest.m_userId = userId;
    cloudRequest.m_appId = appId;
    cloudRequest.m_permissionLevel = permission;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
}

- (CloudRequest*)requestSetXmpFolderWithId:(NSInteger)xmpId
                               andFolderId:(NSInteger)folderId
                            andCallbackObj:(id)obj
                            andCallbackSel:(SEL)sel
{
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeSetXmpFolder andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_xmpId = xmpId;
    cloudRequest.m_folderId = folderId;
    
    [self cloudSendRequest:cloudRequest];
    
    return cloudRequest;
}

- (CloudRequest*)requestSetXmpPermissionWithId:(NSInteger)xmpId
                                     andUserId:(NSInteger)userId
                                 andPermission:(NSString *)permission
                                andCallbackObj:(id)obj
                                andCallbackSel:(SEL)sel
{
    // Create async request
    CloudRequest * cloudRequest = [[CloudRequest alloc] initWithType:CloudRequestTypeSetXmpPermission andCallbackObject:obj andCallbackSelector:sel];
    
    cloudRequest.m_xmpId = xmpId;
    cloudRequest.m_userId = userId;
    cloudRequest.m_permissionLevel = permission;
    
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
    
    NSString * rootedUrlString = [NSString stringWithFormat:@"%@%@", SERVER_ROOT, urlString];
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
        
        // OPHO
        case CloudRequestTypeGetServerStatus:
        {
            url = CloudRequestTypeGetServerStatusUrl;
            
        } break;
            
        // CONVERTING/DEPRECATING
        case CloudRequestTypeGetFile:
        {
            
            url = CloudRequestTypeGetFileUrl;
            
            NSDictionary * param = [NSDictionary dictionaryWithObjectsAndKeys:
                                    @"data[UserFiles][id]", @"Name",
                                    [NSNumber numberWithInteger:cloudRequest.m_fileId], @"Value", nil];
            
            params = [NSArray arrayWithObject:param];
            
        } break;
            
        // OPHO
        case CloudRequestTypeGetUserStatus:
        {
            url = CloudRequestTypeGetUserStatusUrl;
            
        } break;
            
        // OPHO
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
            
        // OPHO
        case CloudRequestTypeActivate:
        {
            url = CloudRequestTypeActivateUrl;
            
            // key
            
        } break;
            
        // OPHO
        case CloudRequestTypeLogin:
        {
            
            url = CloudRequestTypeLoginUrl;
            
            NSDictionary * param1 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"username", @"Name",
                                     cloudRequest.m_username, @"Value", nil];
            
            NSDictionary * param2 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"password", @"Name",
                                     cloudRequest.m_password, @"Value", nil];
            
            // Optional: email
            
            NSDictionary * param4 = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"remember", @"Name",
                                     cloudRequest.m_rememberLogin, @"Value", nil];
            
            
            params = [NSArray arrayWithObjects:param1, param2, param4, nil];
            
        } break;
            
        // OPHO
        case CloudRequestTypeLogout:
        {
            url = CloudRequestTypeLogoutUrl;
            
        } break;
            
        // OPHO
        case CloudRequestTypeNewXmp:
        {
            url = CloudRequestTypeNewXmpUrl;
            
            // xmpfolderid=#
            
        } break;
            
        // OPHO
        case CloudRequestTypeDeleteXmp:
        {
            url = CloudRequestTypeDeleteXmpUrl;
            
            // xmpid=#
            
        } break;
            
        // OPHO
        case CloudRequestTypeSaveXmp:
        {
            url = CloudRequestTypeSaveXmpUrl;
            
            //xmpid
            //xmpfile
            //xmpdata
            
        } break;
            
        // OPHO
        case CloudRequestTypeGetXmp:
        {
            url = CloudRequestTypeGetXmpUrl;
            
            //xmpid
            //xmponly
            
        } break;

        // OPHO
        case CloudRequestTypeGetXmpList:
        {
            url = CloudRequestTypeGetXmpListUrl;
            
            //type
            //appid
            //permission
            
        } break;
            
        // OPHO
        case CloudRequestTypeSetXmpFolder:
        {
            url = CloudRequestTypeSetXmpFolderUrl;
            
            //xmpid
            //xmpfolderid
            
        } break;
            
        // OPHO
        case CloudRequestTypeSetXmpPermission:
        {
            url = CloudRequestTypeSetXmpPermissionUrl;
            
            //xmpid
            //userid
            //permission
            
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
            
        case CloudRequestTypeGetUserStatus:
        {
            
            NSLog(@"Cloud Response: Get User Status");
            
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
            
        case CloudRequestTypeActivate:
        {
            
            NSLog(@"Cloud Response: Activate");
            
        } break;
            
        case CloudRequestTypeLogin:
        {
            
            NSLog(@"Cloud Response: Login");
            
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
            
        case CloudRequestTypeLogout: {
            m_loggedIn = NO;
            m_facebookAccessToken = nil;
        } break;
            
        case CloudRequestTypeNewXmp:
        {
            NSLog(@"Cloud Response: New Xmp");
            
        } break;
            
        case CloudRequestTypeDeleteXmp:
        {
            NSLog(@"Cloud Response: Delete Xmp");
            
        } break;
            
        case CloudRequestTypeSaveXmp:
        {
            NSLog(@"Cloud Response: Save Xmp");
            
        } break;
            
        case CloudRequestTypeGetXmp:
        {
            NSLog(@"Cloud Response: Get Xmp");
            
        } break;
            
        case CloudRequestTypeGetXmpList:
        {
            NSLog(@"Cloud Response: Get Xmp List");
            
        } break;
            
        case CloudRequestTypeSetXmpFolder:
        {
            NSLog(@"Cloud Response: Set Xmp Folder");
            
        } break;
            
        case CloudRequestTypeSetXmpPermission:
        {
            NSLog(@"Cloud Response: Set Xmp Permission");
            
        } break;
                        
        default: {
            
            NSLog(@"Cloud request type DEFAULT");
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
