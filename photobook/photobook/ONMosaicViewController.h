//
//  ONMosaicViewController.h
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "../PaperMosaic/PaperMosaicCanvas.h"

@interface ONMosaicViewController : UIViewController {
    CGContextRef ctx;
	ActiveXDialog *ADelegate;
	UIImageView *Mosaic;
	UIImageView *Paper;
	CGPoint touchPos;
}

- (void)doAsAutoMode; //작동을 왜 안하니 ㅠㅠ
- (BOOL)setPaperColor:(UIColor *)color;
- (id)initWithFrame:(CGRect)frame scale:(CGSize)factor;

@end
