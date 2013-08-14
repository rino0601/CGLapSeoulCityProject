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
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
