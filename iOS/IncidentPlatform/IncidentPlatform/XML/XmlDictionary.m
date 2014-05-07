//
//  XmlDictionary.m
//  gTarAppCore
//
//  Created by Marty Greenia on 4/13/11.
//  Copyright 2011 IncidentTech. All rights reserved.
//

#import "XmlDictionary.h"


@implementation XmlDictionary

@synthesize m_xmlDictionary;

- (id)initWithXmlBlobData:(NSData *)xmlBlobData
{
    
    self = [super init];
    
	if ( self )
	{
		
		[self parseXmlBlobData:xmlBlobData];
		
	}
	
	return self;
	
}

+ (NSDictionary*)dictionaryFromXmlBlob:(NSString*)xmlBlob
{

	XmlDictionary * xmlDictionary = [[XmlDictionary alloc] initWithXmlBlobData:[xmlBlob dataUsingEncoding:NSASCIIStringEncoding]];
	
	NSDictionary * dictionary = xmlDictionary.m_xmlDictionary;
	
    
	return dictionary;
}

+ (NSDictionary*)dictionaryFromXmlBlobData:(NSData*)xmlBlobData
{

	XmlDictionary * xmlDictionary = [[XmlDictionary alloc] initWithXmlBlobData:xmlBlobData];
	
	NSDictionary * dictionary = xmlDictionary.m_xmlDictionary;
	
    
	return dictionary;	
}

- (void)parseXmlBlobData:(NSData*)xmlBlobData
{

	// hold onto the xml blob we are parsing
	m_xmlBlobData = xmlBlobData;
	
	// allocate some useful objects
	m_dictionaryArray = [[NSMutableArray alloc] init];
	m_accumulatedText = [[NSMutableString alloc] init];
	
	// start the parsing
	NSXMLParser * parser = [[NSXMLParser alloc] initWithData:m_xmlBlobData];
	
	parser.delegate = self;
	
	[parser parse];
    
	
	// do something with the result?
	
}

#pragma mark NSXMLParser delegate

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName
  namespaceURI:(NSString *)namespaceURI
 qualifiedName:(NSString *)qName
	attributes:(NSDictionary *)attributeDict
{
	
	// the parent of the current level is just the last dict in the array
	NSMutableDictionary * parentDictionary = [m_dictionaryArray lastObject];
	
	NSMutableDictionary * childDictionary = [attributeDict mutableCopy];
    
	id existingChild = [parentDictionary objectForKey:elementName];
	
	// multiple child of the same name -> turn into an array
	if ( existingChild != nil )
	{
		
		// Child array already exists 
		if ( [existingChild isKindOfClass:[NSMutableArray class]] == YES )
		{
			
			NSMutableArray * childArray = (NSMutableArray*) existingChild;
			
			[childArray addObject:childDictionary];
            
		}
		else
		{
			
			// Otherwise make a new child array.
			NSMutableArray * childArray = [[NSMutableArray alloc] initWithObjects:(id)existingChild, nil];
			
			[childArray addObject:childDictionary];
			
			[parentDictionary setObject:childArray forKey:elementName];
            
			
		}
		
	}
	else 
	{
		[parentDictionary setObject:childDictionary forKey:elementName];
	}
    
	// This node is done. make the current child the new parent node.
	[m_dictionaryArray addObject:childDictionary];
    
    
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName
  namespaceURI:(NSString *)namespaceURI
 qualifiedName:(NSString *)qName
{
	
	NSMutableDictionary * currentDictionary = [m_dictionaryArray lastObject];
	
	// save any text that was accumulated during this element
	if ( [m_accumulatedText length] > 0 )
	{
        
        @autoreleasepool {
        
        // strip and whitespace
        NSString * stripString = [m_accumulatedText stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        
        if ( stripString != nil && [stripString isEqualToString:@""] == NO )
        {
            [currentDictionary setObject:stripString forKey:XML_DICTIONARY_TEXT_NODE];
		}
        
        
		m_accumulatedText = [[NSMutableString alloc] init];
        
        }
        
	}
	
    // Hold onto this dictionary untill we can confirm we are done with it
    
	// Now that we are done with this node, we can pop it off the stack.
	[m_dictionaryArray removeObject:currentDictionary];
	
    // Once we reached the end (e.g. just poped off the </xml> node)
    // then we are done with this dictionary
	if ( [m_dictionaryArray count] == 0 )
	{
		m_xmlDictionary = currentDictionary;
	}
    
    
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
	
	[m_accumulatedText appendString:string];
	
}

- (void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError
{

	NSString * errorString = [parseError localizedDescription];

	NSLog(@"%@", errorString);
	
}


@end
