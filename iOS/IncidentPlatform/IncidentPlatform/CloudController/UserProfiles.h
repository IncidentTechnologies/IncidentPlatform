//
//  UserProfiles.h
//  gTarAppCore
//
//  Created by Marty Greenia on 9/21/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@class UserProfile;
@class XmlDom;

@interface UserProfiles : NSObject
{
    
    NSMutableArray * m_profilesArray;

}

@property (nonatomic, strong) NSMutableArray * m_profilesArray;

- (id)initWithXmlDom:(XmlDom*)xmlDom;
- (void)addProfilesFromXmlDom:(XmlDom*)xmlDom;

@end
