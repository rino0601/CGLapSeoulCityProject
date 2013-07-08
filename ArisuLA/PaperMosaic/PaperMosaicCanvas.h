//
//  PaperMosaicCanvas.h
//  PaperMosaic
//
//  Created by Hanjong Ko on 13. 1. 11..
//  Copyright (c) 2013년 lemonApple. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "InteractiveColoredPaperMosaic.h"
#import "cpp/ActiveXDialog.h"

@interface PaperMosaicCanvas : UIView<UIAlertViewDelegate> {
@private
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

class CPaperMosaicCanvas {
private:
	PaperMosaicCanvas *instance;
public:
	CPaperMosaicCanvas(PaperMosaicCanvas *canvas){instance=canvas;}
	void setTimerWithTimeInterval(NSTimeInterval second);
	void killAlert();
};