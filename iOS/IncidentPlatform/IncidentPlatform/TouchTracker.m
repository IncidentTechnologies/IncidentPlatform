//
//  TouchTracker.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "TouchTracker.h"

@implementation TouchTracker

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        [self awakeFromNib];
    }
    return self;
}

-(void)awakeFromNib {
    [self setCircleThickness:3.0f];
    [self setLineThickness:1.0f];
    [self setCircleRadius:10.0f];
    
    [self setMinRadius:44.0f];
    [self setMinOuterRadius:54.0f];
    
    [self setTouchTrackerColor:[UIColor blackColor]];
}

- (void)drawRect:(CGRect)rect {
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetShouldAntialias(context, YES);
    
    CGContextSetStrokeColorWithColor(context, [self TouchTrackerColor].CGColor);
    CGContextSetLineWidth(context, [self CircleThickness]);
    
    CGRect circRect = CGRectMake(m_touchCenter.x - _CircleRadius, m_touchCenter.y - _CircleRadius, _CircleRadius * 2.0f, _CircleRadius * 2.0f);
    CGContextAddEllipseInRect(context, circRect);
    CGContextStrokePath(context);
    
    // Line
    float xxLine = m_touchCenter.x;
    float yyLine = m_touchCenter.y;
    
    float theta = atan((m_touchCenter.x - rect.size.height/2.0f)/(m_touchCenter.y - rect.size.width/2.0f));
    //NSLog(@"theta: %f", theta);
    float xLine = xxLine - (_CircleRadius) * sinf(theta);
    float yLine = yyLine - (_CircleRadius) * cosf(theta);
    
    float lineStartX = rect.size.width/2.0f + (_MinRadius) * sinf(theta);
    float lineStartY = rect.size.height/2.0f + (_MinRadius) * cosf(theta);

    if(m_quad == TOUCH_QUAD_I || m_quad == TOUCH_QUAD_II) {
        //NSLog(@"hi");
        xLine = xxLine + (_CircleRadius) * sinf(theta);
        yLine = yyLine + (_CircleRadius) * cosf(theta);
        
        lineStartX = rect.size.width/2.0f - (_MinRadius) * sinf(theta);
        lineStartY = rect.size.height/2.0f - (_MinRadius) * cosf(theta);
    }
    
    CGContextSetLineWidth(context, [self LineThickness]);
    CGContextMoveToPoint(context, lineStartX, lineStartY);
    CGContextAddLineToPoint(context, xLine, yLine);
    CGContextStrokePath(context);
    
    UIGraphicsEndImageContext();
}

-(void)updateTouchView:(CGPoint)touchPoint withCenter:(CGPoint)center andDegree:(float)degree {
    float width = touchPoint.x - center.x;
    float height = touchPoint.y - center.y;
    
    if(width >= 0.0 && height < 0.0)
        m_quad = TOUCH_QUAD_I;
    else if(width < 0.0 && height < 0.0)
        m_quad = TOUCH_QUAD_II;
    else if(width < 0.0 && height >= 0.0)
        m_quad = TOUCH_QUAD_III;
    else if(width > 0.0 && height >= 0.0)
        m_quad = TOUCH_QUAD_IV;
    
    float dist = sqrtf(powf(width, 2.0f) + powf(height, 2.0f));
    //NSLog(@"dist: %f", dist);
    
    if(dist < _MinOuterRadius) {
        //NSLog(@"less");
        float theta = degree / (180.0f / M_PI);
        width = _MinOuterRadius * 2.0f + 2.0f * _CircleRadius + _CircleThickness * 2.0f;
        height = _MinOuterRadius * 2.0f + 2.0f * _CircleRadius + _CircleThickness * 2.0f;
        switch(m_quad) {
            case TOUCH_QUAD_I: {
                m_touchCenter.x = width/2.0f + _MinOuterRadius * fabs(sinf(theta));
                m_touchCenter.y = height/2.0f - _MinOuterRadius * fabs(cosf(theta));
            } break;
                
            case TOUCH_QUAD_II: {
                m_touchCenter.x = width/2.0f - _MinOuterRadius * fabs(sinf(theta));
                m_touchCenter.y = height/2.0f - _MinOuterRadius * fabs(cosf(theta));
            } break;
                
            case TOUCH_QUAD_III: {
                m_touchCenter.x = width/2.0f - _MinOuterRadius * fabs(sinf(theta));
                m_touchCenter.y = height/2.0f + _MinOuterRadius * fabs(cosf(theta));
            } break;
                
            case TOUCH_QUAD_IV: {
                m_touchCenter.x = width/2.0f + _MinOuterRadius * fabs(sinf(theta));
                m_touchCenter.y = height/2.0f + _MinOuterRadius * fabs(cosf(theta));
            } break;
        }
    }
    else {
        width = fabs(width)*2.0f + 2.0f * _CircleRadius + _CircleThickness * 2.0f;
        height = fabs(height)*2.0f + 2.0f * _CircleRadius + _CircleThickness * 2.0f;
        
        if(width > height)
            height = width;
        else
            width = height;
        
        m_touchCenter.x = width/2.0f + (touchPoint.x - center.x);
        m_touchCenter.y = height/2.0f + (touchPoint.y - center.y);
    }
    
    float originX = center.x - width / 2.0f;
    float originY = center.y - height / 2.0f;
    
    [self setFrame:CGRectMake(originX, originY, width, height)];
    [self setNeedsDisplay];
}



@end
