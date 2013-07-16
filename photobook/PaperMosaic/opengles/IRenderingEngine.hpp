//
//  IRenderingEngine.h
//  helloCone
//
//  Created by Hanjong Ko on 13. 1. 22..
//  Copyright (c) 2013년 lemonApple. All rights reserved.
//

#ifndef __helloCone__IRenderingEngine__
#define __helloCone__IRenderingEngine__
enum DeviceOrientation {
	DeviceOrientationUnkown,
	DeviceOrientationPortrait,
	DeviceOrientationPortraitUpsideDown,
	DeviceOrientationLandscapeLeft,
	DeviceOrientationLandscapeRight,
	DeviceOrientationFaceUp,
	DeviceOrientationFaceDown,
};

struct IRenderingEngine *CreateRenderer1();

struct IRenderingEngine {
	virtual void Initialize(int width, int height)=0;
	virtual void Render() const = 0;
	virtual ~IRenderingEngine(){}
	
	virtual void getBufferImage(IplImage * buffer) = 0;
	virtual void Clear(float r, float g, float b) = 0;
	virtual void DrawCone(int x, int y, Byte r, Byte g, Byte b, int depth) = 0;
};
#endif /* defined(__helloCone__IRenderingEngine__) */
