//
//  XmlDom.h
//  gTarAppCore
//
//  Created by Marty Greenia on 7/13/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@class XmlDictionary;

@interface XmlDom : NSObject <NSCoding>
{

    NSDictionary * m_backingDictionary;
    
}

- (id)initWithXmlString:(NSString*)xmlString;
- (id)initWithXmlData:(NSData*)xmlData;
- (id)initWithXmlDictionary:(NSDictionary*)xmlDictionary;

- (XmlDom*)getChildWithName:(NSString*)childName;
- (NSArray*)getChildArrayWithName:(NSString*)childName;

- (NSString*)getTextFromChildWithName:(NSString*)childName;
- (NSInteger)getIntegerFromChildWithName:(NSString*)childName;
- (NSNumber*)getNumberFromChildWithName:(NSString*)childName;
- (NSInteger)getDateFromChildWithName:(NSString*)childName;
- (NSString *)getValueFromChildWithName:(id)childName;
- (NSString *)getAttribute:(NSString *)attribute fromChildWithName:(id)childName;
- (NSArray*)getChildrenFromChildWithName:(NSString*)childName;

- (NSString *)getAttribute:(NSString *)attribute;
- (NSString*)getValue;
- (NSString*)getText;
- (NSInteger)getInteger;
- (NSNumber*)getNumber;
- (NSInteger)getDate;
- (NSArray*)getChildren;

@end
