//
//  UserProfile.m
//  gTarAppCore
//
//  Created by Marty Greenia on 4/11/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "UserProfile.h"

#import "XmlDictionary.h"
#import "XmlDom.h"
#import "CloudController.h"

@implementation UserProfile

@synthesize m_profileId;
@synthesize m_userId;
@synthesize m_firstName;
@synthesize m_middleName;
@synthesize m_lastName;
@synthesize m_name;
@synthesize m_email;
@synthesize m_profileText;
@synthesize m_imgFileId;

//@synthesize m_profilePic;

- (id)init
{

    self = [super init];
    
	if ( self )
	{
		/*
		m_userId = -1;
		m_firstName = @"gTar";
		m_middleName = @"Guitar";
		m_lastName = @"User";
		m_email = @"admin@strumhub.com";
		m_profileText = @"I love playing on my gTar!";
		 */
	}
	
	return self;
	
}

- (id)initWithXmlDom:(XmlDom*)xmlDom
{
    
    if ( xmlDom == nil )
    {
        return nil;
    }
    
    self = [super init];
    
    if ( self )
    {
        
//        <id>18</id>
//        <user_id>26</user_id>
//        <name>FancyProfile</name>
//        <firstname>Dummy</firstname>
//        <middlename>Dummy</middlename>
//        <lastname>Dummy</lastname>
//        <email>whaaa@incidenttech.com</email>
//        <profiletext>derp derpderpderp</profiletext>
//        <img_file_id>329</img_file_id>
//        <created>2011-09-19 00:56:16</created>
//        <modified>2011-09-19 02:20:55</modified>

        self.m_profileId = [xmlDom getIntegerFromChildWithName:@"id"];
        self.m_userId = [xmlDom getIntegerFromChildWithName:@"user_id"];
        
        self.m_firstName = [xmlDom getTextFromChildWithName:@"firstname"]; 
        self.m_middleName = [xmlDom getTextFromChildWithName:@"middlename"]; 
        self.m_lastName = [xmlDom getTextFromChildWithName:@"lastname"]; 
        self.m_name = [xmlDom getTextFromChildWithName:@"name"]; 
        self.m_email = [xmlDom getTextFromChildWithName:@"email"]; 
        self.m_profileText = [xmlDom getTextFromChildWithName:@"profiletext"]; 

        self.m_imgFileId = [xmlDom getIntegerFromChildWithName:@"img_file_id"];
        
    }
    
    return self;
    
}

- (id)initWithXmlDictionary:(XmlDictionary*)xmlDictionary
{
	
    if ( xmlDictionary == nil )
    {
        return nil;
    }
    
    self = [super init];
    
	if ( self )
	{
		
////		NSDictionary * rootDictionary;
//		NSDictionary * nodeDictionary;
//		
//		//rootDictionary = [xmlDictionary.m_xmlDictionary objectForKey:@"userinfo"];		
//		//nodeDictionary = [rootDictionary objectForKey:@"id"];
//
//        NSString * numberStr;
//        
//		nodeDictionary = [xmlDictionary.m_xmlDictionary objectForKey:@"id"];
//		numberStr = [nodeDictionary objectForKey:@"value"];
//		self.m_userId = [numberStr integerValue];
//
//		nodeDictionary = [xmlDictionary.m_xmlDictionary objectForKey:@"firstname"];
//		self.m_firstName = [nodeDictionary objectForKey:@"value"];
//		
//		nodeDictionary = [xmlDictionary.m_xmlDictionary objectForKey:@"middlename"];
//		self.m_middleName = [nodeDictionary objectForKey:@"value"];
//
//		nodeDictionary = [xmlDictionary.m_xmlDictionary objectForKey:@"lastname"];
//		self.m_lastName = [nodeDictionary objectForKey:@"value"];
//
//		nodeDictionary = [xmlDictionary.m_xmlDictionary objectForKey:@"email"];
//		self.m_email = [nodeDictionary objectForKey:@"value"];
//
//		nodeDictionary = [xmlDictionary.m_xmlDictionary objectForKey:@"profiletext"];
//		self.m_profileText = [nodeDictionary objectForKey:XML_DICTIONARY_TEXT_NODE];
//		
//		nodeDictionary = [xmlDictionary.m_xmlDictionary objectForKey:@"profilepicpath"];
//		self.m_profilePicPath = [nodeDictionary objectForKey:@"value"];

	}
	
	return self;
	
}

#pragma mark -
#pragma mark NSCoding 

- (void)encodeWithCoder:(NSCoder *)coder
{
	
	[coder encodeInteger:m_userId forKey:@"UserId"];
	[coder encodeObject:m_firstName forKey:@"FirstName"];
	[coder encodeObject:m_middleName forKey:@"MiddleName"];
	[coder encodeObject:m_lastName forKey:@"LastName"];
	[coder encodeObject:m_name forKey:@"Name"];
	[coder encodeObject:m_email forKey:@"Email"];
	[coder encodeObject:m_profileText forKey:@"ProfileText"];
	[coder encodeInteger:m_imgFileId forKey:@"ImageFileId"];
	
//	NSData * imageData = UIImagePNGRepresentation( m_profilePic );
//	[coder encodeObject:imageData forKey:@"ProfilePic"];
	
}

// Decode an object from an archive
- (id)initWithCoder:(NSCoder *)coder
{
    
    self = [super init];
    
	if ( self )
	{
		
		self.m_userId = [coder decodeIntegerForKey:@"UserId"];
		self.m_firstName = [coder decodeObjectForKey:@"FirstName"];
		self.m_middleName = [coder decodeObjectForKey:@"MiddleName"];
		self.m_lastName = [coder decodeObjectForKey:@"LastName"];
        self.m_name = [coder decodeObjectForKey:@"Name"];
		self.m_email = [coder decodeObjectForKey:@"Email"];
		self.m_profileText = [coder decodeObjectForKey:@"ProfileText"];
		self.m_imgFileId = [coder decodeIntegerForKey:@"ImageFileId"];
		
//		NSData * imageData = [coder decodeObjectForKey:@"ProfilePic"];
		
//		if( imageData != nil )
//		{
//			self.m_profilePic = [UIImage imageWithData:imageData];
//		}
		
	}
	
	return self;
	
}



- (BOOL)isEqual:(id)anObject
{
    
    if ( [anObject isKindOfClass:[self class]] == NO )
    {
        return NO;
    }
    
    UserProfile * userProfile = (UserProfile*)anObject;
    
    return ( userProfile.m_userId == self.m_userId );
    
}

- (void)setM_imgFileId:(NSInteger)imgFileId
{
    // 0 is an invalid file, but 1 is the server's "default image" picture
    if ( imgFileId == 0 )
    {
        imgFileId = 1;
    }
    
    m_imgFileId = imgFileId;
}

@end
