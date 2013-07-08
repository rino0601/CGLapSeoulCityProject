#import "ContentsView.h"
#import "MenuView.h"

@interface ContentsView ()
- (void)openMail;

- (void)openTwitter;
- (void)closeTwitter;

- (void)removeContentsView;
- (void)controlMenuView:(BOOL)menuOn;
- (void)runOtherBook:(UIButton *)btn;
- (void)playPageEffect;
- (void)checkWebLoad:(NSTimer *)timer;

- (void)subviewChange;

- (void)playLive:(NSString *)moviePath;
- (void)videoPlay;
- (void)nextPlay;
- (void)prevPlay;
- (void)backToHome2;
- (void)playFirst;
@end

@implementation ContentsView

#define PLAYER_WAIT_TIME 0.2
#ifndef ScaleCGRectMake
#define ScaleCGRectMake(x,y,w,h) CGRectMake(((x)*scaleFactorX),((y)*scaleFactorY),((w)*scaleFactorX),((h)*scaleFactorY))
#endif

- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
		NSUserDefaults *defaultsValues = [NSUserDefaults standardUserDefaults];
		scaleFactorX = frame.size.width / 480.0f;
		scaleFactorY = frame.size.height / 320.0f;
		playCount = [defaultsValues integerForKey:@"readPage"] - 1;
		
		videoList = [[NSArray alloc] initWithArray:[defaultsValues arrayForKey:@"videoList"]];
		
		backImageView = [[UIImageView alloc] initWithFrame:frame];
		
		backImageView.backgroundColor = [UIColor clearColor];
		backImageView.image = [UIImage imageNamed:[NSString stringWithFormat:@"%@.png",[videoList objectAtIndex:playCount]]];
		
		movPlayer1 = [[MPMoviePlayerController alloc] init];
		movPlayer1.view.frame = frame;
		movPlayer1.controlStyle = MPMovieControlStyleNone;
		movPlayer1.fullscreen = YES;
		movPlayer1.view.backgroundColor = [UIColor clearColor];
		movPlayer1.view.userInteractionEnabled = YES;
		movPlayer1.backgroundView.backgroundColor = [UIColor clearColor];
		
		[self addSubview:backImageView];
		
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(pausePlay) name:@"pausePlay" object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(backToHome2) name:@"backToHome2" object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(rePlay) name:@"rePlay" object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(videoPlay) name:@"togSubtitle" object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(playFirst) name:@"gotoFirst" object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(mediaPlayerDidFinish:) name:MPMoviePlayerPlaybackDidFinishNotification object:movPlayer1];
		
		[self addSubview:movPlayer1.view];
		
		menuView = [[MenuView alloc] initWithFrame:frame];
		menuView.isPlay=YES;
		[menuView setHidden:YES];
		
		[movPlayer1.view addSubview:menuView];
		
		[self videoPlay];
    }
	
    return self;
}


- (void)openTwitter {
	NSString *twitter = @"http://m.twitter.com/genicube";
	
	twitterWebView = [[UIWebView alloc] initWithFrame:self.bounds];
	
	actIndicator = [[UIActivityIndicatorView alloc] initWithFrame:ScaleCGRectMake(0, 0, 32, 32)];
	actIndicator.center = CGPointMake(240, 160);
	
	actIndicator.activityIndicatorViewStyle = UIActivityIndicatorViewStyleGray;
	[actIndicator startAnimating];
	
	[twitterWebView addSubview:actIndicator];
	
	
	[NSTimer scheduledTimerWithTimeInterval:0.3 target:self selector:@selector(checkWebLoad:) userInfo:nil repeats:YES];
	
	UIButton *btnBack = [UIButton buttonWithType:UIButtonTypeCustom];
	btnBack.frame = ScaleCGRectMake(16, 17, 40, 35);
	
	[btnBack setBackgroundImage:[UIImage imageNamed:@"btn_back.png"] forState:UIControlStateNormal];
	[btnBack addTarget:self action:@selector(closeTwitter) forControlEvents:UIControlEventTouchUpInside];
	
	[twitterWebView addSubview:btnBack];
	
	twitterWebView.contentMode = UIViewContentModeScaleToFill;
	
	NSURL *url = [[NSURL alloc] initWithString:twitter];
	
	NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
    
	[twitterWebView loadRequest:requestObj];
	
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlDown forView:self cache:YES];
	
	[UIView setAnimationDelegate:self];
	
	[self addSubview:twitterWebView];
	
    [UIView commitAnimations];
	
}


- (void)checkWebLoad:(NSTimer *)timer {
	if (! twitterWebView.loading) {
		[actIndicator removeFromSuperview];
		[timer invalidate];
	}
}


- (void)closeTwitter {
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlUp forView:self cache:YES];
	
	[UIView setAnimationDelegate:self];
	
	[twitterWebView removeFromSuperview];
	
    [UIView commitAnimations];
}


- (void)openMail {
	[[NSNotificationCenter defaultCenter] postNotificationName:@"openMail" object:nil];
}


- (void)controlMenuView:(BOOL)menuOn {
	if(menuOn){
		[menuView setHidden:![menuView isHidden]];
		menuView.isPlay?[self pausePlay]:[self rePlay];
		menuView.isPlay=!menuView.isPlay;
	} else {
		[menuView setHidden:YES];
		menuView.isPlay=YES;
	}
}


- (void)playFirst {
	[endPage removeFromSuperview];
	
	endPage = nil;
	
	playCount = -1;
	[self nextPlay];
	
	[self controlMenuView:NO];
}


- (void)backToHome2 {
	[[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:movPlayer1];
	
	[movPlayer1 pause];
	
	[menuView setHidden:YES];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:@"bgmPlay" object:nil];
	
	[UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlUp forView:self cache:YES];
	
	[UIView setAnimationDelegate:self];
	
	if (endPage != nil) {
		[endPage removeFromSuperview];
		endPage = nil;
	}
	
	[movPlayer1.view removeFromSuperview];
	[backImageView removeFromSuperview];
	
	[UIView setAnimationDidStopSelector:@selector(removeContentsView)];
	
    [UIView commitAnimations];
}

- (void)removeContentsView {
	movPlayer1 = nil;
	
	[self removeFromSuperview];
}



- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	CGPoint pt = [[touches anyObject] locationInView:self];
	startPos.x = pt.x;
	startPos.y = pt.y;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	CGPoint pt = [[touches anyObject] locationInView:self];
	if (!isAnimate) {
		if (pt.x < startPos.x - 50) {
			if( playCount + 1 == [videoList count]){
				[self controlMenuView:NO];
				[movPlayer1 stop];
				return;
			}
			if(playCount + 1 < [videoList count]) {
				[self controlMenuView:NO];
				[self nextPlay];
				return;
			}
		} else if (startPos.x + 50 < pt.x) {
			if(playCount==0)
				return;
			[self controlMenuView:NO];
			[self prevPlay];
			return;
		} else {
			[self controlMenuView:YES];
			return;
		}
	}
}


- (void)videoPlay {
	isAnimate = NO;
	
	[movPlayer1 stop];
	[menuView.btnPlay setBackgroundImage:[UIImage imageNamed:@"btn_pause.png"] forState:UIControlStateNormal];
	
	if (! endPage.hidden) {
		endPage.hidden = YES;
	}
	
	if (naviView != nil) {
		[naviView removeFromSuperview];
	}
	
	if (playCount == [videoList count]) {
		playCount = 0;
	}
	
	if (playCount < 0 ) {
		playCount = [videoList count] - 1;
	}
	
	NSBundle *bundle = [NSBundle mainBundle];
	NSString *videoPath;
	
	NSUserDefaults *defaultsValues = [NSUserDefaults standardUserDefaults];
	
	if ([defaultsValues boolForKey:@"togSubtitle"]) {
		videoPath = [bundle pathForResource:[NSString stringWithFormat:@"%@st",[videoList objectAtIndex:playCount]] ofType:@"mp4"];
		
		[defaultsValues setBool:YES forKey:@"togSubtitle"];
	} else {
		videoPath = [bundle pathForResource:[videoList objectAtIndex:playCount] ofType:@"mp4"];
		
		[defaultsValues setBool:NO forKey:@"togSubtitle"];
	}
	
	[self playLive:videoPath];
	
	[defaultsValues setInteger:playCount + 1 forKey:@"readPage"];
}




- (void)playLive:(NSString *)moviePath{
	[movPlayer1 setContentURL:[NSURL fileURLWithPath:moviePath]];
	
	autoPlay = YES;
    
	[movPlayer1 play];
}


- (void)pausePlay {
	[movPlayer1 pause];
}


- (void)rePlay {
	if (naviView != nil) {
		[naviView removeFromSuperview];
	}
	
	[movPlayer1 play];
}

- (void)playPageEffect {
	NSString *bgmFile = [[NSBundle mainBundle] pathForResource:@"pageEffect" ofType:@"mp3"];
	NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:bgmFile];
	
	AVAudioPlayer *pagePlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:nil];
    
	[pagePlayer play];
}


- (void)nextPlay {
	isAnimate = YES;
	
	NSInteger imageNextCount;
	
	imageNextCount = playCount + 1;
	
	if (imageNextCount == [videoList count]) {
		imageNextCount = 0;
	}
	
	backImageView.image = [UIImage imageNamed:[NSString stringWithFormat:@"%@",[videoList objectAtIndex:imageNextCount]]];
	
	[self playPageEffect];
	
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlUp forView:self cache:YES];
	
	[UIView setAnimationDelegate:self];
	[self exchangeSubviewAtIndex:0 withSubviewAtIndex:1];
	
	[UIView setAnimationDidStopSelector:@selector(nextPlay2)];
	
    [UIView commitAnimations];
}


- (void)nextPlay2 {
	autoPlay = NO;
	[movPlayer1 stop];
	
	playCount = ++playCount;
	
	[self performSelector:@selector(subviewChange) withObject:self afterDelay:PLAYER_WAIT_TIME];
	[self videoPlay];
}


- (void)prevPlay {
	isAnimate = YES;
	
	NSInteger imagePrevCount;
	
	imagePrevCount = playCount - 1;
	
	if (imagePrevCount < 0 ) {
		imagePrevCount = [videoList count] - 1;
	}
	
	backImageView.image = [UIImage imageNamed:[NSString stringWithFormat:@"%@.png",[videoList objectAtIndex:imagePrevCount]]];
	
	[self playPageEffect];
	
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1.0];
    
    [UIView setAnimationTransition:UIViewAnimationTransitionCurlDown forView:self cache:YES];
	
	[UIView setAnimationDelegate:self];
	[self exchangeSubviewAtIndex:0 withSubviewAtIndex:1];
	
	[UIView setAnimationDidStopSelector:@selector(prevPlay2)];
	
    [UIView commitAnimations];
}


- (void)prevPlay2 {
	playCount = --playCount;
	
	autoPlay = NO;
	[movPlayer1 stop];
	
	[self performSelector:@selector(subviewChange) withObject:self afterDelay:PLAYER_WAIT_TIME];
	
	[self videoPlay];
}


- (void)subviewChange {
	[self exchangeSubviewAtIndex:1 withSubviewAtIndex:0];
}


- (void)mediaPlayerDidFinish:(NSNotification*)aNotification {
	NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
	
	if (playCount + 1 == [videoList count] ) {
		[setting setInteger:1 forKey:@"readPage"];
		
		endPage = [[UIView alloc] initWithFrame:self.bounds];
		
		UIImageView *endImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(122, 120, 240, 56)];
		endImageView.image = [UIImage imageNamed:@"img_end.png"];
		endImageView.backgroundColor = [UIColor clearColor];
		endImageView.userInteractionEnabled = YES;
		[endPage addSubview:endImageView];
		
		
		UIImageView *comingsoonBackImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(49, 187, 388, 129)];
		comingsoonBackImageView.image = [UIImage imageNamed:@"end_bottom_bg.png"];
		comingsoonBackImageView.backgroundColor = [UIColor clearColor];
		comingsoonBackImageView.userInteractionEnabled = YES;
		[endPage addSubview:comingsoonBackImageView];
		
		
		NSUserDefaults *moreBookSet = [NSUserDefaults standardUserDefaults];
		
		UIButton *btnBook1 = [UIButton buttonWithType:UIButtonTypeCustom];
		btnBook1.frame = ScaleCGRectMake(75, 239, 53, 63);
		
		[btnBook1 setBackgroundImage:[UIImage imageNamed:[moreBookSet stringForKey:@"moreBook1Name"]] forState:UIControlStateNormal];
		[btnBook1 addTarget:self action:@selector(runOtherBook:) forControlEvents:UIControlEventTouchUpInside];
		btnBook1.tag = 1;
		
		[endPage addSubview:btnBook1];
		
		
		UIButton *btnBook2 = [UIButton buttonWithType:UIButtonTypeCustom];
		btnBook2.frame = ScaleCGRectMake(252, 239, 53, 63);
		
		[btnBook2 setBackgroundImage:[UIImage imageNamed:[moreBookSet stringForKey:@"moreBook2Name"]] forState:UIControlStateNormal];
		[btnBook2 addTarget:self action:@selector(runOtherBook:) forControlEvents:UIControlEventTouchUpInside];
		btnBook2.tag = 2;
		
		[endPage addSubview:btnBook2];
		
		
		
		UILabel *moreLabel = [[UILabel alloc] initWithFrame:ScaleCGRectMake(90, 200, 300, 30)];
		moreLabel.font = [UIFont boldSystemFontOfSize:20];
		moreLabel.text = @"More GENI CUBE Stories";
		moreLabel.backgroundColor = [UIColor clearColor];
		moreLabel.textColor = [UIColor lightGrayColor];
		moreLabel.textAlignment = NSTextAlignmentCenter;
		
		[endPage addSubview:moreLabel];
		
		
		UILabel *book1Label = [[UILabel alloc] initWithFrame:ScaleCGRectMake(120, 230, 120, 80)];
		book1Label.font = [UIFont systemFontOfSize:12];
		book1Label.text = [moreBookSet stringForKey:@"moreBook1Label"];
		book1Label.backgroundColor = [UIColor clearColor];
		book1Label.textColor = [UIColor lightGrayColor];
		book1Label.textAlignment = NSTextAlignmentCenter;
		book1Label.numberOfLines = 2;
		
		[endPage addSubview:book1Label];
		
		
		UILabel *book2Label = [[UILabel alloc] initWithFrame:ScaleCGRectMake(300, 230, 120, 80)];
		book2Label.font = [UIFont systemFontOfSize:12];
		book2Label.text = [moreBookSet stringForKey:@"moreBook2Label"];
		book2Label.backgroundColor = [UIColor clearColor];
		book2Label.textColor = [UIColor lightGrayColor];
		book2Label.textAlignment = NSTextAlignmentCenter;
		book2Label.numberOfLines = 2;
		
		[endPage addSubview:book2Label];
		
		
		if (![moreBookSet boolForKey:@"moreBook1Enable"]) {
			UIImageView *comingsoonImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(103, 225, 281, 56)];
			comingsoonImageView.image = [UIImage imageNamed:@"img_comingsoon.png"];
			comingsoonImageView.backgroundColor = [UIColor clearColor];
			comingsoonImageView.userInteractionEnabled = YES;
			[endPage addSubview:comingsoonImageView];
			
		} else if (![moreBookSet boolForKey:@"moreBook2Enable"]) {
			UIImageView *comingsoonImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(270, 230, 127, 71)];
			comingsoonImageView.image = [UIImage imageNamed:@"img_comingsoon2.png"];
			comingsoonImageView.backgroundColor = [UIColor clearColor];
			comingsoonImageView.userInteractionEnabled = YES;
			[endPage addSubview:comingsoonImageView];
		}
		
		
		UIButton *btnHome = [UIButton buttonWithType:UIButtonTypeCustom];
		btnHome.frame = ScaleCGRectMake(16, 17, 40, 35);
		
		[btnHome setBackgroundImage:[UIImage imageNamed:@"btn_home.png"] forState:UIControlStateNormal];
		[btnHome addTarget:self action:@selector(backToHome2) forControlEvents:UIControlEventTouchUpInside];
		
		[endPage addSubview:btnHome];
		
		
		UIButton *btnAgain = [UIButton buttonWithType:UIButtonTypeCustom];
		btnAgain.frame = ScaleCGRectMake(96, 8, 93, 93);
		
		[btnAgain setBackgroundImage:[UIImage imageNamed:@"btn_again.png"] forState:UIControlStateNormal];
		[btnAgain addTarget:self action:@selector(playFirst) forControlEvents:UIControlEventTouchUpInside];
		
		[endPage addSubview:btnAgain];
		
		
		UIButton *btnTwitter = [UIButton buttonWithType:UIButtonTypeCustom];
		btnTwitter.frame = ScaleCGRectMake(196, 8, 93, 93);
		
		[btnTwitter setBackgroundImage:[UIImage imageNamed:@"btn_twitter.png"] forState:UIControlStateNormal];
		[btnTwitter addTarget:self action:@selector(openTwitter) forControlEvents:UIControlEventTouchUpInside];
		
		[endPage addSubview:btnTwitter];
		
		
		UIButton *btnMail = [UIButton buttonWithType:UIButtonTypeCustom];
		btnMail.frame = ScaleCGRectMake(296, 8, 93, 93);
		
		[btnMail setBackgroundImage:[UIImage imageNamed:@"btn_mail.png"] forState:UIControlStateNormal];
		[btnMail addTarget:self action:@selector(openMail) forControlEvents:UIControlEventTouchUpInside];
		
		[endPage addSubview:btnMail];
		
		[UIView beginAnimations:nil context:NULL];
		[UIView setAnimationDuration:1.0];
		
		[UIView setAnimationTransition:UIViewAnimationTransitionCurlDown forView:self cache:YES];
		
		[UIView setAnimationDelegate:self];
		
		[self addSubview:endPage];
		
		[UIView commitAnimations];
		
		isAnimate = YES;
		
		
	} else {
		if ([setting boolForKey:@"togPageFlip"] && autoPlay && !menuView.changeSubtitle) {
			[self nextPlay];
		} else {
			[menuView.btnPlay setBackgroundImage:[UIImage imageNamed:@"btn_play.png"] forState:UIControlStateNormal];
			menuView.isPlay = YES;
			
			naviView = [[UIView alloc] initWithFrame:ScaleCGRectMake(0, 250, 480, 70)];
			naviView.userInteractionEnabled = YES;
			
			if (playCount != 0 && playCount + 1 != [videoList count]) {
				UIImageView *prevNaviImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(0, 0, 100, 70)];
				prevNaviImageView.image = [UIImage imageNamed:@"btn_prev.png"];
				prevNaviImageView.backgroundColor = [UIColor clearColor];
				prevNaviImageView.userInteractionEnabled = YES;
				[naviView addSubview:prevNaviImageView];
			}
			
			if (playCount + 1 != [videoList count]) {
				UIImageView *nextNaviImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(380, 0, 100, 70)];
				nextNaviImageView.image = [UIImage imageNamed:@"btn_next.png"];
				nextNaviImageView.backgroundColor = [UIColor clearColor];
				nextNaviImageView.userInteractionEnabled = YES;
				[naviView addSubview:nextNaviImageView];
			}
			
			[movPlayer1.view addSubview:naviView];
			
			menuView.changeSubtitle = NO;
		}
	}
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

- (void)didReceiveMemoryWarning {
	
}

- (void)viewDidUnload {
	
}



@end
