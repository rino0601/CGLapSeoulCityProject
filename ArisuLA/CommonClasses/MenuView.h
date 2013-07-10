#import <UIKit/UIKit.h>


@interface MenuView : UIView <UIScrollViewDelegate> {
	BOOL isPlay;
	BOOL menuOn;
	BOOL changeSubtitle;
	
	UIButton *btnSubtitle;
	UIButton *btnPlay;
	
	CGFloat scaleFactorX ;
	CGFloat scaleFactorY ;
}


@property(nonatomic, strong) UIButton *btnPlay;
@property(readwrite, assign) BOOL isPlay;
@property(readwrite, assign) BOOL changeSubtitle;


- (void)show:(BOOL)flag;
- (void)playPause:(id)obj;
- (void)backToHome;
- (void)toggleSubtitle;
- (void)firstPlay;

@end
