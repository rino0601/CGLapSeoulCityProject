//
//  ONVideoViewController.m
//  photobook
//
//  Created by SeulBeom on 13. 8. 21..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//
#import "ONAppDelegate.h"
#import "ONVideoViewController.h"
#import "UIImage+SetAlpha.h"


@interface ONVideoViewController ()
- (IBAction) toggleSubs:(id)sender;
- (IBAction) doBack:(id)sender;
- (IBAction) togglePlay:(id)sender;
- (IBAction) playbackSliderValueChanged:(id)sender;

@end

@implementation ONVideoViewController

@synthesize videoView;
@synthesize playbackSlider;
@synthesize playButton;
@synthesize subtitleView;
@synthesize subtitleButton;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        NSLog(@"%@",nibNameOrNil);
        NSLog(@"%@",nibBundleOrNil);
    }
    return self;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
    isPlay = YES;
    [self setSubtitleOn:YES];
    playIndex = 0;
    [self initSubtitleContents];
    
    subtitleView.text = @"";
    
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *moviePath = [bundle pathForResource:@"00" ofType:@"mp4"];
    NSURL *movieURL = [NSURL fileURLWithPath:moviePath];
    
    player = [[AVPlayer alloc] initWithURL:movieURL];
    
    AVPlayerLayer *layer = [AVPlayerLayer layer];
    
    [layer setPlayer:player];
    [layer setFrame:CGRectMake(0, 0, 1024, 768)];
    [layer setVideoGravity:AVLayerVideoGravityResizeAspectFill];
    
    
    maxPlayTime = player.currentItem.duration;
    minPlayTime = CMTimeMake(0,maxPlayTime.timescale);
    
    [videoView.layer addSublayer:layer];
    playTimer = [NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(updateTime:) userInfo:nil repeats:YES];
    
    [playbackSlider setValue:0];
    [self play:YES];
    
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
- (void)updateTime:(NSTimer *)timer {
    CMTime t = [[player currentItem] currentTime];
    playbackSlider.value = (((float)t.value / t.timescale) / ((float)maxPlayTime.value / maxPlayTime.timescale));
    [self setSubtitleContents:t];
    if (player.rate == 0)
        [playTimer invalidate];
}
- (void)initSubtitleContents {
    NSString* path = [[NSBundle mainBundle] pathForResource:@"00"
                                                     ofType:@"smi"];
    NSString* content = [NSString stringWithContentsOfFile:path
                                                  encoding:NSUTF8StringEncoding
                                                     error:NULL];
    
    subData = [[NSMutableArray alloc] init];
    
    NSScanner *scanner = [NSScanner scannerWithString:content];
    
    while (![scanner isAtEnd])
    {
        @autoreleasepool
        {
            (void) [scanner scanUpToString:@"<" intoString:nil];
            (void) [scanner scanString:@"<" intoString:nil];
            NSString *keyString;
            (void) [scanner scanUpToString:@">" intoString:&keyString];
            (void) [scanner scanString:@">" intoString:nil];
            if([keyString hasPrefix:@"SYNC"]){
                
                NSInteger time;
                NSString *data;
                time = [[keyString substringFromIndex:11] intValue];
                (void) [scanner scanUpToString:@">" intoString:nil];
                (void) [scanner scanString:@">" intoString:nil];
                (void) [scanner scanUpToString:@"<" intoString:&data];
                data = [data stringByReplacingOccurrencesOfString:@"\n" withString:@""];
                
                NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                            [NSNumber numberWithInteger:time], @"time",
                                            data, @"data",
                                            nil];
                
                [subData addObject:dictionary];
                
                //NSLog(@"%@", subData);
            }
        }
    }
}
- (void)setSubtitleContents:(CMTime) playTime {
    
    int playSeconds = playTime.value / playTime.timescale;
    int index = -1;
    for (int i = 0; i <= subData.count; i++) {
        if(playSeconds < [[[subData objectAtIndex:i] objectForKey:@"time"] intValue]/1000) {
            //NSLog(@"%d, %d",playSeconds,[[[subData objectAtIndex:i] objectForKey:@"time"] intValue]/1000);
            index = i - 1;
            break;
        }
    }
    if( index == -1) {
        [subtitleView setText:@""];
        return;
    }
    
    
    /* setContentToHTMLString is private method.
     * But we can use this method with some tricks.
     * note: http://stackoverflow.com/questions/13239267/kerning-in-ios-uitextview
     */
    NSString *css = @"*{text-rendering: optimizeLegibility;text-shadow: 0 0 0.2em #FFF, 0 0 0.2em #FFF;letter-spacing:-1.5px;font-family:\"KoPubBatangBold\";font-size:30px}";
    NSString *html = [NSString stringWithFormat:@"<html><head><style>%@</style></head><body>%@</body></html>", css,[[subData objectAtIndex:index] valueForKey:@"data"]];
    @try {
        SEL setContentToHTMLString = NSSelectorFromString([@[@"set", @"Content", @"To", @"HTML", @"String", @":"] componentsJoinedByString:@""]);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
        [subtitleView performSelector:setContentToHTMLString withObject:html];
#pragma clang diagnostic pop
    }
    @catch (NSException *exception) {
        // html+css could not be applied to text view, so no kerning
        [subtitleView setText:[[subData objectAtIndex:index] valueForKey:@"data"]];
    }
}

- (IBAction) playbackSliderValueChanged:(id)sender {
    float t = [(UISlider*)sender value];
    [player pause];
    CMTime time = CMTimeMake((int)((maxPlayTime.value - minPlayTime.value) * t), maxPlayTime.timescale);
    [player seekToTime:time];
    [player play];
}

- (IBAction) toggleSubs:(id)sender {
    [self setSubtitleOn:!isOnSubs];
}
- (void) setSubtitleOn:(BOOL)flag {
    if(flag){
        [subtitleView setHidden:NO];
        isOnSubs = YES;
        [subtitleButton setImage:[UIImage imageNamed:@"subtitle_on.png"] forState:UIControlStateHighlighted];
        [subtitleButton setImage:[[UIImage imageNamed:@"subtitle_on.png"] imageByApplyingAlpha:0.6] forState:UIControlStateNormal];

    } else {
        [subtitleView setHidden:YES];
        isOnSubs = NO;
        [subtitleButton setImage:[UIImage imageNamed:@"subtitle_off.png"] forState:UIControlStateHighlighted];
        [subtitleButton setImage:[[UIImage imageNamed:@"subtitle_off.png"] imageByApplyingAlpha:0.6] forState:UIControlStateNormal];

    }
}
- (IBAction) togglePlay:(id)sender {
    [self play:!isPlay];
}

- (void) play:(BOOL)flag{
    if(flag){
        [playButton setImage:[UIImage imageNamed:@"pause.png"] forState:UIControlStateNormal];
        [player play];
        isPlay = YES;
        if (![playTimer isValid]){
            playTimer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(updateTime:) userInfo:nil repeats:YES];
        }
    }
    else{
        [playButton setImage:[UIImage imageNamed:@"play.png"] forState:UIControlStateNormal];
        [player pause];
        isPlay = NO;
    }
}

- (IBAction) doBack:(id)sender {
    [self play:NO];
}

@end
