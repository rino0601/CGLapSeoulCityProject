//
//  ColorPickerView.m
//  TimidTiger
//
//  Created by Hanjong Ko on 13. 7. 4..
//
//

#import "ColorPickerView.h"

@implementation ColorPickerView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
		self.backgroundColor = [UIColor redColor];
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    
}


@end
