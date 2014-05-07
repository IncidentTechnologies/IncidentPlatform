//
//  UserSongSessions.h
//  gTarAppCore
//
//  Created by Marty Greenia on 9/8/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@class XmlDom;

@interface UserSongSessions : NSObject
{

    NSMutableArray * m_sessionsArray;

}

@property (nonatomic, strong) NSArray * m_sessionsArray;

- (id)initWithXmlDom:(XmlDom*)xmlDom;
- (void)addSessionsFromXmlDom:(XmlDom*)xmlDom;

@end
