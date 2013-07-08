//
//  PaperMosaicViewController.m
//  TimidTiger
//
//  Created by Hanjong Ko on 13. 7. 3..
//
//

#import "PaperMosaicViewController.h"
#import "PaperMosaicCanvas.h"

@interface PaperMosaicViewController ()
@property CGFloat scaleFactorX;
@property CGFloat scaleFactorY;
@end

#ifndef ScaleCGRectMake
#define ScaleCGRectMake(x,y,w,h) CGRectMake(((x)*_scaleFactorX),\
((y)*_scaleFactorY),((w)*_scaleFactorX),((h)*_scaleFactorY))
#endif

@implementation PaperMosaicViewController

- (id)init {
	if (self = [super init]) {
		CGRect appFrame = [[UIScreen mainScreen] applicationFrame];
		UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
		if (UIInterfaceOrientationLandscapeLeft == orientation ||
			UIInterfaceOrientationLandscapeRight == orientation)
		{
            if (appFrame.size.width < appFrame.size.height)
			{
                appFrame = CGRectMake(appFrame.origin.y, appFrame.origin.x, appFrame.size.height, appFrame.size.width);
			}
        }
		self.view.frame = appFrame;
		_scaleFactorX = self.view.frame.size.width / 480.0f;
		_scaleFactorY = self.view.frame.size.height / 320.0f;
		CGSize factor;
		factor.width = _scaleFactorX;
		factor.height= _scaleFactorY;
		
		UIImageView *backImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"bg.png"]];
		backImageView.frame = self.view.frame;
		backImageView.userInteractionEnabled = YES;
		
		UIImageView *logoImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(26, 14, 101, 30)];
		logoImageView.image = [UIImage imageNamed:@"logo.png"];
		[backImageView addSubview:logoImageView];
		
		UIButton *btnSetting = [UIButton buttonWithType:UIButtonTypeCustom];
		btnSetting.frame = ScaleCGRectMake(426, 10, 42, 40);
		
		[btnSetting setBackgroundImage:[UIImage imageNamed:@"btn_back.png"] forState:UIControlStateNormal];
		[btnSetting addTarget:self action:@selector(backTOMainApp:) forControlEvents:UIControlEventTouchUpInside];
		[backImageView addSubview:btnSetting];
		
		PaperMosaicCanvas *mosaicView = [[PaperMosaicCanvas alloc] initWithFrame:ScaleCGRectMake(20, 60, 440, 240)scale:factor];
		[backImageView addSubview:mosaicView];
		
		[self.view addSubview:backImageView];
	}
	return self;
}

- (void)backTOMainApp:(UIButton *)sender {
	[self dismissViewControllerAnimated:YES completion:nil];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
