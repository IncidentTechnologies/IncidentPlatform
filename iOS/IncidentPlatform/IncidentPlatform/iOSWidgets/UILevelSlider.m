//
//  UILevelSlider.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/18/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "UILevelSlider.h"

@implementation UILevelSlider

@synthesize SliderValue = _SliderValue;
@synthesize DisplayValue = _DisplayValue;

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    
    if (self) {
        [self awakeFromNib];
    }
    
    return self;
}

-(double)GetValue {
    return (1.0f - _SliderValue);
}

-(void)awakeFromNib {
    m_sliderHeight = 20.0f;
    m_inset = 5.0f;
    
    m_trackWidth = 5.0f;
    
    m_greenPoint = 0.5f;
    m_lightGreenPoint = 0.7f;
    m_yellowPoint = 0.85f;
    m_redPoint = 1.0f;
    
    [self setSliderValue:0.0f];
    [self setDisplayValue:1.0f];
    
    m_fTouched = false;
}

-(void)setSliderValue:(double)value {
    _SliderValue = value;
    
    if(_SliderValue > 1.0f)
        _SliderValue = 1.0f;
    else if(_SliderValue < 0.0f)
        _SliderValue = 0.0f;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self setNeedsDisplay];
    });
}

-(double)SliderValue {
    return _SliderValue;
}

-(void)setDisplayValue:(double)value {
    _DisplayValue = value;
    
    if(_DisplayValue > 1.0f)
        _DisplayValue = 1.0f;
    else if(_DisplayValue < 0.0f)
        _DisplayValue = 0.0f;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self setNeedsDisplay];
    });
}

-(double)DisplayValue {
    return _DisplayValue;
}

-(BOOL) TouchIntersectControl:(CGPoint)touch {
    float effectiveHeight = [self frame].size.height - m_sliderHeight;
    float controlPoint = (_SliderValue * effectiveHeight) + m_sliderHeight/2.0f;
    
    if(touch.y > (controlPoint - m_sliderHeight/2.0f) && touch.y < (controlPoint + m_sliderHeight/2.0f))
        return true;
    else
        return false;
}

-(BOOL)beginTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    m_ptStart = [touch locationInView:self];
    m_ptCurrent = m_ptStart;
    m_fTouched = true;
    m_fIntersected = false;
    
    if([self TouchIntersectControl:m_ptStart])
        m_fIntersected = true;
    
    //NSLog(@"Start Touch");
    
    return true;
}

-(BOOL)continueTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    CGPoint ptCurrent = [touch locationInView:self];
    
    if(!m_fIntersected) {
        if([self TouchIntersectControl:ptCurrent])
            m_fIntersected = true;
    }
    else {
        CGRect rect = [self frame];
        float effectiveHeight = rect.size.height - m_sliderHeight;
        float effectiveTouch = ptCurrent.y - m_sliderHeight/2.0f;
        
        if(effectiveTouch < 0.0f)
            effectiveTouch = 0.0f;
        else if(effectiveTouch > effectiveHeight)
            effectiveTouch = effectiveHeight;
        
        _SliderValue = effectiveTouch / effectiveHeight;
        
        [self sendActionsForControlEvents:UIControlEventValueChanged];
        [self setNeedsDisplay];
    }
    
    return true;
}

-(void)endTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    //CGPoint ptEnd = [touch locationInView:self];
    m_fTouched = false;
}

- (void)drawRect:(CGRect)rect {
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetShouldAntialias(context, YES);
    
    CGContextSetLineWidth(context, m_trackWidth);
    
    float effectiveHeight = rect.size.height - m_sliderHeight;
    float trackStartHeight = rect.size.height - m_sliderHeight/2.0f;
    float trackEndHeight = m_sliderHeight/2.0f;
    
    // Track
    CGContextSetStrokeColorWithColor(context, [UIColor blackColor].CGColor);
    CGContextMoveToPoint(context, rect.size.width/2.0f, trackEndHeight);
    CGContextAddLineToPoint(context, rect.size.width/2.0f, trackStartHeight);
    CGContextStrokePath(context);
    
    // Green Level
    CGContextSetStrokeColorWithColor(context, [UIColor greenColor].CGColor);
    float greenVal = (_DisplayValue > m_greenPoint) ? m_greenPoint : _DisplayValue;
    float greenHeight = effectiveHeight * greenVal;
                            
    CGContextMoveToPoint(context, rect.size.width/2.0f, trackStartHeight);
    CGContextAddLineToPoint(context, rect.size.width/2.0f, trackStartHeight - greenHeight);
    CGContextStrokePath(context);
    
    // Light Green Level
    if(_DisplayValue > m_greenPoint) {
        CGContextSetStrokeColorWithColor(context, [UIColor colorWithRed:(176.0f/255.0f) green:(253.0f/255.0f) blue:(147.0f/255.0f) alpha:1.0f].CGColor);
        float lightGreenVal = (_DisplayValue > m_lightGreenPoint) ? m_lightGreenPoint : _DisplayValue;
        float lightGreenStart = m_greenPoint * effectiveHeight;
        float lightGreenHeight = effectiveHeight * lightGreenVal;
        
        CGContextMoveToPoint(context, rect.size.width/2.0f, trackStartHeight - lightGreenStart);
        CGContextAddLineToPoint(context, rect.size.width/2.0f, trackStartHeight - lightGreenHeight);
        CGContextStrokePath(context);
    }
    

    // Yellow Level
    if(_DisplayValue > m_lightGreenPoint) {
        CGContextSetStrokeColorWithColor(context, [UIColor yellowColor].CGColor);
        float yellowVal = (_DisplayValue > m_yellowPoint) ? m_yellowPoint : _DisplayValue;
        float yellowStart = m_lightGreenPoint * effectiveHeight;
        float yellowHeight = effectiveHeight * yellowVal;
        
        CGContextMoveToPoint(context, rect.size.width/2.0f, trackStartHeight - yellowStart);
        CGContextAddLineToPoint(context, rect.size.width/2.0f, trackStartHeight - yellowHeight);
        CGContextStrokePath(context);
    }
    
    // Red Level
    if(_DisplayValue > m_yellowPoint) {
        CGContextSetStrokeColorWithColor(context, [UIColor redColor].CGColor);
        float redVal = (_DisplayValue > m_redPoint) ? m_redPoint : _DisplayValue;
        float redStart = m_yellowPoint * effectiveHeight;
        float redHeight = effectiveHeight * redVal;
        
        CGContextMoveToPoint(context, rect.size.width/2.0f, trackStartHeight - redStart);
        CGContextAddLineToPoint(context, rect.size.width/2.0f, trackStartHeight - redHeight);
        CGContextStrokePath(context);
    }
    
    // Control Rect
    CGContextSetFillColorWithColor(context, [UIColor blueColor].CGColor);
    float effSliderPoint = _SliderValue * effectiveHeight;
    float actualSliderPoint = effSliderPoint + m_sliderHeight/2.0f;
    CGRect levelRect = CGRectMake(m_inset, actualSliderPoint - m_sliderHeight/2.0f, rect.size.width - 2.0f*m_inset, m_sliderHeight);
    CGContextFillRect(context, levelRect);
    
    /*
    static float theta = 0.0f;
    _DisplayValue = (sin(theta) + 1.0f)/2.0f ;
    _SliderValue = (cos(theta) + 1.0f)/2.0f ;
    theta += 0.05;
    [NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(cbDrawRect:) userInfo:nil repeats:NO];
    */
}

- (void) cbDrawRect:(NSTimer *) timer {
    [self setNeedsDisplay];
}


@end
