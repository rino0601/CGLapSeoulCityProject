#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AVFoundation/AVFoundation.h>

@class MenuView;

@interface ContentsView : UIView {
	CGPoint startPos;
	BOOL isAnimate;
	
	UIActivityIndicatorView *actIndicator;
	
	MenuView *menuView;
	
	UIImageView *backImageView;
    UIView *endPage;
	UIView *naviView;
	UIWebView *twitterWebView;
    
    UIButton *prevButton;
    UIButton *nextButton;
		
	NSArray *videoList;
	
	MPMoviePlayerController *movPlayer1;

	NSInteger playCount;

	BOOL autoPlay;
	
	CGFloat scaleFactorX ;
	CGFloat scaleFactorY ;
}

@end
