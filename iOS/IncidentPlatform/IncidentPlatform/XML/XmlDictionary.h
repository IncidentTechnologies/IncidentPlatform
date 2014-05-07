//
//  XmlDictionary.h
//  gTarAppCore
//
//  Created by Marty Greenia on 4/13/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>

#define XML_DICTIONARY_TEXT_NODE @"__TEXT_NODE__"

@interface XmlDictionary : NSObject <NSXMLParserDelegate>
{

	NSData * m_xmlBlobData;

	NSDictionary * m_xmlDictionary;
	
	NSMutableArray * m_dictionaryArray;
	NSMutableString * m_accumulatedText;
	
}

@property (nonatomic, readonly) NSDictionary * m_xmlDictionary;

- (id)initWithXmlBlobData:(NSData*)xmlBlobData;

+ (NSDictionary*)dictionaryFromXmlBlobData:(NSData*)xmlBlobData;
+ (NSDictionary*)dictionaryFromXmlBlob:(NSString*)xmlBlob;

- (void)parseXmlBlobData:(NSData*)xmlBlobData;


@end
