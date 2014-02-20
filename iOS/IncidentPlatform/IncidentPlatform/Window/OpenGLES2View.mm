//
//  OpenGLES2View.m
//  IncidentPlatform
//
//  Created by Idan Beck on 1/6/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "OpenGLES2View.h"

#import "GLES2Triangle.h"
#import "GLES2MeshObj.h"
#import "MatrixFactory.h"

@interface OpenGLES2View () {
    
}
@end

@implementation OpenGLES2View

-(id) init {
    self = [super init];
    if(self) {
        NSLog(@"OpenGLES2View: init");
    }
    
    return self;
}

- (id)initWithFrame:(CGRect)frame{
    self = [super initWithFrame:frame];
    
    if (self) {
        NSLog(@"OpenGLES2View: initWithFrame:");
        [self setupView];
    }
    return self;
}

-(void) setupView {
    [self setupLayer];
    [self setupContext];
    [self setupColorRenderBuffer];
    [self setupFrameBuffer];
    
    [self compileShaders];
    
    [self setupDisplayLink];
}

// Set layer class to CAEAGLayer
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void)setupLayer {
    m_eaglLayer = (CAEAGLLayer*) self.layer;
    m_eaglLayer.opaque = TRUE;
}

- (void)setupContext {
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    m_eaglContext = [[EAGLContext alloc] initWithAPI:api];

    if(!m_eaglContext) {
        NSLog(@"Failed to initialize OpenGL ES2.0 context, exiting");
        exit(1);
    }
    
    if(![EAGLContext setCurrentContext:m_eaglContext]) {
        NSLog(@"Failed to set context");
        exit(1);
    }
}

-(void)setupColorRenderBuffer {
    glGenRenderbuffers(1, &m_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderBuffer);
    [m_eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:m_eaglLayer];
}

-(void)setupFrameBuffer {
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderBuffer);
}

-(void)setupDisplayLink {
    CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void) render:(CADisplayLink*)displayLink {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Set up the projection
    float aspect = [[UIScreen mainScreen] bounds].size.height / [[UIScreen mainScreen] bounds].size.width;    
    matrix<float> projMat = CreateOpenGLProjectionMatrix(1.0f, 100.0f);
    glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, (GLfloat *)(projMat.matrix_entry));
    
    // Set up viewport
    glViewport(0.0f, 0.0f, self.frame.size.width, self.frame.size.height);
    
    // Create a tri
    GLES2Triangle *tri = new GLES2Triangle(TRIANGLE_UNIT_EQUILATERAL);
    tri->GLES2Init();
    
    //tri->MoveX(0.1f);
    tri->MoveZ(-1.0f);
    tri->RotateY(CACurrentMediaTime());
    
    tri->GLES2Update(m_modelView);
    
    
    tri->Render(m_positionSlot, m_colorSlot);
    
    /*
    NSBundle *IncidentPlatformBundle = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:@"IncidentPlatformBundle" withExtension:@"bundle"]];
    NSString *pTempFilePath = [IncidentPlatformBundle pathForResource:@"teapot" ofType:@"obj"];
    GLES2MeshObj *meshObj = new GLES2MeshObj((char *)[pTempFilePath UTF8String]);
    meshObj->GLES2Init();
    meshObj->GLES2Update();
    meshObj->Render(m_positionSlot, m_colorSlot);
     */
    
    if(![m_eaglContext presentRenderbuffer:GL_RENDERBUFFER]) {
        NSLog(@"Failed to present render buffer");
    }
}

- (GLuint) compileShader:(NSString*)shaderName withType:(GLenum)shaderType {
    // Get Shader Source
    NSError *error;
    //NSString *shaderPath = [[NSBundle mainBundle] pathForResource:shaderName ofType:@"glsl"];

    NSBundle *IncidentPlatformBundle = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:@"IncidentPlatformBundle" withExtension:@"bundle"]];
    NSString *shaderPath = [IncidentPlatformBundle pathForResource:shaderName ofType:@"glsl"];
    
    NSString *shaderString = [NSString stringWithContentsOfFile:shaderPath encoding:NSUTF8StringEncoding error:&error];
    if(!shaderString) {
        NSLog(@"compileShader: Error loading shader %@: %@", shaderName, error.localizedDescription);
        exit(1);
    }
    
    // Create the Shader for Compilation
    GLuint shaderHandle = glCreateShader(shaderType);
    const char *shaderStringUTF8 = [shaderString UTF8String];
    int shaderStringUTF8_n = [shaderString length];
    glShaderSource(shaderHandle, 1, &shaderStringUTF8, &shaderStringUTF8_n);
    
    // Compile the shader
    glCompileShader(shaderHandle);
    
    // Check Compilation Status
    GLint compileStatus;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) {
        GLchar msg[256];
        glGetShaderInfoLog(shaderHandle, sizeof(msg), 0, &msg[0]);
        NSString *msgString = [NSString stringWithUTF8String:msg];
        NSLog(@"compileShader: Error compiling shader: %@", msgString);
        exit(1);
    }
    
    return shaderHandle;
}

- (void) compileShaders {
    GLuint vertexShader = [self compileShader:@"vertex" withType:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self compileShader:@"fragment" withType:GL_FRAGMENT_SHADER];
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    glLinkProgram(programHandle);
    
    GLint linkStatus;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == GL_FALSE) {
        GLchar msg[256];
        glGetProgramInfoLog(programHandle, sizeof(msg), 0, &msg[0]);
        NSString *msgString = [NSString stringWithUTF8String:msg];
        NSLog(@"compileShaders: Error linking shaders: %@", msgString);
        exit(1);
    }
    
    glUseProgram(programHandle);
    
    // Connect the vars to the vertex shader
    m_positionSlot = glGetAttribLocation(programHandle, "Position");
    glEnableVertexAttribArray(m_positionSlot);
    
    m_colorSlot = glGetAttribLocation(programHandle, "SourceColor");
    glEnableVertexAttribArray(m_colorSlot);
    
    // Projection Matrix
    m_projectionMatrix = glGetUniformLocation(programHandle, "Projection");
    
    // Model View
    m_modelView = glGetUniformLocation(programHandle, "ModelView");
}



@end
