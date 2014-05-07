    //
//  UserSong.m
//  gTar
//
//  Created by wuda on 11/11/10.
//  Copyright 2010 IncidentTech. All rights reserved.
//

#import "UserSong.h"

#import "CloudController.h"
#import "XmlDom.h"
#import "FileController.h"

@implementation UserSong

@synthesize m_xmlDom;
@synthesize m_songId, m_authorId, m_tempo, m_title, m_author, m_genre, m_description, m_licenseInfo, m_xmpFileId, m_timeCreated, m_timeModified;
@synthesize m_imgFileId;
@synthesize m_playScore, m_playStars;
@synthesize m_cost;
@synthesize m_productId;
@synthesize m_difficulty;
@synthesize m_viewCount, m_rating, m_permissions, m_score;
@synthesize m_albumArtImage;
@synthesize m_userLeased, m_userOwned;

- (id)initWithXmlDom:(XmlDom*)xmlDom;
{

    if ( xmlDom == nil )
    {
        return nil;
    }
    
    self = [super init];
    
    if ( self )
    {
//        <id>5</id>
//        <user_id>1</user_id>
//        <viewcount>5</viewcount>
//        <rating>25</rating>
//        <permissions>0</permissions>
//        <score>3</score>
//        <title>Friday Song</title>
//        <author>Richard Gee</author>
//        <tempo>120</tempo>
//        <description>This is a song about friday by Richard Gee</description>
//        <genre>Jazz</genre>
//        <path>user/1/songs/Friday_Song_Richard_Gee_20091218.xmp</path>
//        <img_path>user/2/img/Halloween06_QN5_04.jpg</img_path>
//        <created>2009-12-18 16:55:47</created>
//        <modified>2009-12-18 16:55:47</modified>
//        <cost>0.99</cost>
        
        self.m_xmlDom = xmlDom;
    
        NSDateFormatter * dateFormat = [[NSDateFormatter alloc] init];
        
        [dateFormat setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
        
        
        self.m_songId = [xmlDom getIntegerFromChildWithName:@"id"];
        self.m_authorId = [xmlDom getIntegerFromChildWithName:@"user_id"];
        self.m_viewCount = [xmlDom getIntegerFromChildWithName:@"viewcount"];
        self.m_rating = [xmlDom getNumberFromChildWithName:@"rating"];
        self.m_permissions = [xmlDom getIntegerFromChildWithName:@"permissions"];
        self.m_score = [xmlDom getIntegerFromChildWithName:@"score"];
        self.m_title = [xmlDom getTextFromChildWithName:@"title"];
        self.m_author = [xmlDom getTextFromChildWithName:@"author"];
        self.m_tempo = [xmlDom getIntegerFromChildWithName:@"tempo"];
        self.m_description = [xmlDom getTextFromChildWithName:@"description"];
        self.m_genre = [xmlDom getTextFromChildWithName:@"genre"];
        self.m_licenseInfo = [xmlDom getTextFromChildWithName:@"license_info"];
        self.m_xmpFileId = [xmlDom getIntegerFromChildWithName:@"xmp_file_id"];
        self.m_imgFileId = [xmlDom getIntegerFromChildWithName:@"image_file_id"];
        self.m_timeCreated = [xmlDom getDateFromChildWithName:@"created"];
        self.m_timeModified = [xmlDom getDateFromChildWithName:@"modified"];
        self.m_cost = [xmlDom getNumberFromChildWithName:@"cost"];
        self.m_productId = [xmlDom getTextFromChildWithName:@"product_id"];
        self.m_difficulty = [xmlDom getIntegerFromChildWithName:@"difficulty"];
        
        // Optional song fields
        XmlDom *pTempNode = [xmlDom getChildWithName:@"user_leased"];
        if(pTempNode != NULL)
            self.m_userLeased = [[pTempNode getText] integerValue];
        else
            self.m_userLeased = FALSE;
    
        pTempNode = [xmlDom getChildWithName:@"user_owned"];
        if(pTempNode != NULL)
            self.m_userOwned = [[pTempNode getText] integerValue];
        else
            self.m_userOwned = FALSE;
    }
    
    return self;
    
}

// TODO
// I'm hardcoding the img file id to 1 because we currently don't display album art in the app.
// I don't want to rip out all the code as it is working now, because we will add art in eventually.
// This let's us avoid downloading useless album art in the short term by just short circuiting
// to an image that is (effectively) guaranteed to be in the cache all the time.
// Remove this getter to restore normal operation of album art downloading.
- (NSInteger)m_imgFileId
{
    return 1;
}

// just a container class, doesn't have anything else
// Encode an object for an archive
- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:m_xmlDom forKey:@"XmlDom"];
    
	[coder encodeInteger:m_songId forKey:@"SongId"];
	[coder encodeInteger:m_authorId forKey:@"AuthorId"];
    [coder encodeInteger:m_tempo forKey:@"Tempo"];
	
	[coder encodeObject:m_title forKey:@"Title"];
	[coder encodeObject:m_author forKey:@"Author"];
	[coder encodeObject:m_genre forKey:@"Genre"];
	[coder encodeObject:m_description forKey:@"Description"];
    [coder encodeObject:m_licenseInfo forKey:@"LicenseInfo"];
    
	[coder encodeInteger:m_xmpFileId forKey:@"XmpFileId"];
	[coder encodeInteger:m_imgFileId forKey:@"ImgFileId"];

	[coder encodeInteger:m_playScore forKey:@"PlayScore"];
    [coder encodeInteger:m_playStars forKey:@"PlayStars"];

    [coder encodeInteger:m_timeModified forKey:@"TimeModified"];
    [coder encodeInteger:m_timeCreated forKey:@"TimeCreated"];
    
    [coder encodeObject:m_cost forKey:@"Cost"];
    [coder encodeObject:m_productId forKey:@"ProductId"];
    
    [coder encodeInteger:m_difficulty forKey:@"Difficulty"];
    
    [coder encodeInteger:m_viewCount forKey:@"ViewCount"];
    [coder encodeObject:m_rating forKey:@"Rating"];
    [coder encodeInteger:m_permissions forKey:@"Permissions"];
    [coder encodeInteger:m_score forKey:@"Score"];
    
    [coder encodeBool:m_userOwned forKey:@"UserOwned"];
    [coder encodeBool:m_userLeased forKey:@"UserLeased"];
    
    // convert to data
    NSData * imageData = UIImagePNGRepresentation( m_albumArtImage );
    
    [coder encodeObject:imageData forKey:@"AlbumArtImage"];

}

// Decode an object from an archive
- (id)initWithCoder:(NSCoder *)coder
{
    //self = [super initWithCoder:coder];
	self = [super init];
	
    self.m_xmlDom = [coder decodeObjectForKey:@"XmlDom"];
    
    self.m_songId = [coder decodeIntegerForKey:@"SongId"];
	self.m_authorId = [coder decodeIntegerForKey:@"AuthorId"];
    self.m_tempo = [coder decodeIntegerForKey:@"Tempo"];
	
    self.m_title = [coder decodeObjectForKey:@"Title"];
	self.m_author = [coder decodeObjectForKey:@"Author"];
	self.m_genre = [coder decodeObjectForKey:@"Genre"];
    self.m_description = [coder decodeObjectForKey:@"Description"];
    self.m_licenseInfo = [coder decodeObjectForKey:@"LicenseInfo"];
    
	self.m_xmpFileId = [coder decodeIntegerForKey:@"XmpFileId"];
	self.m_imgFileId = [coder decodeIntegerForKey:@"ImgFileId"]; 
	
	self.m_playScore = [coder decodeIntegerForKey:@"PlayScore"];
	self.m_playStars = [coder decodeIntegerForKey:@"PlayStars"];
	
	self.m_timeModified = [coder decodeIntegerForKey:@"TimeModified"];
	self.m_timeCreated = [coder decodeIntegerForKey:@"TimeCreated"];
    
    self.m_cost = [coder decodeObjectForKey:@"Cost"];
    self.m_productId = [coder decodeObjectForKey:@"ProductId"];
    
    self.m_difficulty = [coder decodeIntegerForKey:@"Difficulty"];
    
    self.m_viewCount = [coder decodeIntegerForKey:@"ViewCount"];
    self.m_rating = [coder decodeObjectForKey:@"Rating"];
    self.m_permissions = [coder decodeIntegerForKey:@"Permissions"];
    self.m_score = [coder decodeIntegerForKey:@"Score"];
    
    self.m_userLeased = [coder decodeBoolForKey:@"UserLeased"];
    self.m_userOwned = [coder decodeBoolForKey:@"UserOwned"];
    
    NSData * imageData = [coder decodeObjectForKey:@"AlbumArtImage"];
    
    if ( imageData )
    {
        self.m_albumArtImage = [UIImage imageWithData:imageData];
    }
    else
    {
        self.m_albumArtImage = nil;
    }
    
	return self;
    
}


- (BOOL)isEqual:(id)anObject
{
    
    if ( [anObject isKindOfClass:[UserSong class]] == NO )
    {
        return false;
    }
    
    UserSong * aUserSong = (UserSong*)anObject;
    
    // really this is the only thing that matters.
    // Do we want to test for different versions?
    // E.g. the same songId but different metadata?j
    
    if ( self.m_songId == aUserSong.m_songId)
    {
        return YES;
    }
    else
    {
        return NO;
    }
    
}

#pragma mark - Misc

- (NSComparisonResult)compareCreated:(id)anObject
{
    
    UserSong * song = (UserSong*)anObject;
    
    if ( song.m_timeCreated > self.m_timeCreated )
    {
        return NSOrderedAscending;
    }
    
    if ( song.m_timeCreated < self.m_timeCreated )
    {
        return NSOrderedDescending;
    }
    
    return NSOrderedSame;
    
}

- (NSComparisonResult)comparePlayScore:(id)anObject
{
    
    UserSong * song = (UserSong*)anObject;
    
    if ( song.m_playScore > self.m_playScore )
    {
        return NSOrderedAscending;
    }
    
    if ( song.m_playScore < self.m_playScore )
    {
        return NSOrderedDescending;
    }
    
    return NSOrderedSame;
    
}

- (NSComparisonResult)compareDifficulty:(id)anObject
{
    
    UserSong * song = (UserSong*)anObject;
    
    if ( song.m_difficulty > self.m_difficulty )
    {
        return NSOrderedAscending;
    }
    
    if ( song.m_difficulty < self.m_difficulty )
    {
        return NSOrderedDescending;
    }
    
    return NSOrderedSame;
    
}

#pragma mark - NSCopying

- (id)copyWithZone:(NSZone *)zone
{

    UserSong * userSong;
    
//    if ( m_xmlDom != nil )
//    {
//        userSong = [[UserSong alloc] initWithXmlDom:m_xmlDom];
//    }
//    else
    {
        
        userSong = [[UserSong alloc] init];
        
        userSong.m_songId = self.m_songId;
        userSong.m_authorId = self.m_authorId;
        userSong.m_viewCount = self.m_viewCount;
        userSong.m_rating = self.m_rating;
        userSong.m_permissions = self.m_permissions;
        userSong.m_score = self.m_score;
        userSong.m_title = self.m_title;
        userSong.m_author = self.m_author;
        userSong.m_tempo = self.m_tempo;
        userSong.m_description = self.m_description;
        userSong.m_genre = self.m_genre;
        userSong.m_xmpFileId = self.m_xmpFileId;
        userSong.m_imgFileId = self.m_imgFileId;
        userSong.m_timeCreated = self.m_timeCreated;
        userSong.m_timeModified = self.m_timeModified;
        userSong.m_cost = self.m_cost;
        userSong.m_productId = self.m_productId;
        userSong.m_difficulty = self.m_difficulty;
        userSong.m_userOwned = self.m_userOwned;
        userSong.m_userLeased = self.m_userLeased;

    }
    
    return userSong;
    
}

@end
