//
//  User.h
//  gTarAppCore
//
//  Created by Marty Greenia on 4/11/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@class XmlDom;

@interface User : NSObject <NSCoding>
{
	NSString * m_username;
	NSInteger m_userId;
}

@property (nonatomic, strong) NSString * m_username;
@property (nonatomic, assign) NSInteger m_userId;

- (id)initWithXmlDom:(XmlDom*)xmlDom;

@end
