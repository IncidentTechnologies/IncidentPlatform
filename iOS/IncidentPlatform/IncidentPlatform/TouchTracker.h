//
//  TouchTracker.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef enum TouchQuad {
    TOUCH_QUAD_I,
    TOUCH_QUAD_II,
    TOUCH_QUAD_III,
    TOUCH_QUAD_IV,
    TOUCH_QUAD_INVALID
} TOUCH_QUAD;

@interface TouchTracker : UIView {

    //float m_circleThickness;
    //float m_lineThickness;
    
    TOUCH_QUAD m_quad;
    
    CGPoint m_touchCenter;
}

@property UIColor *TouchTrackerColor;

@property float CircleThickness;
@property float LineThickness;

@property float CircleRadius;
@property float MinRadius;
@property float MinOuterRadius;

-(void)updateTouchView:(CGPoint)touchPoint withCenter:(CGPoint)center andDegree:(float)degree;

@end
