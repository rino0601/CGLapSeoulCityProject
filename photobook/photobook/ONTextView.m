//
//  ONTextView.m
//  photobook
//
//  Created by SeulBeom on 13. 8. 14..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "ONTextView.h"

@implementation ONTextView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}


- (id)styleString {
    return [[super styleString] stringByAppendingString:@"; line-height: 1.5em"];
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void) drawTextInRect:(CGRect)rect
{
 CGContextRef context = UIGraphicsGetCurrentContext();
 CGContextSelectFont (context, [self.font.fontName cStringUsingEncoding:NSUTF8StringEncoding], self.font.pointSize, kCGEncodingMacRoman);
 CGContextSetCharacterSpacing(context, 0.85);
 CGContextSetFillColorWithColor(context, [[UIColor clearColor] CGColor]);
 CGAffineTransform myTextTransform = CGAffineTransformScale(CGAffineTransformIdentity, 1.f, -1.f );
 CGContextSetTextMatrix (context, myTextTransform);
 
 // draw 1 but invisbly to get the string length.
 CGPoint p =CGContextGetTextPosition(context);
 float centeredY = (self.font.pointSize + (self.frame.size.height- self.font.pointSize)/2)-2;
 CGContextShowTextAtPoint(context, 0, centeredY, [self.text cStringUsingEncoding:NSUTF8StringEncoding ], [self.text length]);
 CGPoint v =CGContextGetTextPosition(context);
 
 // calculate width and draw second one.
 float width = v.x - p.x;
 float centeredX =(self.frame.size.width- width)/2;
 CGContextSetFillColorWithColor(context, [self.textColor CGColor]);
 CGContextShowTextAtPoint(context, centeredX, centeredY, [self.text cStringUsingEncoding:NSUTF8StringEncoding ], [self.text length]);

}

*/

@end
