//
//  FileController.m
//  gTarAppCore
//
//  Created by Marty Greenia on 11/3/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "FileController.h"

#import "CloudController.h"
#import "CloudRequest.h"
#import "CloudResponse.h"
#import "FileRequest.h"

#define MAX_FILE_CACHE_SIZE 300*1024*1024 // 300 MB
#define MAX_FILE_CACHE_SIZE_BUFFER 10*1024*1024 // 10 MB
#define MAX_IMAGE_SIZE 500.0

@implementation FileController

@synthesize m_cloudController;

- (id)initWithCloudController:(CloudController*)cloudController {
    self = [super init];
    
    if ( self ) {
        m_cloudController = cloudController;
        m_fileCacheMap = [[NSMutableDictionary alloc] init];
        m_pendingFileRequests = [[NSMutableDictionary alloc] init];
        
        // Scan the hdd for files that have been caches
        if ( [self enumerateFileCache] == NO )
            return nil;
    }
    
    return self;
}

- (void)clearCache {
    
    NSLog(@"Clearing the File cache!");
    
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString * cacheDirectory = [paths objectAtIndex:0];
    NSString * cachePath = [cacheDirectory stringByAppendingPathComponent:@"File"];
    
    NSError * error = nil;
    
    BOOL result;
    
    // Delete the old cache folder
    result = [[NSFileManager defaultManager] removeItemAtPath:cachePath error:&error];
    
    if ( result == NO || error != nil ) {
        NSLog(@"Failed to delete File cache");
        return;
    }
    
    // Now that all the files are deleted, clear the mapping to them.
    [m_fileCacheMap removeAllObjects];
    
    // Create a new cache folder
    result = [[NSFileManager defaultManager] createDirectoryAtPath:cachePath withIntermediateDirectories:YES attributes:nil error:&error];
    
    if ( result == NO || error != nil ) {
        NSLog(@"Error creating File cache path: %@", cachePath);
        return;
    }
}

- (BOOL)enumerateFileCache {
    [m_fileCacheMap removeAllObjects];
    
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString * cacheDirectory = [paths objectAtIndex:0];
    NSString * cachePath = [cacheDirectory stringByAppendingPathComponent:@"File"];
    
    NSError * error = nil;
    
    if ( [[NSFileManager defaultManager] fileExistsAtPath:cachePath] == NO ) {
        
        // Cache is empty, create it
        [[NSFileManager defaultManager] createDirectoryAtPath:cachePath withIntermediateDirectories:YES attributes:nil error:&error];
        
        if ( error != nil ) {
            NSLog(@"Error: '%@' creating File cache path: '%@'", [error localizedDescription], cachePath);
            return NO;
        }
        
    }
    else {
        NSArray * cacheContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:cachePath error:&error];
        
        if ( error != nil ) {
            NSLog(@"Error: '%@' enumerating File cache path: '%@'", [error localizedDescription], cachePath);
            return NO;
        }
        
        for ( NSString * fileName in cacheContents ) {
            NSArray * components = [fileName componentsSeparatedByString:@"."];
            NSString * keyStr = [components objectAtIndex:0];
            NSNumber * key = [NSNumber numberWithInteger:[keyStr integerValue]];
            NSString * filePath = [cachePath stringByAppendingPathComponent:fileName];
            
            if ( filePath == nil || key == nil )
                NSLog(@"Failed to map %@ to %@", key, filePath);
            else
                [m_fileCacheMap setObject:filePath forKey:key];
        }
    }
    
    return YES;
}

#pragma mark General

- (BOOL)fileExists:(NSInteger)fileId {
    NSNumber * key = [NSNumber numberWithInteger:fileId];
    NSString * filePath = [m_fileCacheMap objectForKey:key];
    
    return [[NSFileManager defaultManager] fileExistsAtPath:filePath];
}

- (id)loadFile:(NSInteger)fileId {
    NSNumber * key = [NSNumber numberWithInteger:fileId];
    NSString * filePath = [m_fileCacheMap objectForKey:key];
    
    if ( filePath == nil )
        return nil;
    
    // Refresh access time
    NSError * error;
    NSDate * now = [[NSDate alloc] init];
    
    [[NSFileManager defaultManager] setAttributes:[NSDictionary dictionaryWithObject:now forKey:NSFileModificationDate]
                                     ofItemAtPath:filePath
                                            error:&error];
    
    if ( [filePath hasSuffix:@".png"] == YES ) {
        UIImage * img = [UIImage imageWithContentsOfFile:filePath];
        return img;
    }
    else if ( [filePath hasSuffix:@".xmp"] == YES ) {
        NSString * str = [NSString stringWithContentsOfFile:filePath encoding:NSASCIIStringEncoding error:nil];
        return str;
    }
    else if ( [filePath hasSuffix:@".bin"] == YES ) {
        NSData * data = [NSData dataWithContentsOfFile:filePath];
        return data;
    }
    else {
        NSLog(@"Failed loading invalid file type");
        return nil;
    }

    return nil;
}

- (BOOL)saveFilePath:(NSString*)filePath withFileId:(NSInteger)fileId {
    if ( filePath == nil )
        return NO;
    
    id file = nil;
    
    if ( [filePath hasSuffix:@".png"] == YES ||
         [filePath hasSuffix:@".jpg"] == YES ||
         [filePath hasSuffix:@".jpeg"] == YES )
    {
        UIImage * img = [UIImage imageWithContentsOfFile:filePath];
        file = img;
    }
    else if ( [filePath hasSuffix:@".xmp"] == YES ) {
        NSString * str = [NSString stringWithContentsOfFile:filePath encoding:NSASCIIStringEncoding error:nil];
        file = str;
    }
    else if ( [filePath hasSuffix:@".bin"] == YES ) {
        NSData * data = [NSData dataWithContentsOfFile:filePath];
        file = data;
    }
    else {
        NSLog(@"Failed loading invalid file type before saving");
        return NO;
    }
        
    return [self saveFile:file withFileId:fileId];
}

- (BOOL)saveFile:(id)file withFileId:(NSInteger)fileId {
    
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString * cacheDirectory = [paths objectAtIndex:0];
    NSString * cachePath = [cacheDirectory stringByAppendingPathComponent:@"File"];
    
    NSString * fileName = nil;
    NSData * fileContents = nil;
    
    if ( [file isKindOfClass:[UIImage class]] == YES ) {
        UIImage * img = (UIImage*)file;
        
        fileName = [NSString stringWithFormat:@"%u.png", fileId];
        fileContents = UIImagePNGRepresentation(img);
    }
    else if ( [file isKindOfClass:[NSString class]] == YES ) {
        NSString * str = (NSString*)file;
        
        fileName = [NSString stringWithFormat:@"%u.xmp", fileId];
        fileContents = [str dataUsingEncoding:NSASCIIStringEncoding];
    }
    else if ( [file isKindOfClass:[NSData class]] == YES ) {
        NSData * data = (NSData*)file;
        
        fileName = [NSString stringWithFormat:@"%u.bin", fileId];
        fileContents = data;
    }
    else {
        NSLog(@"Failed to save invalid file type");
        return NO;
    }
    
    NSString * filePath = [cachePath stringByAppendingPathComponent:fileName];

    BOOL success = [fileContents writeToFile:filePath atomically:YES];
    
    if ( success == NO )
        return NO;
    
    NSNumber * key = [NSNumber numberWithInteger:fileId];
    
    [m_fileCacheMap setObject:filePath forKey:key];
    
    // Set the access time
    NSError * error;
    NSDate * now = [[NSDate alloc] init];
    [[NSFileManager defaultManager] setAttributes:[NSDictionary dictionaryWithObject:now forKey:NSFileModificationDate]
                                     ofItemAtPath:filePath
                                            error:&error];

    // Don't backup this file
    [self addSkipBackupAttributeToFileId:fileId];
    
    return YES;
    
}

- (id)getFileOrReturnNil:(NSInteger)fileId {
    // nil is ok
    return [self loadFile:fileId];
}

- (id)getFileOrDownloadSync:(NSInteger)fileId
{
    
    // FileId of zero is an invalid id
    if ( fileId == 0 )
    {
        return nil;
    }
    
    id file = [self loadFile:fileId];
    
    if ( file == nil )
    {
        // A nil obj/sel indicates sync
        CloudRequest * cloudRequest = [m_cloudController requestFile:fileId andCallbackObj:nil andCallbackSel:nil];
        
        CloudResponse * cloudResponse = cloudRequest.m_cloudResponse;
        
        file = [self receivedFileResponse:cloudResponse];
        
    }
    
    return file;

}

- (void)getFileOrDownloadAsync:(NSInteger)fileId callbackObject:(id)obj callbackSelector:(SEL)sel
{
    
    // FileId of zero is an invalid id
    if ( fileId == 0 )
    {
        [obj performSelector:sel withObject:nil];
        return;
    }
    
    id file = [self loadFile:fileId];
    
    if ( file == nil )
    {
        [self downloadFile:fileId callbackObject:obj callbackSelector:sel];
    }
    else
    {
        [obj performSelector:sel withObject:file];
    }
    
}

- (void)precacheFile:(NSInteger)fileId
{
    
    // FileId of zero is an invalid id
    if ( fileId == 0 )
    {
        return;
    }
    
    // We only cache the object if we don't have it already.
    NSNumber * key = [NSNumber numberWithInteger:fileId];
    
    if ( [m_fileCacheMap objectForKey:key] != nil )
    {
        return;
    }
    
    // download a file for the first time
    FileRequest * fileRequest = [m_pendingFileRequests objectForKey:key];
    
    // see if there is already a request pending
    if ( fileRequest == nil )
    {
        
        fileRequest = [[FileRequest alloc] initWithFileId:fileId];
        [m_pendingFileRequests setObject:fileRequest forKey:key];

        // send it to the cloud
        [m_cloudController requestFile:fileId andCallbackObj:self andCallbackSel:@selector(downloadFileCallback:)];

    }
    
}

- (void)downloadFile:(NSInteger)fileId callbackObject:(id)obj callbackSelector:(SEL)sel
{
    
    NSNumber * key = [NSNumber numberWithInteger:fileId];
    
    FileRequest * fileRequest = [m_pendingFileRequests objectForKey:key];
    
    // see if there is another request pending
    if ( fileRequest == nil )
    {
        // send off the request
        fileRequest = [[FileRequest alloc] initWithFileId:fileId andCallbackObject:obj andSelector:sel];
        [m_pendingFileRequests setObject:fileRequest forKey:key];
    }
    else
    {
        // If this request is already going, just add ourselves to the list of waiting objects
        [fileRequest addCallbackObject:obj andSelector:sel];
    }
    
    // send it to the cloud
    [m_cloudController requestFile:fileId andCallbackObj:self andCallbackSel:@selector(downloadFileCallback:)];
    
}

- (void)downloadFileCallback:(CloudResponse*)cloudResponse
{
    
    id file = [self receivedFileResponse:cloudResponse];
    
    NSNumber * key = [NSNumber numberWithInteger:cloudResponse.m_responseFileId];

    FileRequest * fileRequest = [m_pendingFileRequests objectForKey:key];
    
    if ( fileRequest != nil )
    {
        // send the file to the original caller(s)
        [fileRequest returnResponse:file];
        
        [m_pendingFileRequests removeObjectForKey:key];
    }
    
}

- (id)receivedFileResponse:(CloudResponse*)cloudResponse
{
    
    NSData * fileData = cloudResponse.m_receivedData;
    
    NSString * fileType = cloudResponse.m_mimeType;
    
    id file = [self createFile:fileData fromMimeType:fileType];
    
    NSNumber * key = [NSNumber numberWithInteger:cloudResponse.m_responseFileId];
    
    if ( file != nil )
    {
        [self saveFile:file withFileId:cloudResponse.m_responseFileId];
    }
    
    // Remove this below? Already done in downloadFileCallback
    FileRequest * fileRequest = [m_pendingFileRequests objectForKey:key];
    
    if ( fileRequest != nil )
    {
        [fileRequest returnResponse:file];
    }
    
    return file;
    
}

- (id)createFile:(NSData*)data fromMimeType:(NSString*)mimeType
{
    
    if ( [mimeType isEqualToString:@"text/xml"] == YES )
    {
        NSString * xmpBlob = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
        
        return xmpBlob;
    }
    else if ( ([mimeType isEqualToString:@"image/png"] == YES) ||
              ([mimeType isEqualToString:@"image/jpeg"] == YES) ||
              ([mimeType isEqualToString:@"image/jpg"] == YES) )
    {
        UIImage * image = [[UIImage alloc] initWithData:data];
        
        // Normalize as needed
        if ( image.size.width > MAX_IMAGE_SIZE || image.size.height > MAX_IMAGE_SIZE )
        {
            NSLog(@"Aspect-fitting image to %d x %d", (NSInteger)MAX_IMAGE_SIZE, (NSInteger)MAX_IMAGE_SIZE);
            
            CGSize newSize = image.size;
            
            if ( newSize.width > MAX_IMAGE_SIZE )
            {
                newSize.height = newSize.height / newSize.width * MAX_IMAGE_SIZE;
                newSize.width = MAX_IMAGE_SIZE;
            }
            
            if ( newSize.height > MAX_IMAGE_SIZE )
            {
                newSize.width = newSize.width / newSize.height * MAX_IMAGE_SIZE;
                newSize.height = MAX_IMAGE_SIZE;
            }
            
            UIGraphicsBeginImageContextWithOptions(newSize, NO, 1);
            CGContextSetInterpolationQuality(UIGraphicsGetCurrentContext(), kCGInterpolationHigh);
            [image drawInRect:CGRectMake(0,0,newSize.width,newSize.height)];
            UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
            UIGraphicsEndImageContext();
            
            image = newImage;
        }
        
        return image;
    }
    else if ( [mimeType isEqualToString:@"binary/octet-stream"] == YES )
    {
        return data;
    }
    
    return nil;
    
}

- (void)checkCacheSize
{
    
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString * cacheDirectory = [paths objectAtIndex:0];
    NSString * cachePath = [cacheDirectory stringByAppendingPathComponent:@"File"];
    
    NSArray * filesArray = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:cachePath error:nil];
    
    NSEnumerator * filesEnumerator = [filesArray objectEnumerator];
    NSString * fileName;
    
    unsigned int fileSize = 0;
    unsigned int fileCount = 0;
    
    while ( fileName = [filesEnumerator nextObject] )
    {
        NSDictionary * fileDictionary = [[NSFileManager defaultManager] attributesOfItemAtPath:[cachePath stringByAppendingPathComponent:fileName] error:nil];
        fileSize += [fileDictionary fileSize];
        fileCount++;
    }
    
    NSLog(@"Cache size: %u KB (%u files)", fileSize/1024, fileCount);
    
    if ( fileSize > MAX_FILE_CACHE_SIZE )
    {
        [self pruneCache:(fileSize - MAX_FILE_CACHE_SIZE) + MAX_FILE_CACHE_SIZE_BUFFER];
    }
    
}

- (void)pruneCache:(NSInteger)bytesToPrune
{
    
    if ( bytesToPrune <= 0 )
    {
        // nothing to do
        return;
    }
    
    NSLog(@"Pruning cache by %u bytes", bytesToPrune);
    
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString * cacheDirectory = [paths objectAtIndex:0];
    NSString * cachePath = [cacheDirectory stringByAppendingPathComponent:@"File"];
    
    NSArray * filesArray = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:cachePath error:nil];
    
    NSMutableArray * filesAndProperties = [[NSMutableArray alloc] init];
    
    for ( NSString * fileName in filesArray )
    {
        
        NSError * error;
        
        NSString * filePath = [cachePath stringByAppendingPathComponent:fileName];
        
        NSDictionary * propertiesDictionary = [[NSFileManager defaultManager] attributesOfItemAtPath:filePath error:&error];
        
        // Modification date
        NSDate * modDate = [propertiesDictionary objectForKey:NSFileModificationDate];
        
        // File extension
        NSArray * components = [fileName componentsSeparatedByString:@"."];
        NSString * extension = [components objectAtIndex:1];
        
        if ( error == nil )
        {
            [filesAndProperties addObject:[NSDictionary dictionaryWithObjectsAndKeys:
                                           filePath, @"FilePath",
                                           modDate, @"FileModDate",
                                           extension, @"FileExtension",
                                           nil]];
        }
        
    }
    
    NSArray * sortedFiles = [filesAndProperties sortedArrayUsingComparator:
                             ^(id dict1, id dict2)
                             {
                                 NSString * extension1 = [dict1 objectForKey:@"FileExtension"];
                                 NSString * extension2 = [dict2 objectForKey:@"FileExtension"];
                                 
                                 // An XMP file is always more valuable than anything else
                                 if ( [extension1 isEqualToString:@"xmp"] == YES &&
                                      [extension2 isEqualToString:@"xmp"] == NO )
                                 {
                                     return (NSComparisonResult)NSOrderedDescending;
                                 }
                                 
                                 if ( [extension1 isEqualToString:@"xmp"] == NO &&
                                     [extension2 isEqualToString:@"xmp"] == YES )
                                 {
                                     return (NSComparisonResult)NSOrderedAscending;
                                 }
                                 
                                 NSComparisonResult comp = [[dict1 objectForKey:@"FileModDate"] compare:
                                                            [dict2 objectForKey:@"FileModDate"]];
                                 
                                 return comp;
                             }];
    
    for ( NSDictionary * fileDictionary in sortedFiles )
    {
        // how big is this file
        NSString * filePath = [fileDictionary objectForKey:@"FilePath"];
        
        NSError * error = nil;
        
        NSDictionary * propertiesDictionary = [[NSFileManager defaultManager] attributesOfItemAtPath:filePath error:nil];
        
        [[NSFileManager defaultManager] removeItemAtPath:filePath error:&error];
        
        if ( error == nil )
        {
            bytesToPrune -= [propertiesDictionary fileSize];
            
            if ( bytesToPrune <= 0 )
            {
                // we're done
                break;
            }
        }
        
    }
    
    // Re-enumerate the files we have left
    [self enumerateFileCache];
    
}

#pragma mark - Permissions setting

// This changes a file's attributes so they aren't backed-up
- (BOOL)addSkipBackupAttributeToFileId:(NSInteger)fileId
{
    
    NSNumber * key = [NSNumber numberWithInteger:fileId];
    
    NSString * filePath = [m_fileCacheMap objectForKey:key];
    
    if ( filePath == nil )
    {
        return NO;
    }
    
    return [self addSkipBackupAttributeToItemAtURL:[NSURL fileURLWithPath:filePath]];
    
}

- (BOOL)addSkipBackupAttributeToItemAtURL:(NSURL *)URL
{
    
    if ( [[NSFileManager defaultManager] fileExistsAtPath:[URL path]] == NO )
    {
        NSLog(@"File does not exist, cannot set attributes.");
        return NO;
    }
    
    NSError * error = nil;
    
    BOOL success = [URL setResourceValue:[NSNumber numberWithBool:YES]
                                  forKey:NSURLIsExcludedFromBackupKey
                                   error:&error];
    
    if ( success == NO )
    {
        NSLog(@"Error excluding %@ from backup %@", [URL lastPathComponent], error);
    }
    
    return success;
    
}


@end
