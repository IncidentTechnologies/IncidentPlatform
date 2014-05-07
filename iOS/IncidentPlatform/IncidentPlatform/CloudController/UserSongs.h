//
//  UserSongs.h
//  gTar
//
//  Created by wuda on 11/11/10.
//  Copyright 2010 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@class UserSong;
@class XmlDom;

@interface UserSongs : NSObject <NSXMLParserDelegate>
{

	NSString * m_songsXml;
	
	NSMutableArray * m_songsArray;
    
	// Parsing variables
	UserSong * m_currentSong;
	
	NSMutableString * m_accumulatedText; 

}

@property (nonatomic, strong) NSString * m_songsXml;
@property (nonatomic, readonly) NSMutableArray * m_songsArray;

- (id)initWithXmlBlob:(NSString*)xmlBlob;
- (id)initWithXml:(NSDictionary*)xmlDictionary;
- (id)initWithXmlDom:(XmlDom*)xmlDom;

- (void)addSongsFromXmlDom:(XmlDom*)xmlDom;

- (NSString*)textFromNode:(NSDictionary*)node;
- (NSInteger)integerFromNode:(NSDictionary*)node;
- (NSNumber*)numberFromNode:(NSDictionary*)node;

- (void)addSongsFromNode:(NSDictionary*)node;

- (UserSong*)getSongWithSongId:(NSInteger)songId;


- (void)parseXml;

@end
