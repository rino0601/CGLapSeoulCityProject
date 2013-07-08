//
//  GLView.h
//  helloCone
//
//  Created by Hanjong Ko on 13. 1. 22..
//  Copyright (c) 2013년 lemonApple. All rights reserved.
//

#import "IRenderingEngine.hpp"
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>

@interface GLView : UIView {
	EAGLContext *m_context;
	IRenderingEngine *m_renderingEngine;
	float m_timestamp;
}

+(Class) layerClass ;
- (IRenderingEngine *)RenderEngine;
- (void) drawView:(CADisplayLink *) displayLink;
- (id)initWithFrame:(CGRect)frame;

@end


//interface for C++

class CGLView {
private:
	GLView *instance;
public:
	void AllocInitWithFrame(CGRect frame);
	void getBufferImage(IplImage *buffer);
	void Clear(float r, float g, float b);
	void DrawCone(int x, int y, Byte r, Byte g, Byte b, int depth);
};