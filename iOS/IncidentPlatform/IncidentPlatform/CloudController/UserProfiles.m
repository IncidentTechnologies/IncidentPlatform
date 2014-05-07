//
//  UserProfiles.m
//  gTarAppCore
//
//  Created by Marty Greenia on 9/21/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "UserProfiles.h"
#import "UserProfile.h"
#import "XmlDom.h"

@implementation UserProfiles

@synthesize m_profilesArray;

- (id)initWithXmlDom:(XmlDom*)xmlDom
{

    if ( xmlDom == nil )
    {
        return nil;
    }
    
    self = [super init];
    
    if ( self )
    {
        
        m_profilesArray = [[NSMutableArray alloc] init];
        
        [self addProfilesFromXmlDom:xmlDom];
        
    }
    
    return self;
    
}

- (void)addProfilesFromXmlDom:(XmlDom*)xmlDom
{
    
    NSArray * profilesArray = [xmlDom getChildArrayWithName:@"UserProfile"];
    
    for ( XmlDom * profileDom in profilesArray )
    {
        UserProfile * userProfile = [[UserProfile alloc] initWithXmlDom:profileDom];
        
        [m_profilesArray addObject:userProfile];
        
    }
    
}


@end
