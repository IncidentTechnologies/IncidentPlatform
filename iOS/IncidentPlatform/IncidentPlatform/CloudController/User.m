//
//  User.m
//  gTarAppCore
//
//  Created by Marty Greenia on 4/11/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "User.h"
#import "XmlDom.h"

@implementation User

@synthesize m_username;
@synthesize m_userId;

- (id)initWithXmlDom:(XmlDom*)xmlDom
{
    
    if ( xmlDom == nil )
    {
        return nil;
    }
    
    self = [super init];
    
    if ( self )
    {
        
        self.m_username = [xmlDom getTextFromChildWithName:@"username"];
        self.m_userId = [xmlDom getIntegerFromChildWithName:@"id"];

    }
    
    return self;
    
}


// Encode an object to an archive
- (void)encodeWithCoder:(NSCoder *)coder
{
    
    [coder encodeObject:m_username forKey:@"Username"];
    [coder encodeInteger:m_userId forKey:@"UserId"];
    
}

// Decode an object from an archive
- (id)initWithCoder:(NSCoder *)coder
{
	self = [super init];
	
    self.m_username = [coder decodeObjectForKey:@"Username"];
    self.m_userId = [coder decodeIntegerForKey:@"UserId"];
    
	return self;
    
}

@end
