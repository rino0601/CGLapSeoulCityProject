#import <UIKit/UIKit.h>
#import "MainViewController.h"
@interface TimidTigerAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
}

@property (nonatomic, strong) IBOutlet UIWindow *window;

@property (nonatomic, strong) UINavigationController *navigationViewController;

@property (nonatomic, strong) MainViewController *mainView;

@end

