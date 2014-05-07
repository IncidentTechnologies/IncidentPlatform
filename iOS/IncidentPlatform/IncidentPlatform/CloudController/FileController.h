//
//  FileController.h
//  gTarAppCore
//
//  Created by Marty Greenia on 11/3/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <UIKit/UIKit.h>

@class CloudController;
@class CloudResponse;
@class CloudRequest;

@interface FileController : NSObject
{
    
    CloudController * m_cloudController;
    
    NSMutableDictionary * m_fileCacheMap;
    
    NSMutableDictionary * m_pendingFileRequests;
    
}

@property (nonatomic, strong) CloudController * m_cloudController;

- (id)initWithCloudController:(CloudController*)cloudController;
- (void)clearCache;
- (BOOL)enumerateFileCache;

// general file
- (BOOL)fileExists:(NSInteger)fileId;
- (id)loadFile:(NSInteger)fileId;
- (BOOL)saveFilePath:(NSString*)filePath withFileId:(NSInteger)fileId;
- (BOOL)saveFile:(id)file withFileId:(NSInteger)fileId;

- (id)getFileOrReturnNil:(NSInteger)fileId;
- (id)getFileOrDownloadSync:(NSInteger)fileId;
- (void)getFileOrDownloadAsync:(NSInteger)fileId callbackObject:(id)obj callbackSelector:(SEL)sel;

- (void)precacheFile:(NSInteger)fileId;
- (void)downloadFile:(NSInteger)fileId callbackObject:(id)obj callbackSelector:(SEL)sel;
- (void)downloadFileCallback:(CloudResponse*)cloudResponse;

- (id)receivedFileResponse:(CloudResponse*)cloudResponse;
- (id)createFile:(NSData*)data fromMimeType:(NSString*)mimeType;

- (void)checkCacheSize;
- (void)pruneCache:(NSInteger)bytesToPrune;

- (BOOL)addSkipBackupAttributeToFileId:(NSInteger)fileId;
- (BOOL)addSkipBackupAttributeToItemAtURL:(NSURL *)URL;



@end
