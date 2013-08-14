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

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void) drawTextInRect:(CGRect)rect
{
    CGRect bounds = [self bounds];
    const char *result = [self.text UTF8String];
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSelectFont(context, "KoPubBatangBold", self.font.pointSize, kCGEncodingMacRoman);
    CGContextSetTextDrawingMode(context, kCGTextFill);
    CGContextSetRGBFillColor(context, 1, 0, 0, 1);
 
    CGContextSetCharacterSpacing(context, -25); // <-----파라미터를 수정해주시면 됩니다.
    CGContextTranslateCTM(context, 0, bounds.size.height + 5);
    CGContextScaleCTM(context, 1, -1);
    CGContextShowTextAtPoint(context, 0, rect.size.height/2, result, strlen(result));
}



@end
