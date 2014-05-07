//
//  UserSongs.m
//  gTar
//
//  Created by wuda on 11/11/10.
//  Copyright 2010 IncidentTech. All rights reserved.
//

#import "UserSong.h"
#import "UserSongs.h"
#import "XmlDictionary.h"
#import "XmlDom.h"

@implementation UserSongs

@synthesize m_songsXml, m_songsArray;

- (id)init
{
    
    self = [super init];
    
    if ( self )
    {
        m_songsArray = [[NSMutableArray alloc] init];
    }
    
    return self;
    
}

- (id)initWithXmlBlob:(NSString *)xmlBlob
{
    
    if ( xmlBlob == nil )
    {
        return nil;
    }
	
    self = [super init];
    
	if ( self )
	{
		m_songsXml = xmlBlob;
		
		
		[self parseXml];
	}
		
	return self;
	
}

- (id)initWithXml:(NSDictionary*)xmlDictionary
{
    
    if ( xmlDictionary == nil )
    {
        return nil;
    }

    self = [super init];
    
    if ( self )
    {
        
        m_songsArray = [[NSMutableArray alloc] init];

        // Pull out what we want from the XML DOM
		NSDictionary * userCreateSongsNode = [xmlDictionary objectForKey:@"UserCreatedSongsList"];

        if ( userCreateSongsNode != nil )
        {
            [self addSongsFromNode:userCreateSongsNode];
        
        }
        
        // Pull out what we want from the XML DOM
		NSDictionary * userLeasedSongsNode = [xmlDictionary objectForKey:@"UserLeasedSongsList"];
        
        if ( userLeasedSongsNode != nil )
        {
            [self addSongsFromNode:userLeasedSongsNode];
            
        }
        
        
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
        
        m_songsArray = [[NSMutableArray alloc] init];
        
        [self addSongsFromXmlDom:xmlDom];
        
        /*
        // user created songs
        NSArray * userCreatedSongs = [xmlDom getChildrenFromChildWithName:@"UserCreatedSongs"];

        for ( XmlDom * dom in userCreatedSongs )
        {
            UserSong * userSong = [[UserSong alloc] initWithXmlDom:dom];
            
            [m_songsArray addObject:userSong];
            
            [userSong release];
        }
        
        // user leased songs
        NSArray * userLeasedSongs = [xmlDom getChildrenFromChildWithName:@"UserLeasedSongs"];

        for ( XmlDom * dom in userLeasedSongs )
        {
            UserSong * userSong = [[UserSong alloc] initWithXmlDom:dom];
            
            [m_songsArray addObject:userSong];
            
            [userSong release];
        }
*/
    }
    
    return self;
    
}


- (void)addSongsFromXmlDom:(XmlDom*)xmlDom
{
    
    NSArray * userSongsDom = [xmlDom getChildArrayWithName:@"UserSongs"];
    
    for ( XmlDom * userSongDom in userSongsDom )
    {
        UserSong * userSong = [[UserSong alloc] initWithXmlDom:userSongDom];
        
        [m_songsArray addObject:userSong];
        
    }
    
}

- (NSString*)textFromNode:(NSDictionary*)node
{
    return [node objectForKey:XML_DICTIONARY_TEXT_NODE];
}

- (NSInteger)integerFromNode:(NSDictionary*)node
{
    NSString * numString = [self textFromNode:node];
    return [numString integerValue];
}

- (NSNumber*)numberFromNode:(NSDictionary*)node
{
    NSString * numString = [self textFromNode:node];
    return [NSNumber numberWithFloat:[numString floatValue]];
}

- (void)addSongsFromNode:(NSDictionary*)node
{
    
    for ( unsigned int item = 0; YES; item++ )
    {
        
        NSString * keyString = [NSString stringWithFormat:@"item_%u", item];
        
        NSDictionary * itemNode = [node objectForKey:keyString];
        
        if ( itemNode == nil ) 
        {
            break;
        }
        
        NSDictionary * userSongNode = [itemNode objectForKey:@"UserSong"];
        
        if ( userSongNode == nil )
        {
            break;
        }
        
        // add song
        UserSong * userSong = [[UserSong alloc] init];
        
        userSong.m_songId = [self integerFromNode:[userSongNode objectForKey:@"id"]];
        userSong.m_authorId = [self integerFromNode:[userSongNode objectForKey:@"user_id"]];
        
        userSong.m_tempo = [self integerFromNode:[userSongNode objectForKey:@"tempo"]];
        
        userSong.m_title = [self textFromNode:[userSongNode objectForKey:@"title"]];
        userSong.m_author = [self textFromNode:[userSongNode objectForKey:@"author"]];
        userSong.m_genre = [self textFromNode:[userSongNode objectForKey:@"genre"]];
        userSong.m_description = [self textFromNode:[userSongNode objectForKey:@"description"]];
//        userSong.m_xmpFileId = [self textFromNode:[userSongNode objectForKey:@"path"]];
//        userSong.m_imgFileId = [self textFromNode:[userSongNode objectForKey:@"img_path"]];
        
        userSong.m_timeCreated = [self integerFromNode:[userSongNode objectForKey:@"created"]];
        userSong.m_timeModified = [self integerFromNode:[userSongNode objectForKey:@"modified"]];
        
        userSong.m_cost = [self numberFromNode:[userSongNode objectForKey:@"cost"]];

        
        //                NSInteger m_score;
        //                NSInteger m_stars;
        
        [m_songsArray addObject:userSong];
        

    }
    
    
}

- (UserSong*)getSongWithSongId:(NSInteger)songId
{
	
	for ( unsigned int songIndex = 0; songIndex < [m_songsArray count]; songIndex++ )
	{
		UserSong * userSong = [m_songsArray objectAtIndex:songIndex];
		
		if ( userSong.m_songId == songId )
		{
			return userSong;
		}
	}
	
	return nil;
	
}

#pragma mark -
#pragma mark XMP parsing and NSXMLParser delegate

- (void)parseXml
{

	NSData * rawXmp = [m_songsXml dataUsingEncoding:NSASCIIStringEncoding];
	
	// Init and kick off the parser
	NSXMLParser * parser = [[NSXMLParser alloc] initWithData:rawXmp];
	
	[parser setDelegate:self];
	
	[parser parse];
	
}

//
// NSXML delegate functions
//
- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName
  namespaceURI:(NSString *)namespaceURI
 qualifiedName:(NSString *)qName
	attributes:(NSDictionary *)attributeDict
{
	
	if ( [elementName isEqualToString:@"usersongs"] )
	{
		
		if ( m_songsArray != nil )
		{
			
			m_songsArray = nil;
		}
		
		m_songsArray = [[NSMutableArray alloc] init];
				
	}
	else if ( [elementName isEqualToString:@"song"] )
	{
		
		m_currentSong = [[UserSong alloc] init];
		
	}
	else if ( [elementName isEqualToString:@"id"] )
	{
		
		NSInteger authorId = [[attributeDict objectForKey:@"value"] intValue];
							
		m_currentSong.m_songId = authorId;
		
	}
	else if ( [elementName isEqualToString:@"author_id"] )
	{
		
		NSInteger songId = [[attributeDict objectForKey:@"value"] intValue];
		
		m_currentSong.m_songId = songId;
		
	}
	else if ( [elementName isEqualToString:@"title"] )
	{
		
		m_accumulatedText = [[NSMutableString alloc] init];
		
	}
	else if ( [elementName isEqualToString:@"author"] )
	{
		
		m_accumulatedText = [[NSMutableString alloc] init];
		
	}
	else if ( [elementName isEqualToString:@"genre"] )
	{
		
		m_accumulatedText = [[NSMutableString alloc] init];
		
	}
	else if ( [elementName isEqualToString:@"description"] )
	{
		
		m_accumulatedText = [[NSMutableString alloc] init];
		
	}
	else if ( [elementName isEqualToString:@"path"] )
	{
		
		m_accumulatedText = [[NSMutableString alloc] init];
		
	}
	else if ( [elementName isEqualToString:@"timecreated"] )
	{

		m_accumulatedText = [[NSMutableString alloc] init];

	}
	else if ( [elementName isEqualToString:@"timemodified"] )
	{

		m_accumulatedText = [[NSMutableString alloc] init];

	}
	else if ( [elementName isEqualToString:@"created"] )
	{

		m_accumulatedText = [[NSMutableString alloc] init];

	}
	else if ( [elementName isEqualToString:@"modified"] )
	{
		
		m_accumulatedText = [[NSMutableString alloc] init];

	}
	else if ( [elementName isEqualToString:@"cost"] )
	{

		NSNumber * cost = [NSNumber numberWithFloat:[[attributeDict objectForKey:@"value"] floatValue]];
        
		m_currentSong.m_cost = cost;
		
	}
	
	
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName
  namespaceURI:(NSString *)namespaceURI
 qualifiedName:(NSString *)qName 
{
	
	if ( [elementName isEqualToString:@"usersongs"] )
	{

	}
	else if ( [elementName isEqualToString:@"song"] )
	{
		
		[m_songsArray addObject:m_currentSong];
		
		
		m_currentSong = nil;
		
	}
	else if ( [elementName isEqualToString:@"title"] )
	{
		
		m_currentSong.m_title = m_accumulatedText;
		
		
		m_accumulatedText = nil;
		
	}
	else if ( [elementName isEqualToString:@"author"] )
	{
		
		m_currentSong.m_author = m_accumulatedText;
		
		
		m_accumulatedText = nil;
		
	}
	else if ( [elementName isEqualToString:@"genre"] )
	{
		
		m_currentSong.m_genre = m_accumulatedText;
		
		
		m_accumulatedText = nil;
		
	}
	else if ( [elementName isEqualToString:@"description"] )
	{
		
		m_currentSong.m_description = m_accumulatedText;
		
		
		m_accumulatedText = nil;
		
	}
//	else if ( [elementName isEqualToString:@"path"] )
//	{
//		
//		m_currentSong.m = m_accumulatedText;
//	
//		[m_accumulatedText release];
//		
//		m_accumulatedText = nil;
//		
//	}
//	else if ( [elementName isEqualToString:@"img_path"] )
//	{
//		
//		m_currentSong.m_imgUrlPath = m_accumulatedText;
//		
//		[m_accumulatedText release];
//		
//		m_accumulatedText = nil;
//		
//	}
	else if ( [elementName isEqualToString:@"timecreated"] )
	{
		
		m_currentSong.m_timeCreated = [m_accumulatedText integerValue];
		
		
		m_accumulatedText = nil;
		
		
	}
	else if ( [elementName isEqualToString:@"timemodified"] )
	{
		
		m_currentSong.m_timeModified = [m_accumulatedText integerValue];
		
		
		m_accumulatedText = nil;
		
	}
	else if ( [elementName isEqualToString:@"created"] )
	{
		
		m_currentSong.m_timeCreated = [m_accumulatedText integerValue];
		
		
		m_accumulatedText = nil;
		
		
	}
	else if ( [elementName isEqualToString:@"modified"] )
	{
		
		m_currentSong.m_timeModified = [m_accumulatedText integerValue];
		
		
		m_accumulatedText = nil;
		
	}
	
}

// This method is called by the parser when it find parsed character data ("PCDATA") in an element.
// The parser is not guaranteed to deliver all of the parsed character data for an element in a single
// invocation, so it is necessary to accumulate character data until the end of the element is reached.
//
- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
	
	[m_accumulatedText appendString:string];
	
}

// Parse error
- (void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError
{
	/*
	 if ([parseError code] != NSXMLParserDelegateAbortedParseError && !didAbortParsing)
	 {
	 [self performSelectorOnMainThread:@selector(handleEarthquakesError:)
	 withObject:parseError
	 waitUntilDone:NO];
	 }
	 */
}


@end
