//
//  FileRequest.h
//  gTarAppCore
//
//  Created by Marty Greenia on 11/14/11.
//  Copyright (c) 2011 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FileRequest : NSObject
{
    
    NSInteger fileId;
    
    NSMutableArray * m_callbackObjectArray;
    NSMutableArray * m_callbackSelectorArray;

}

@property (nonatomic, readonly) NSInteger fileId;

- (id)initWithFileId:(NSInteger)fileId;
- (id)initWithFileId:(NSInteger)fileId andCallbackObject:(id)obj andSelector:(SEL)sel;

- (void)addCallbackObject:(id)obj andSelector:(SEL)sel;
- (void)returnResponse:(id)file;

@end
