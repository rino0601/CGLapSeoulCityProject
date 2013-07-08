#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <QuartzCore/QuartzCore.h>
#import <MediaPlayer/MediaPlayer.h>
#import <MessageUI/MessageUI.h>
#import <MessageUI/MFMailComposeViewController.h>

@class ContentsView;

@interface MainViewController : UIViewController <MFMailComposeViewControllerDelegate> {
	CGFloat scaleFactorX;
	CGFloat scaleFactorY;
}

@end
