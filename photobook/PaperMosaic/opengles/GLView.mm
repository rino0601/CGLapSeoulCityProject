//
//  GLView.m
//  helloCone
//
//  Created by Hanjong Ko on 13. 1. 22..
//  Copyright (c) 2013년 lemonApple. All rights reserved.
//

#import <OpenGLES/EAGLDrawable.h>
#import "GLView.h"
#import <mach/mach_time.h>
#import <OpenGLES/ES2/gl.h>
#import "UIImageCVArrConverter.h"

@implementation GLView

+(Class) layerClass {
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)super.layer;
		//backbuffer를 받기위해 필요함.
		eaglLayer.drawableProperties = @{kEAGLDrawablePropertyRetainedBacking: [NSNumber numberWithBool:YES],kEAGLDrawablePropertyColorFormat: kEAGLColorFormatRGBA8};
		eaglLayer.opaque = YES;
		m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		if(!m_context || ![EAGLContext setCurrentContext:m_context]) {
			return nil;
		}
		m_renderingEngine = CreateRenderer1();
		
		[m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
		m_renderingEngine->Initialize(CGRectGetWidth(frame), CGRectGetHeight(frame));
		
		[self drawView:nil];
		m_timestamp = CACurrentMediaTime();
			
//		CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
//		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}

- (IRenderingEngine *)RenderEngine {
	return m_renderingEngine;
}

- (void) drawView:(CADisplayLink *)displayLink {
	if(displayLink != nil) {
		m_timestamp = displayLink.timestamp;
	}
	
	m_renderingEngine->Render();
	[m_context presentRenderbuffer:GL_RENDERBUFFER];
}
@end

#pragma mark -
#pragma mark interface for C++

void CGLView::AllocInitWithFrame(CGRect frame){
	this->instance = [[GLView alloc] initWithFrame:frame];
}
void CGLView::getBufferImage(IplImage *buffer) {
	[instance RenderEngine]->Render();
	[instance RenderEngine]->getBufferImage(buffer);
}
void CGLView::Clear(float r, float g , float b) {
	[instance RenderEngine]->Clear(r, g, b);
}
void CGLView::DrawCone(int x, int y, Byte r, Byte g, Byte b, int depth) {
	[instance RenderEngine]->DrawCone(x, y, r, g, b, depth);
}