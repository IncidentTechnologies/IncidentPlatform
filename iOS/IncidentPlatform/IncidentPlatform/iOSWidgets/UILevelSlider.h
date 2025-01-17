//
//  UILevelSlider.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/18/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol UILevelSliderDelegate <NSObject>

- (void)valueDidChange:(double)newValue forSlider:(id)sender;

@end

@interface UILevelSlider : UIControl {
    float m_sliderHeight;
    float m_inset;
    
    float m_trackWidth;
    
    float m_greenPoint;
    float m_lightGreenPoint;
    float m_yellowPoint;
    float m_redPoint;
    
    // Touch tracking
    CGPoint m_ptStart;
    CGPoint m_ptCurrent;
    BOOL m_fTouched;
    BOOL m_fIntersected;
}

@property (weak, nonatomic) id<UILevelSliderDelegate>delegate;

@property UIColor *GreenColor;
@property UIColor *LightGreenColor;
@property UIColor *YellowColor;
@property UIColor *RedColor;
@property UIColor *ControlColor;

@property (nonatomic) double SliderValue;
@property (nonatomic) double DisplayValue;

-(double)GetValue;
-(BOOL) TouchIntersectControl:(CGPoint)touch;

- (void) cbDrawRect:(NSTimer *) timer;

@end
