//
//  Knob.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/16/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "UIKnob.h"

@implementation UIKnob

@synthesize delegate;

-(id) initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    
    if(self){
        [self awakeFromNib];
    }
    
    return self;
}

-(void) awakeFromNib {
    [self setStartValue:0.0f];
    [self setEndValue:1.0f];
    m_value = 0.0;
    
    
    [self setStartDegree:135.0f];
    [self setEndDegree:45.0f];
    [self setFClockWise:true];
    
    m_inset = 6.0f;
    
    m_outerWidth = 3.0f;
    m_fTouched = false;
    
    // Create the touchdown view
    m_touchView = [[TouchTracker alloc] init];
    [m_touchView setBackgroundColor:[UIColor clearColor]];
    [m_touchView setHidden:TRUE];
    [self addSubview:m_touchView];
    
    // Force Square
    if([self frame].size.height != [self frame].size.width) {
        float newDimension = MAX([self frame].size.width, [self frame].size.height);
        CGRect newFrame = CGRectMake([self frame].origin.x, [self frame].origin.y, newDimension, newDimension);
        [self setFrame:newFrame];
    }
    
    float frameWidth = [self frame].size.width;
    [m_touchView setMinRadius:(frameWidth/2.0f - m_inset)];
    [m_touchView setMinOuterRadius:(frameWidth/2.0f - m_inset + [m_touchView CircleRadius])];
    
    // Set the outset
    m_outset = frameWidth / 10.0f;
    
    [self setFAbsolute:false];
    [self setRotateFactor:0.01f];
}

-(double) GetValue {
    double range = _EndValue - _StartValue;
    double value = m_value * range + _StartValue;
    
    return value;
}

-(void) SetValue:(double)value {
    double range = _EndValue - _StartValue;
    
    m_value = (value - _StartValue) / range;
    if(m_value > 1.0f)
        m_value = 1.0f;
    else if(m_value < 0.0f)
        m_value = 0.0f;
    
    // Redraw
    [self setNeedsDisplay];
    
    return;
}

-(float)getDegreeFromPoint:(CGPoint)pt withCenter:(CGPoint)center {
    float xMidDiff = pt.x - center.x;
    float yMidDiff = center.y - pt.y;
    
    //float dist = sqrt(pow(xMidDiff, 2.0f) + pow(yMidDiff, 2.0f));
    float theta = atan(yMidDiff / xMidDiff);
    
    // convert theta
    if(xMidDiff >= 0.0f && yMidDiff > 0.0f) {        // QUAD I
        theta = M_PI + M_PI_2 + theta;
    }
    else if(xMidDiff < 0.0f && yMidDiff >= 0.0f) {   // QUAD II
        theta = M_PI_2 + (theta);
    }
    else if(xMidDiff < 0.0f && yMidDiff < 0.0f) {   // QUAD III
        theta = M_PI_2 + theta;
    }
    else if(xMidDiff >= 0.0f && yMidDiff <= 0.0f) {   // QUAD IV
        theta = M_PI + M_PI_2 + theta;
    }

    return (theta * 180.0f) / M_PI;
}

-(BOOL)beginTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    m_ptStart = [touch locationInView:self];
    m_ptCurrent = m_ptStart;
    m_fTouched = true;
    
    [m_touchView setHidden:false];
    
    float xMidPoint = [self frame].size.width / 2.0f;
    float yMidPoint = [self frame].size.height / 2.0f;
    m_lastDegree = [self getDegreeFromPoint:m_ptStart withCenter:CGPointMake(xMidPoint, yMidPoint)];
    
    [m_touchView updateTouchView:m_ptStart withCenter:CGPointMake(xMidPoint, yMidPoint) andDegree:m_lastDegree];
    
    //NSLog(@"Start Touch");
    
    return true;
}

-(BOOL)continueTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    CGPoint ptCurrent = [touch locationInView:self];
    
    //float xDiff = ptCurrent.x - m_ptCurrent.x;
    //float yDiff = ptCurrent.y - m_ptCurrent.y;
    
    // Determine polar coord
    float xMidPoint = [self frame].size.width / 2.0f;
    float yMidPoint = [self frame].size.height / 2.0f;

    float degree = [self getDegreeFromPoint:ptCurrent withCenter:CGPointMake(xMidPoint, yMidPoint)];
    
    [m_touchView updateTouchView:ptCurrent withCenter:CGPointMake(xMidPoint, yMidPoint) andDegree:degree];
    
    float degreeDiff = m_lastDegree - degree;
    
    // Bit of a hack but it works
    if(degreeDiff > 270.f)
        degreeDiff -= 360.0f;
    else if(degreeDiff < -270.0f)
        degreeDiff += 360.0f;
    
    m_lastDegree = degree;
    m_ptCurrent = ptCurrent;
    
    //NSLog(@"degree: %f diff: %f", degree, degreeDiff);
    
    // convert degree to value
    float degreeRange;
    if(_EndDegree > _StartDegree)
        degreeRange = (_EndDegree - _StartDegree);
    else
        degreeRange = (360.0f - _StartDegree) + _EndDegree;
    
    float degreeValue = _StartDegree - degree;
    if(degreeValue < 0)
        degreeValue += 360.0f;
    
    if(_fAbsolute)
        m_value = degreeValue / degreeRange;
    else
        m_value += degreeDiff * _RotateFactor;
    
    if(m_value > 1.0f)
        m_value = 1.0f;
    if(m_value < 0.0f)
        m_value = 0.0f;
    
    //NSLog(@"Continue Touch %f %f val: %f", degree, dist, m_value);
    [self sendActionsForControlEvents:UIControlEventValueChanged];
    
    [self setNeedsDisplay];
    return true;
}

-(void)endTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    CGPoint ptEnd = [touch locationInView:self];
    m_fTouched = false;
    
    [m_touchView setHidden:true];
    [delegate trackingDidEnd];
}

-(void) drawRect:(CGRect)rect{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetShouldAntialias(context, YES);
    
    float degree;
    if(_EndDegree > _StartDegree)
        degree = _EndDegree - _StartDegree;
    else
        degree = (360.0f - _StartDegree) + _EndDegree;
    
    degree = _StartDegree + (degree * m_value);
    if(degree >= 360.0f)
        degree -= 360.0f;
    
    float theta = degree / (180.0f / M_PI);
    
    float xMid = rect.origin.x + (rect.size.width / 2.0f);
    float yMid = rect.origin.y + (rect.size.height / 2.0f);
    float xRadius = ((rect.size.width / 2.0f) - m_inset);
    float yRadius = ((rect.size.height / 2.0f) - m_inset);
    
    // Outer Circle
    UIColor *outerColor = [UIColor colorWithRed:(84.0f/255.0f) green:(159.0f/255.0f) blue:(215.0f/255.0f) alpha:1.0f];
    CGContextSetStrokeColorWithColor(context, outerColor.CGColor);
    float startTheta = _StartDegree / (180.0f / M_PI);
    float endTheta = _EndDegree / (180.0f / M_PI);
    CGContextAddArc(context, xMid, yMid, xRadius, startTheta, endTheta, !_fClockWise);
    CGContextStrokePath(context);

    // Line
    CGContextSetStrokeColorWithColor(context, [UIColor blackColor].CGColor);
    CGContextSetLineWidth(context, 2.0);
    
    float xVec = cosf(theta);
    float yVec = sinf(theta);
    
    float xStartPoint = xMid + xVec * m_outset;
    float yStartPoint = yMid + yVec * m_outset;
    
    CGContextMoveToPoint(context, xStartPoint, yStartPoint);
    
    float xLineVal = xMid + xVec * xRadius;
    float yLineVal = yMid + yVec * yRadius;
    
    CGContextAddLineToPoint(context, xLineVal, yLineVal);
    CGContextStrokePath(context);
    
    // Highlight Arc
    CGContextAddArc(context, xMid, yMid, xRadius + 1.5f, startTheta, theta, !_fClockWise);
    CGContextSetLineWidth(context, 3.0);
    CGContextSetStrokeColorWithColor(context, [UIColor orangeColor].CGColor);
    CGContextStrokePath(context);
    
    // Text
    /*
    CGRect textRect = CGRectInset(rect, 30, 30);
    int textVal = (int)((m_endValue - m_startValue) * m_value) + m_startValue;
    NSString *text = [NSString stringWithFormat:@"%d", textVal];
    
    NSDictionary *fontAttributes = @{NSFontAttributeName: [UIFont fontWithName:@"Arial" size:20.0f],
                                     NSForegroundColorAttributeName: [UIColor blackColor]};
    [text drawInRect:textRect withAttributes:fontAttributes];
     */
    
    /*
    theta += .01f;
    if(theta >= 2.0f * M_PI)
        theta = 0.0f;
     */
    
    //[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(cbDrawRect:) userInfo:nil repeats:NO];
    
}

- (void) cbDrawRect:(NSTimer *) timer {
    [self setNeedsDisplay];
}

@end