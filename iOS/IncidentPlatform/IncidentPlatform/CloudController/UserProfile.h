//
//  UserProfile.h
//  gTarAppCore
//
//  Created by Marty Greenia on 4/11/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import <UIKit/UIKit.h>

@class XmlDictionary;
@class XmlDom;
@class CloudController;

@interface UserProfile : NSObject <NSCoding>
{

    NSInteger m_profileId;
	NSInteger m_userId;
	NSString * m_firstName;
	NSString * m_middleName;
	NSString * m_lastName;
	NSString * m_name;
	NSString * m_email;
	NSString * m_profileText;
    NSInteger m_imgFileId;
	
	// cache the profile image so we don't need to keep hitting the server
    // This is done by the FileController now
//	UIImage * m_profilePic;
	
}

@property (nonatomic, assign) NSInteger m_profileId;
@property (nonatomic, assign) NSInteger m_userId;
@property (nonatomic, strong) NSString * m_firstName;
@property (nonatomic, strong) NSString * m_middleName;
@property (nonatomic, strong) NSString * m_lastName;
@property (nonatomic, strong) NSString * m_name;
@property (nonatomic, strong) NSString * m_email;
@property (nonatomic, strong) NSString * m_profileText;
@property (nonatomic, assign) NSInteger m_imgFileId;

//@property (nonatomic, retain) UIImage * m_profilePic;

- (id)initWithXmlDictionary:(XmlDictionary*)xmlDictionary;
- (id)initWithXmlDom:(XmlDom*)xmlDom;

@end
