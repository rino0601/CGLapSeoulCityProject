#import "MainViewController.h"
#import "ContentsView.h"
#import "PaperMosaicViewController.h"

@interface MainViewController ()

@property(strong, nonatomic) AVAudioPlayer *bgmPlayer;
@property(strong, nonatomic) MPMoviePlayerController *logoPlayer;
@property(strong, nonatomic) ContentsView *contentsView;
@property(strong, nonatomic) UIImageView *subtitleCheckImageView;
@property(strong, nonatomic) UIImageView *pageflipCheckImageView;

- (void)videoView:(id)obj;

- (void)viewSetting;
- (void)viewBefore;
- (void)viewMoreBooks;
- (void)openMail;

- (void)backToHome;

- (void)loadMainMenu;
- (void)bgmPlay;

- (void)setPlistFile;

- (void)subtitleView;
- (void)subtitleHide;
- (void)pageflipAuto;
- (void)pageflipManual;
- (void)runOtherBook:(UIButton *)btn;

@end

@implementation MainViewController

#ifndef ScaleCGRectMake
#define ScaleCGRectMake(x,y,w,h) CGRectMake(((x)*scaleFactorX),((y)*scaleFactorY),((w)*scaleFactorX),((h)*scaleFactorY))
#endif

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
		scaleFactorX = self.view.frame.size.width / 480.0f;
		scaleFactorY = self.view.frame.size.height / 320.0f;
		
		NSBundle *bundle = [NSBundle mainBundle];
		NSString *videoPath = [bundle pathForResource:@"logo" ofType:@"mp4"];
		
		NSURL *logoURL = [NSURL fileURLWithPath:videoPath];
		_logoPlayer = [[MPMoviePlayerController alloc] initWithContentURL:logoURL];
		_logoPlayer.view.frame = self.view.frame;
		_logoPlayer.controlStyle = MPMovieControlStyleNone;
		_logoPlayer.view.backgroundColor = [UIColor whiteColor];
		_logoPlayer.backgroundView.backgroundColor = [UIColor whiteColor];
		_logoPlayer.fullscreen = YES;
		_logoPlayer.view.userInteractionEnabled = YES;
		_logoPlayer.movieSourceType = MPMovieSourceTypeFile;
		
		[self.view addSubview:_logoPlayer.view];
		 
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(mediaPlayerDidFinish:) name:MPMoviePlayerPlaybackDidFinishNotification object:_logoPlayer];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(openMail) name:@"openMail" object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(paperMosaicView:) name:@"PaperMosaic" object:nil];
		
		
		[_logoPlayer play];
	}
	
	[self setPlistFile];
	
    return self;
}



- (void)setPlistFile {
	NSString *pathStr = [[NSBundle mainBundle] bundlePath];
	NSString *finalPath = [pathStr stringByAppendingPathComponent:@"settings.plist"];
	NSDictionary *plistDefault = [NSDictionary dictionaryWithContentsOfFile:finalPath];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults registerDefaults:plistDefault];
	[defaults synchronize];
}


- (void)bgmPlay {
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"bgmPlay" object:nil];
	
	NSString *bgmFile = [[NSBundle mainBundle] pathForResource:@"bgm" ofType:@"mp3"];
	NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:bgmFile];
	
	_bgmPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:nil];
	
	_bgmPlayer.numberOfLoops = -1;
	
	[_bgmPlayer play];
}


- (void)openMail {
	NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
		
	NSString *mailBody = [NSString stringWithFormat:@"%@ \n",[setting stringForKey:@"bookTitle"]];
	NSArray *toRecipients = [[NSArray alloc] initWithObjects:@"adgun@genicube.co.kr",nil];
	
	MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
	picker.mailComposeDelegate = self;
	[picker setToRecipients:toRecipients];
	[picker setSubject:@"To GENI CUBE "];
	[picker setMessageBody:mailBody isHTML:YES];
	
	[self presentViewController:picker animated:YES completion:nil];
	
}


- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error {
	// Notifies users about errors associated with the interface

	UIAlertView *alert;
	
	switch (result) {
		case MFMailComposeResultCancelled:			
			break;
		case MFMailComposeResultSaved:
			break;
		case MFMailComposeResultSent:
			break;
		case MFMailComposeResultFailed:
			break;
		default:
			alert = [[UIAlertView alloc] initWithTitle:@"Email" message:@"메일 전송이 실패하였습니다."
														   delegate:self cancelButtonTitle:@"OK" otherButtonTitles: nil];
			[alert show];
			break;
	}
	[self dismissViewControllerAnimated:YES completion:nil];
}	



- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	[[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:_logoPlayer];
	
	if (_logoPlayer != nil) {
		[_logoPlayer pause];
		[self loadMainMenu];
	}
}


- (void)mediaPlayerDidFinish:(NSNotification*)aNotification {
	if (_logoPlayer != nil) {
		[_logoPlayer pause];
		[self loadMainMenu];
	}
}


- (void)loadMainMenu {

	[self bgmPlay];
	
	UIImageView *backImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"bg.png"]];
	backImageView.frame = self.view.frame;
	backImageView.userInteractionEnabled = YES;
	
	
	UIImageView *logoImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(26, 14, 101, 30)];
	logoImageView.image = [UIImage imageNamed:@"logo.png"]; 
	
	[backImageView addSubview:logoImageView];
	
	
	UIImageView *titleImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(217, 61, 250, 76)];
	titleImageView.image = [UIImage imageNamed:@"title.png"]; 
	
	[backImageView addSubview:titleImageView];
	
	
	UIImageView *copyrightImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(111, 291, 255, 10)];
	copyrightImageView.image = [UIImage imageNamed:@"copyright.png"]; 
	
	[backImageView addSubview:copyrightImageView];
	
	
	UIImageView *bookImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(13, 39, 208, 240)];
	bookImageView.image = [UIImage imageNamed:@"book.png"]; 
	
	[backImageView addSubview:bookImageView];
	
	
	UIButton *btnSetting = [UIButton buttonWithType:UIButtonTypeCustom];
	btnSetting.frame = ScaleCGRectMake(426, 10, 42, 40);
	
	[btnSetting setBackgroundImage:[UIImage imageNamed:@"btn_setup.png"] forState:UIControlStateNormal];
	[btnSetting addTarget:self action:@selector(viewSetting) forControlEvents:UIControlEventTouchUpInside];
	
	[backImageView addSubview:btnSetting];
	
	
	UIButton *btnMoreBooks = [UIButton buttonWithType:UIButtonTypeCustom];
	btnMoreBooks.frame = ScaleCGRectMake(406, 266, 63, 40);
	
	[btnMoreBooks setBackgroundImage:[UIImage imageNamed:@"main_btn_more.png"] forState:UIControlStateNormal];
	[btnMoreBooks addTarget:self action:@selector(viewMoreBooks) forControlEvents:UIControlEventTouchUpInside];
	
	[backImageView addSubview:btnMoreBooks];
	
	
	UIButton *btnBefore = [UIButton buttonWithType:UIButtonTypeCustom];
	btnBefore.frame = ScaleCGRectMake(284, 146, 140, 46);
	
	[btnBefore setBackgroundImage:[UIImage imageNamed:@"btn_beforeread.png"] forState:UIControlStateNormal];
	[btnBefore addTarget:self action:@selector(viewBefore) forControlEvents:UIControlEventTouchUpInside];
	
	[backImageView addSubview:btnBefore];
	
	
	UIButton *btnRead = [UIButton buttonWithType:UIButtonTypeCustom];
	btnRead.frame = ScaleCGRectMake(299, 202, 110, 51);
	
	[btnRead setBackgroundImage:[UIImage imageNamed:@"btn_read.png"] forState:UIControlStateNormal];
	[btnRead addTarget:self action:@selector(videoView:) forControlEvents:UIControlEventTouchUpInside];
	
	[backImageView addSubview:btnRead];
	
	
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlDown forView:self.view cache:YES];
	
	[UIView setAnimationDelegate:self];
	
	[self.view addSubview:backImageView];

	[UIView commitAnimations];

	_logoPlayer = nil;
	
}



- (void)viewMoreBooks {
	UIImageView *moreBooksImageView = [[UIImageView alloc] initWithFrame:self.view.frame];
	moreBooksImageView.image = [UIImage imageNamed:@"bg.png"]; 
	moreBooksImageView.userInteractionEnabled = YES;
	
	UIImageView *moreTitleImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(56, 0, 368, 89)];
	moreTitleImageView.image = [UIImage imageNamed:@"title_morebooks.png"]; 
	moreTitleImageView.backgroundColor = [UIColor clearColor];
	moreTitleImageView.userInteractionEnabled = YES;
	[moreBooksImageView addSubview:moreTitleImageView];
	
	
	UIImageView *bigLogoImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(20, 100, 440, 73)];
	bigLogoImageView.image = [UIImage imageNamed:@"morebooktext.png"]; 
	bigLogoImageView.backgroundColor = [UIColor clearColor];
	bigLogoImageView.userInteractionEnabled = YES;
	[moreBooksImageView addSubview:bigLogoImageView];
	
	
	UIImageView *comingsoonBackImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(49, 187, 388, 129)];
	comingsoonBackImageView.image = [UIImage imageNamed:@"end_bottom_bg.png"]; 
	comingsoonBackImageView.backgroundColor = [UIColor clearColor];
	comingsoonBackImageView.userInteractionEnabled = YES;
	[moreBooksImageView addSubview:comingsoonBackImageView];
	
	
	NSUserDefaults *moreBookSet = [NSUserDefaults standardUserDefaults];
	
	UIButton *btnBook1 = [UIButton buttonWithType:UIButtonTypeCustom];
	btnBook1.frame = ScaleCGRectMake(75, 239, 53, 63);
	
	[btnBook1 setBackgroundImage:[UIImage imageNamed:[moreBookSet stringForKey:@"moreBook1Name"]] forState:UIControlStateNormal];
	[btnBook1 addTarget:self action:@selector(runOtherBook:) forControlEvents:UIControlEventTouchUpInside];
	btnBook1.tag = 1;
	
	[moreBooksImageView addSubview:btnBook1];
	
	
	UIButton *btnBook2 = [UIButton buttonWithType:UIButtonTypeCustom];
	btnBook2.frame = ScaleCGRectMake(252, 239, 53, 63);
	
	[btnBook2 setBackgroundImage:[UIImage imageNamed:[moreBookSet stringForKey:@"moreBook2Name"]] forState:UIControlStateNormal];
	[btnBook2 addTarget:self action:@selector(runOtherBook:) forControlEvents:UIControlEventTouchUpInside];
	btnBook2.tag = 2;
	
	[moreBooksImageView addSubview:btnBook2];		
	
	
	UILabel *moreLabel = [[UILabel alloc] initWithFrame:ScaleCGRectMake(90, 200, 300, 30)];
	moreLabel.font = [UIFont boldSystemFontOfSize:20];
	moreLabel.text = @"More GENI CUBE Stories";
	moreLabel.backgroundColor = [UIColor clearColor];
	moreLabel.textColor = [UIColor lightGrayColor];
	moreLabel.textAlignment = NSTextAlignmentCenter;
	
	[moreBooksImageView addSubview:moreLabel];
	
	
	UILabel *book1Label = [[UILabel alloc] initWithFrame:ScaleCGRectMake(120, 230, 120, 80)];
	book1Label.font = [UIFont systemFontOfSize:12];
	book1Label.text = [moreBookSet stringForKey:@"moreBook1Label"];
	book1Label.backgroundColor = [UIColor clearColor];
	book1Label.textColor = [UIColor lightGrayColor];
	book1Label.textAlignment = NSTextAlignmentCenter;
	book1Label.numberOfLines = 2;
	
	[moreBooksImageView addSubview:book1Label];
	
	
	UILabel *book2Label = [[UILabel alloc] initWithFrame:ScaleCGRectMake(300, 230, 120, 80)];
	book2Label.font = [UIFont systemFontOfSize:12];
	book2Label.text = [moreBookSet stringForKey:@"moreBook2Label"];
	book2Label.backgroundColor = [UIColor clearColor];
	book2Label.textColor = [UIColor lightGrayColor];
	book2Label.textAlignment = NSTextAlignmentCenter;
	book2Label.numberOfLines = 2;
	
	[moreBooksImageView addSubview:book2Label];		
	
	
	
	if (![moreBookSet boolForKey:@"moreBook1Enable"]) {
		UIImageView *comingsoonImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(103, 225, 281, 56)];
		comingsoonImageView.image = [UIImage imageNamed:@"img_comingsoon.png"]; 
		comingsoonImageView.backgroundColor = [UIColor clearColor];
		comingsoonImageView.userInteractionEnabled = YES;
		[moreBooksImageView addSubview:comingsoonImageView];		
		
	} else if (![moreBookSet boolForKey:@"moreBook2Enable"]) {
		UIImageView *comingsoonImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(270, 230, 127, 71)];
		comingsoonImageView.image = [UIImage imageNamed:@"img_comingsoon2.png"]; 
		comingsoonImageView.backgroundColor = [UIColor clearColor];
		comingsoonImageView.userInteractionEnabled = YES;
		[moreBooksImageView addSubview:comingsoonImageView];		
	}
	
	UIButton *btnHome = [UIButton buttonWithType:UIButtonTypeCustom];
	btnHome.frame = ScaleCGRectMake(16, 17, 40, 35);
	
	[btnHome setBackgroundImage:[UIImage imageNamed:@"btn_home.png"] forState:UIControlStateNormal];
	[btnHome addTarget:self action:@selector(backToHome) forControlEvents:UIControlEventTouchUpInside];
	
	[moreBooksImageView addSubview:btnHome];
	
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlDown forView:self.view cache:YES];
	
	[UIView setAnimationDelegate:self];

	[self.view addSubview:moreBooksImageView];
	
    [UIView commitAnimations];		
	
}


- (void)runOtherBook:(UIButton *)btn {
	NSUserDefaults *moreBookSet = [NSUserDefaults standardUserDefaults];
		
	if ([moreBookSet boolForKey:[NSString stringWithFormat:@"moreBook%dEnable",btn.tag]]) {
		NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@://",[moreBookSet stringForKey:[NSString stringWithFormat:@"moreBook%dName",btn.tag]]]];

		if(! [[UIApplication sharedApplication] openURL:url]) {
			NSURL *url2 = [NSURL URLWithString:[NSString stringWithFormat:@"http://itunes.apple.com/app/id%@?mt=8",[moreBookSet stringForKey:[NSString stringWithFormat:@"moreBook%dID",btn.tag]]]];

			[[UIApplication sharedApplication] openURL:url2];
		}
	}	
}


- (void)viewSetting {
	UIImageView *settingImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(0, 0, 480, 320)];
	
	settingImageView.image = [UIImage imageNamed:@"set_bg.png"]; 
	settingImageView.userInteractionEnabled = YES;
	
	UIButton *btnHome = [UIButton buttonWithType:UIButtonTypeCustom];
	btnHome.frame = ScaleCGRectMake(16, 17, 40, 35);
	
	[btnHome setBackgroundImage:[UIImage imageNamed:@"btn_home.png"] forState:UIControlStateNormal];
	[btnHome addTarget:self action:@selector(backToHome) forControlEvents:UIControlEventTouchUpInside];
	
	[settingImageView addSubview:btnHome];
	
	
	
	UIButton *btnSubView = [UIButton buttonWithType:UIButtonTypeCustom];
	btnSubView.frame = ScaleCGRectMake(243, 134, 77, 33);
	
	[btnSubView setBackgroundImage:[UIImage imageNamed:@"btn_view_off.png"] forState:UIControlStateNormal];
	[btnSubView addTarget:self action:@selector(subtitleView) forControlEvents:UIControlEventTouchUpInside];
	
	[settingImageView addSubview:btnSubView];
	
	
	
	UIButton *btnSubHide = [UIButton buttonWithType:UIButtonTypeCustom];
	btnSubHide.frame = ScaleCGRectMake(345, 134, 71, 33);
	
	[btnSubHide setBackgroundImage:[UIImage imageNamed:@"btn_hide_off.png"] forState:UIControlStateNormal];
	[btnSubHide addTarget:self action:@selector(subtitleHide) forControlEvents:UIControlEventTouchUpInside];
	
	[settingImageView addSubview:btnSubHide];
	

	
	UIButton *btnPageAuto = [UIButton buttonWithType:UIButtonTypeCustom];
	btnPageAuto.frame = ScaleCGRectMake(244, 204, 74, 33);
	
	[btnPageAuto setBackgroundImage:[UIImage imageNamed:@"btn_auto_off.png"] forState:UIControlStateNormal];
	[btnPageAuto addTarget:self action:@selector(pageflipAuto) forControlEvents:UIControlEventTouchUpInside];
	
	[settingImageView addSubview:btnPageAuto];
	
	
	UIButton *btnPageManual = [UIButton buttonWithType:UIButtonTypeCustom];
	btnPageManual.frame = ScaleCGRectMake(330, 204, 105, 33);
	
	[btnPageManual setBackgroundImage:[UIImage imageNamed:@"btn_manual_off.png"] forState:UIControlStateNormal];
	[btnPageManual addTarget:self action:@selector(pageflipManual) forControlEvents:UIControlEventTouchUpInside];
	
	[settingImageView addSubview:btnPageManual];
	

	
	_subtitleCheckImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"mark_check.png"]];
	
	[settingImageView addSubview:_subtitleCheckImageView];
	
	_pageflipCheckImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"mark_check.png"]];
	
	[settingImageView addSubview:_pageflipCheckImageView];
	
	
	NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
	
	if ([setting boolForKey:@"togSubtitle"]) {	
		[self subtitleView];
	} else {
		[self subtitleHide];
	}
	
	
	if ([setting boolForKey:@"togPageFlip"]) {	
		[self pageflipAuto];
	} else {
		[self pageflipManual];
	}
	
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlDown forView:self.view cache:YES];
	
	[UIView setAnimationDelegate:self];
	
	[self.view addSubview:settingImageView];
	

    [UIView commitAnimations];		
	
}


- (void)subtitleView {
	NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
	
	[setting setBool:YES forKey:@"togSubtitle"];
	
	_subtitleCheckImageView.frame = ScaleCGRectMake(268, 95, 41, 41);
}



- (void)subtitleHide {
	NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
	
	[setting setBool:NO forKey:@"togSubtitle"];
	
	_subtitleCheckImageView.frame = ScaleCGRectMake(357, 95, 41, 41);
}



- (void)pageflipAuto {
	NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
	
	[setting setBool:YES forKey:@"togPageFlip"];
	
	_pageflipCheckImageView.frame = ScaleCGRectMake(268, 165, 41, 41);
}


- (void)pageflipManual {
	NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
	
	[setting setBool:NO forKey:@"togPageFlip"];
	
	_pageflipCheckImageView.frame = ScaleCGRectMake(357, 165, 41, 41);
}



- (void)viewBefore {
	UIImageView *beforeBackImageView = [[UIImageView alloc] initWithFrame:self.view.frame];
	
	beforeBackImageView.image = [UIImage imageNamed:@"bg.png"]; 
	beforeBackImageView.userInteractionEnabled = YES;
	
	UIScrollView *beforeScrollView = [[UIScrollView alloc] initWithFrame:self.view.frame];
	UIImageView *beforeImageView = [[UIImageView alloc] init];
	UIImage *beforeImage = [UIImage imageNamed:@"before_reading.png"];
	
	beforeImageView.frame = ScaleCGRectMake(0, 0, self.view.frame.size.width, beforeImage.size.height);
	beforeImageView.image = beforeImage;

	beforeImageView.userInteractionEnabled = YES;
	beforeImageView.contentMode = UIViewContentModeScaleAspectFit;
	
	beforeScrollView.contentSize = beforeImageView.frame.size;	
	beforeScrollView.showsVerticalScrollIndicator = NO;

	[beforeScrollView addSubview:beforeImageView];
	
	
	
	[beforeBackImageView addSubview:beforeScrollView];
	

	
	UIImageView *titleImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(58, 0, 368, 89)];
	titleImageView.image = [UIImage imageNamed:@"title_before.png"];
	
	[beforeBackImageView addSubview:titleImageView];
	
	
	
	
	UIButton *btnHome = [UIButton buttonWithType:UIButtonTypeCustom];
	btnHome.frame = ScaleCGRectMake(16, 17, 40, 35);
	
	[btnHome setBackgroundImage:[UIImage imageNamed:@"btn_home.png"] forState:UIControlStateNormal];
	[btnHome addTarget:self action:@selector(backToHome) forControlEvents:UIControlEventTouchUpInside];
	
	[beforeBackImageView addSubview:btnHome];
	
	
	
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlDown forView:self.view cache:YES];
	
	[UIView setAnimationDelegate:self];
	
	[self.view addSubview:beforeBackImageView];
	
    [UIView commitAnimations];		
	
}



- (void)backToHome {
	NSInteger cntSubview = [self.view.subviews count];
	UIView *topView = (UIView *)[self.view.subviews objectAtIndex:cntSubview - 1];
	
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlUp forView:self.view cache:YES];
	
	[UIView setAnimationDelegate:self];
	
	[topView removeFromSuperview];
	
    [UIView commitAnimations];		
}





- (void)videoView:(id)obj {
	[_bgmPlayer stop];

	_contentsView = [[ContentsView alloc] initWithFrame:self.view.bounds];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(bgmPlay) name:@"bgmPlay" object:nil];
	
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlDown forView:self.view cache:NO];

	[self.view addSubview:_contentsView];

    [UIView commitAnimations];		
}

-(void)paperMosaicView:(NSNotification *)notify {
	PaperMosaicViewController *paper = [[PaperMosaicViewController alloc] init];
	[self presentViewController:paper animated:YES completion:nil];
}






- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}



@end
