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
	CGPoint touchPos;
}
@property(nonatomic) ActiveXDialog *ADelegate;
@property(strong,nonatomic) IBOutlet UIImageView *Mosaic;
@property(strong,nonatomic) IBOutlet UIImageView *Paper;

- (void)doAsAutoMode;
- (BOOL)setPaperColor:(UIColor *)color;

@end

class CPaperMosaicCanvas {
private:
	PaperMosaicCanvas *instance;
public:
	CPaperMosaicCanvas(PaperMosaicCanvas *canvas){instance=canvas;}
	void setTimerWithTimeInterval(NSTimeInterval second);
	void killAlert();
};