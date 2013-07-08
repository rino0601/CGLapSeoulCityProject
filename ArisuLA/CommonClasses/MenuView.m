#import "MenuView.h"
#import "ContentsView.h"

@implementation MenuView

@synthesize btnPlay, isPlay, changeSubtitle;

#ifndef ScaleCGRectMake
#define ScaleCGRectMake(x,y,w,h) CGRectMake(((x)*scaleFactorX),((y)*scaleFactorY),((w)*scaleFactorX),((h)*scaleFactorY))
#endif

- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
		self.frame = frame;
		self.backgroundColor = [UIColor clearColor];
		self.userInteractionEnabled = YES;
		
		scaleFactorX = frame.size.width / 480.0f;
		scaleFactorY = frame.size.height / 320.0f;
		
		UIImageView *menuImageView = [[UIImageView alloc] initWithFrame:ScaleCGRectMake(0, 320-67, 480, 67)];
		menuImageView.image = [UIImage imageNamed:@"menu_bg_r.png"];
		menuImageView.backgroundColor = [UIColor clearColor];
		menuImageView.userInteractionEnabled = YES;
		
		[self addSubview:menuImageView];
		
		UIButton *btnHome = [UIButton buttonWithType:UIButtonTypeCustom];
		btnHome.frame = ScaleCGRectMake(41, 13, 40, 35);
		
		[btnHome setBackgroundImage:[UIImage imageNamed:@"btn_home.png"] forState:UIControlStateNormal];
		[btnHome addTarget:self action:@selector(backToHome) forControlEvents:UIControlEventTouchUpInside];
		
		[menuImageView addSubview:btnHome];
		
		
		UIButton *btnPage = [UIButton buttonWithType:UIButtonTypeCustom];
		btnPage.frame = ScaleCGRectMake(154, 14, 53, 33);
		
		[btnPage setBackgroundImage:[UIImage imageNamed:@"btn_page.png"] forState:UIControlStateNormal];
		[btnPage addTarget:self action:@selector(firstPlay) forControlEvents:UIControlEventTouchUpInside];
		
		[menuImageView addSubview:btnPage];
		
		
		btnPlay = [UIButton buttonWithType:UIButtonTypeCustom];
		btnPlay.frame = ScaleCGRectMake(285, 15, 32, 32);
		
		[btnPlay setBackgroundImage:[UIImage imageNamed:@"btn_pause.png"] forState:UIControlStateNormal];
		//[btnPlay addTarget:self action:@selector(playPause:) forControlEvents:UIControlEventTouchUpInside];
		[btnPlay addTarget:self action:@selector(mosaicViewUP) forControlEvents:UIControlEventTouchUpInside];
		
		[menuImageView addSubview:btnPlay];
		
		btnSubtitle = [UIButton buttonWithType:UIButtonTypeCustom];
		btnSubtitle.frame = ScaleCGRectMake(393, 16, 55, 30);
		
		NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
		
		if ([setting boolForKey:@"togSubtitle"]) {
			[btnSubtitle setBackgroundImage:[UIImage imageNamed:@"btn_subtitle_off.png"] forState:UIControlStateNormal];
		} else {
			[btnSubtitle setBackgroundImage:[UIImage imageNamed:@"btn_subtitle_on.png"] forState:UIControlStateNormal];
		}
		
		[btnSubtitle addTarget:self action:@selector(toggleSubtitle) forControlEvents:UIControlEventTouchUpInside];
		
		[menuImageView addSubview:btnSubtitle];
		
	}
    return self;
}


- (void)firstPlay {
	[[NSNotificationCenter defaultCenter] postNotificationName:@"gotoFirst" object:nil];
}


- (void)toggleSubtitle {
	NSUserDefaults *setting = [NSUserDefaults standardUserDefaults];
	
	changeSubtitle = YES;
	
	if ([setting boolForKey:@"togSubtitle"]) {
		[setting setBool:NO forKey:@"togSubtitle"];
		[btnSubtitle setBackgroundImage:[UIImage imageNamed:@"btn_subtitle_on.png"] forState:UIControlStateNormal];
	} else {
		[setting setBool:YES forKey:@"togSubtitle"];
		[btnSubtitle setBackgroundImage:[UIImage imageNamed:@"btn_subtitle_off.png"] forState:UIControlStateNormal];
	}
	
	[btnPlay setBackgroundImage:[UIImage imageNamed:@"btn_pause.png"] forState:UIControlStateNormal];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:@"togSubtitle" object:nil];
}


- (void)backToHome {
	[[NSNotificationCenter defaultCenter] postNotificationName:@"backToHome2" object:nil];
}



- (void)playPause:(id)obj {
	switch (isPlay) {
		case NO:
			[obj setBackgroundImage:[UIImage imageNamed:@"btn_play.png"] forState:UIControlStateNormal];
			
			[[NSNotificationCenter defaultCenter] postNotificationName:@"pausePlay" object:nil];
			
			break;
		case YES:
			[obj setBackgroundImage:[UIImage imageNamed:@"btn_pause.png"] forState:UIControlStateNormal];
			
			[[NSNotificationCenter defaultCenter] postNotificationName:@"rePlay" object:nil];
			
			break;
		default:
			break;
	}
	
	isPlay = !isPlay;
}

-(void)mosaicViewUP {
	[[NSNotificationCenter defaultCenter] postNotificationName:@"PaperMosaic" object:nil];
}



@end
