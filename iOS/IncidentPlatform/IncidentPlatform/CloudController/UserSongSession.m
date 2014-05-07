//
//  UserSongSession.m
//  gTarAppCore
//
//  Created by Marty Greenia on 3/21/11.
//  Copyright 2011 Msft. All rights reserved.
//

#import "UserSongSession.h"
#import "XmlDom.h"
#import "UserSong.h"
#import "User.h"
#import "UserProfile.h"

@implementation UserSongSession

@synthesize m_user;
@synthesize m_userProfile;
@synthesize m_userSong;

@synthesize m_sessionId;
@synthesize m_songId;
@synthesize m_userId;
@synthesize m_score;
@synthesize m_scoreMax;
@synthesize m_stars;
@synthesize m_combo;
@synthesize m_xmpFileId;
@synthesize m_created;
@synthesize m_length;
@synthesize m_appId;

@synthesize m_notes;
@synthesize m_xmpBlob;

- (id)initWithXmlDom:(XmlDom*)xmlDom
{
    
    if ( xmlDom == nil )
    {
        return nil;
    }
    
    self = [super init];
    
    if ( self )
    {

//    <UserSongSession>
//    <id>26</id>
//    <user_id>26</user_id>
//    <user_song_id>6</user_song_id>
//    <score>49</score>
//    <maxscore>200</maxscore>
//    <topstreak>1</topstreak>
//    <xmp_file_id>0</xmp_file_id>
//    <notes>gTar Play</notes>
//    <created>2011-07-25 17:43:59</created>
//    </UserSongSession>
        
        self.m_sessionId = [xmlDom getIntegerFromChildWithName:@"id"];
        self.m_userId = [xmlDom getIntegerFromChildWithName:@"user_id"];
		self.m_songId = [xmlDom getIntegerFromChildWithName:@"user_song_id"];
		self.m_score = [xmlDom getIntegerFromChildWithName:@"score"];
		self.m_scoreMax = [xmlDom getIntegerFromChildWithName:@"maxscore"];
        self.m_combo = [xmlDom getIntegerFromChildWithName:@"topstreak"];
		self.m_xmpFileId = [xmlDom getIntegerFromChildWithName:@"xmp_file_id"];
        self.m_created = [xmlDom getDateFromChildWithName:@"created"];
		self.m_notes = [xmlDom getTextFromChildWithName:@"notes"];
        
        // also add the user song if there
        XmlDom * userSongDom = [xmlDom getChildWithName:@"UserSong"];
        
        // if the songId is zero, that also indicates an invalid song
        if ( m_songId != 0 && userSongDom != nil )
        {
            m_userSong = [[UserSong alloc] initWithXmlDom:userSongDom];
        }
        else
        {
            m_userSong = [[UserSong alloc] init];
        }
        
        XmlDom * userDom = [xmlDom getChildWithName:@"User"];
        
        if ( userDom != nil )
        {
            m_user = [[User alloc] initWithXmlDom:userDom];
        }
        
        XmlDom * userProfileDom = [xmlDom getChildWithName:@"UserProfile"];
        
        if ( userProfileDom != nil )
        {
            m_userProfile = [[UserProfile alloc] initWithXmlDom:userProfileDom];
        }

    }
    
    return self;
    
}

// Encode an object for an archive
- (void)encodeWithCoder:(NSCoder *)coder
{
    
    [coder encodeInteger:m_sessionId forKey:@"SessionId"];
    [coder encodeInteger:m_songId forKey:@"SongId"];
    [coder encodeInteger:m_userId forKey:@"UserId"];
    [coder encodeInteger:m_score forKey:@"Score"];
    [coder encodeInteger:m_scoreMax forKey:@"ScoreMax"];
    [coder encodeInteger:m_stars forKey:@"Stars"];
    [coder encodeInteger:m_combo forKey:@"Combo"];
    [coder encodeInteger:m_xmpFileId forKey:@"XmpFileId"];
    [coder encodeInteger:m_created forKey:@"Created"];
    [coder encodeInteger:m_length forKey:@"Length"];

    [coder encodeObject:m_notes	forKey:@"Notes"];
    [coder encodeObject:m_xmpBlob forKey:@"XmpBlob"];

    [coder encodeObject:m_user forKey:@"User"];
    [coder encodeObject:m_userProfile forKey:@"UserProfile"];
    [coder encodeObject:m_userSong forKey:@"UserSong"];

}

// Decode an object from an archive
- (id)initWithCoder:(NSCoder *)coder
{
	
    self = [super init];
    
	if ( self )
	{
        
		m_sessionId = [coder decodeIntegerForKey:@"SessionId"];
		m_songId = [coder decodeIntegerForKey:@"SongId"];
		m_userId = [coder decodeIntegerForKey:@"UserId"];
		m_score = [coder decodeIntegerForKey:@"Score"];
		m_scoreMax = [coder decodeIntegerForKey:@"ScoreMax"];
		m_stars = [coder decodeIntegerForKey:@"Stars"];
		m_combo = [coder decodeIntegerForKey:@"Combo"];
		m_xmpFileId = [coder decodeIntegerForKey:@"XmpFileId"];
        m_created = [coder decodeIntegerForKey:@"Created"];
        m_length = [coder decodeIntForKey:@"Length"];
		
		self.m_notes = [coder decodeObjectForKey:@"Notes"];
		self.m_xmpBlob = [coder decodeObjectForKey:@"XmpBlob"];
		
		self.m_user = [coder decodeObjectForKey:@"User"];
		self.m_userProfile = [coder decodeObjectForKey:@"UserProfile"];
		self.m_userSong = [coder decodeObjectForKey:@"UserSong"];
        
	}
	
	return self;
	
}


- (NSComparisonResult)compareCreated:(id)anObject
{
    
    UserSongSession * session = (UserSongSession*)anObject;
    
    if ( session.m_created > self.m_created )
    {
        return NSOrderedAscending;
    }
    
    if ( session.m_created < self.m_created )
    {
        return NSOrderedDescending;
    }
    
    return NSOrderedSame;
    
}

- (NSComparisonResult)compareCreatedNewestFirst:(id)anObject
{
    
    UserSongSession * session = (UserSongSession*)anObject;
    
    if ( session.m_created > self.m_created )
    {
        return NSOrderedDescending;
    }
    
    if ( session.m_created < self.m_created )
    {
        return NSOrderedAscending;
    }
    
    return NSOrderedSame;
    
}

@end
