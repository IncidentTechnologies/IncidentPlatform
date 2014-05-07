//
//  UserSong.h
//  gTar
//
//  Created by wuda on 11/11/10.
//  Copyright 2010 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@class XmlDom;
@class CloudController;
@class FileController;

@interface UserSong : NSObject <NSCoding, NSCopying>
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

    XmlDom * m_xmlDom;
    
	NSInteger m_songId;
	NSInteger m_authorId;
    NSInteger m_tempo;
	
	NSString * m_title;
	NSString * m_author;
	NSString * m_genre;
	NSString * m_description;
    NSString * m_licenseInfo;
    
	NSInteger m_xmpFileId;
	NSInteger m_imgFileId;
	
	NSInteger m_playScore;
	NSInteger m_playStars;
	
	NSInteger m_timeCreated;
	NSInteger m_timeModified;

    NSNumber * m_cost;
    NSString * m_productId;
    
    NSInteger m_difficulty;

    NSInteger m_viewCount;
    NSNumber * m_rating;
    NSInteger m_permissions;
    NSInteger m_score;

    UIImage * m_albumArtImage;
    
    BOOL m_userLeased;
    BOOL m_userOwned;

}

@property (nonatomic, strong) XmlDom * m_xmlDom;

@property (nonatomic, assign) NSInteger m_songId;
@property (nonatomic, assign) NSInteger m_authorId;
@property (nonatomic, assign) NSInteger m_tempo;

@property (nonatomic, strong) NSString * m_title;
@property (nonatomic, strong) NSString * m_author;
@property (nonatomic, strong) NSString * m_genre;
@property (nonatomic, strong) NSString * m_description;
@property (nonatomic, strong) NSString * m_licenseInfo;

@property (nonatomic, assign) NSInteger m_xmpFileId;
@property (nonatomic, assign) NSInteger m_imgFileId;

@property (nonatomic, assign) NSInteger m_playScore;
@property (nonatomic, assign) NSInteger m_playStars;

@property (nonatomic, assign) NSInteger m_timeCreated;
@property (nonatomic, assign) NSInteger m_timeModified;

@property (nonatomic, strong) NSNumber * m_cost;
@property (nonatomic, strong) NSString * m_productId;

@property (nonatomic, assign) NSInteger m_difficulty;

@property (nonatomic, assign) NSInteger m_viewCount;
@property (nonatomic, strong) NSNumber * m_rating;
@property (nonatomic, assign) NSInteger m_permissions;
@property (nonatomic, assign) NSInteger m_score;

@property (nonatomic, strong) UIImage * m_albumArtImage;

@property (nonatomic, assign) BOOL m_userLeased;
@property (nonatomic, assign) BOOL m_userOwned;

- (id)initWithXmlDom:(XmlDom*)xmlDom;

- (NSComparisonResult)compareCreated:(id)anObject;
- (NSComparisonResult)comparePlayScore:(id)anObject;
- (NSComparisonResult)compareDifficulty:(id)anObject;

@end
