//
//  UserSongSessions.m
//  gTarAppCore
//
//  Created by Marty Greenia on 9/8/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "UserSongSessions.h"
#import "UserSongSession.h"
#import "XmlDom.h"

@implementation UserSongSessions

@synthesize m_sessionsArray;

- (id)initWithXmlDom:(XmlDom*)xmlDom
{
    
    if ( xmlDom == nil )
    {
        return nil;
    }
    
    self = [super init];
    
    if ( self )
    {
        
        m_sessionsArray = [[NSMutableArray alloc] init];
        
        [self addSessionsFromXmlDom:xmlDom];
        
        [m_sessionsArray sortUsingSelector:@selector(compareCreatedNewestFirst:)];
        
    }
    
    return self;
    
}


- (void)addSessionsFromXmlDom:(XmlDom*)xmlDom
{
    
    XmlDom * sessionsDom = [xmlDom getChildWithName:@"UserSongSessions"];
    NSArray * userSongSessionsDom = [sessionsDom getChildArrayWithName:@"UserSongSession"];
    
    for ( XmlDom * sessionDom in userSongSessionsDom )
    {
        UserSongSession * userSongSession = [[UserSongSession alloc] initWithXmlDom:sessionDom];
        
        [m_sessionsArray addObject:userSongSession];
        
    }
    
}



@end
