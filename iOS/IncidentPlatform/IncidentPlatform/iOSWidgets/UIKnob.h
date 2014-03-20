//
//  Knob.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/16/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TouchTracker.h"

typedef enum KnobType{
    KNOB_RANGE,
    KNOB_ENDLESS,
    KNOB_INVALID
} KNOB_TYPE;

@protocol UIKnobDelegate <NSObject>

- (void)trackingDidEnd;
- (void)enableKnob:(id)sender;
- (void)disableKnob:(id)sender;

@end

@interface UIKnob : UIControl {
    double m_value; // this is a [0, 1] value
 
    KNOB_TYPE m_type;
    bool m_fClickable;
    
    //double m_startValue;
    //double m_endValue;
    
    //float m_startDegree;
    //float m_endDegree;
    //BOOL m_fClockWise;
    
    float m_inset;
    float m_outset;
    float m_outerWidth;
    
    // Touch tracking
    CGPoint m_ptStart;
    CGPoint m_ptCurrent;
    BOOL m_fTouched;
    TouchTracker *m_touchView;
    
    // Behavior
    //BOOL m_fAbsolute;
    //float m_RotateFactor;
    float m_lastDegree;
    
    BOOL m_fEnabled;
}

@property (weak, nonatomic) id<UIKnobDelegate>delegate;

@property UIColor *OuterColor;
@property UIColor *HighlightColor;
@property UIColor *LineColor;
@property UIColor *DisabledColor;

@property double StartValue;
@property double EndValue;

@property double StartDegree;
@property double EndDegree;

@property BOOL fClockWise;
@property BOOL fAbsolute;       // Will set the value to radial point
@property float RotateFactor;

-(void) awakeFromNib;
-(id) initWithFrame:(CGRect)frame;

-(double) GetValue;
-(void) SetValue:(double)value;

- (void) cbDrawRect:(NSTimer *) timer;

-(float)getDegreeFromPoint:(CGPoint)pt withCenter:(CGPoint)center;

- (void)handleTapGesture:(UITapGestureRecognizer *)sender;

-(void)EnableKnob;
-(void)DisableKnob;
-(BOOL)isEnabled;

-(void)setTouchTrackerColor:(UIColor*)color;
-(void)setTouchTrackerCircleThickness:(float)thickness;
-(void)setTouchTrackerCircleRadius:(float)radius;
-(void)setTouchTrackerLineThickness:(float)thickness;

@end
