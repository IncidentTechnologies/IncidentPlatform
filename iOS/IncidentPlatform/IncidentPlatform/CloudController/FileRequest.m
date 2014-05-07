//
//  FileRequest.m
//  gTarAppCore
//
//  Created by Marty Greenia on 11/14/11.
//  Copyright (c) 2011 IncidentTech. All rights reserved.
//

#import "FileRequest.h"

@implementation FileRequest

@synthesize fileId;

- (id)initWithFileId:(NSInteger)fileId
{
    
	self = [super init];
    
	if ( self )
	{
		
        m_callbackObjectArray = [[NSMutableArray alloc] init];
        m_callbackSelectorArray = [[NSMutableArray alloc] init];
        
//        [self addCallbackObject:obj andSelector:sel];
        
	}
	
	return self;
	
}

- (id)initWithFileId:(NSInteger)fileId andCallbackObject:(id)obj andSelector:(SEL)sel
{
    
	self = [super init];
    
	if ( self )
	{
		
        m_callbackObjectArray = [[NSMutableArray alloc] init];
        m_callbackSelectorArray = [[NSMutableArray alloc] init];

        [self addCallbackObject:obj andSelector:sel];
        
	}
	
	return self;
	
}

- (void)addCallbackObject:(id)obj andSelector:(SEL)sel
{
    [m_callbackObjectArray addObject:obj];
    [m_callbackSelectorArray addObject:NSStringFromSelector(sel)];
}


- (void)returnResponse:(id)file
{
	
    for ( NSInteger index = 0; index < [m_callbackObjectArray count]; index++ )
    {
        
        id obj = [m_callbackObjectArray objectAtIndex:index];
        
        NSString * selString = [m_callbackSelectorArray objectAtIndex:index];
        
        SEL sel = NSSelectorFromString(selString);
        
        // todo
        [obj performSelector:sel withObject:file];
    }
    
    [m_callbackObjectArray removeAllObjects];
    [m_callbackSelectorArray removeAllObjects];
    
}

@end
