//
//  ColorPickerView.m
//  TimidTiger
//
//  Created by Hanjong Ko on 13. 7. 4..
//
//

#import "ColorPickerView.h"

@interface ColorPickerView ()
@property CGFloat xGap;
@property CGFloat yGap;
@property CGFloat rDegree;
@property CGFloat gDegree;
@property CGFloat bDegree;
@end

@implementation ColorPickerView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
		self.backgroundColor = [UIColor blackColor];
		_xGap = CGRectGetWidth(frame)/256;
		_yGap = CGRectGetHeight(frame)/3;
		_rDegree=_gDegree=_bDegree=0;
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
	CGContextRef ctx;
	//더블 버퍼링을 위한 Graphic Context 할당
	CGColorSpaceRef cs=CGColorSpaceCreateDeviceRGB();
	ctx = CGBitmapContextCreate(NULL,
								(int)CGRectGetWidth(rect),
								(int)CGRectGetHeight(rect),
								8,
								4*(int)CGRectGetWidth(rect),
								cs,
								kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(cs);
	
	
	
	//iOS CGContext Code.
	CGContextSetLineWidth(ctx, 1); // 1이상 값.
	CGContextSetLineCap(ctx, kCGLineCapRound);
	
	for (int i=0; i<3; i++) {
		for (int j=0; j<256; j++) {
			CGRect tile = CGRectMake(j*_xGap,i*_yGap,_xGap,_yGap);
			if(CGRectIntersectsRect(tile, rect)){
				CGContextSetRGBFillColor(ctx, i==0?j/255.0f:0, i==1?j/255.0f:0, i==2?j/255.0f:0, 1);
				CGContextBeginPath(ctx);
				CGContextAddRect(ctx, tile);
				CGContextClosePath(ctx);
				CGContextFillPath(ctx);
			}
		}
	}
	
	//화면에 그리기
	CGImageRef i=CGBitmapContextCreateImage(ctx);
	CGContextDrawImage(UIGraphicsGetCurrentContext(), [self bounds], i);
	CGImageRelease(i);
	
	// 다 그렸으니 메모리 해제.
	CGContextRelease(ctx);
	return ;
}


@end
