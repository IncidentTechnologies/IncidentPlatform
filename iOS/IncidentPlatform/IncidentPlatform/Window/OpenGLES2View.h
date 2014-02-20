//
//  OpenGLES2View.h
//  IncidentPlatform
//
//  Created by Idan Beck on 1/6/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface OpenGLES2View : UIView {
    CAEAGLLayer *m_eaglLayer;
    EAGLContext *m_eaglContext;
    GLuint m_colorRenderBuffer;
    
    GLuint m_positionSlot;
    GLuint m_colorSlot;
    GLuint m_projectionMatrix;
    GLuint m_modelView;
}

-(void)setupDisplayLink;

-(void) setupContext;
-(void) setupColorRenderBuffer;
-(void) setupFrameBuffer;

-(void) render:(CADisplayLink*)displayLink;

-(void) setupView;

- (GLuint) compileShader:(NSString*)shaderName withType:(GLenum)shaderType;
- (void) compileShaders;

@end
