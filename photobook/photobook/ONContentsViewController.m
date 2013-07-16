//
//  ONContentsViewController.m
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "ONContentsViewController.h"

@interface ONContentsViewController ()

- (IBAction)doMenu;
- (IBAction)doLeft;
- (IBAction)doRight;
- (IBAction)doMosaic;
- (IBAction)doPageAnimation;
- (IBAction)onContents:(UIStoryboardSegue *)segue;

- (void)playBookWithIndex:(int)index;

@end

@implementation ONContentsViewController

@synthesize menuBar;
@synthesize menuIcon;
@synthesize contentsViewer;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    isMenuOn = false;
    
    NSString *path = [[NSBundle mainBundle] bundlePath];
    
    NSString *finalPath = [path stringByAppendingPathComponent:@"settings.plist"];
    
    NSMutableDictionary *contentsPlist = [[NSMutableDictionary alloc] initWithContentsOfFile:finalPath];
    
    contentsList = [contentsPlist objectForKey:@"contentsList"];
    
    
    //currentViewIndex = (int)[contentsPlist valueForKey:@"readPage"] - 1;
    currentViewIndex = 0;
    
    maxViewIndex = contentsList.count;
    [self playBookWithIndex:currentViewIndex];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)doMenu {
    CGPoint cp;
    if( !isMenuOn ){
        isMenuOn = !isMenuOn;
        NSLog(@"MenuBarOn");
        [menuBar setHidden:false];
        cp = [menuIcon center];
        cp.y -= 30;
        [menuIcon setCenter:cp];
        
    } else {
        isMenuOn = !isMenuOn;
        NSLog(@"MenuBarOff");
        [menuBar setHidden:true];
        cp = [menuIcon center];
        cp.y += 30;
        [menuIcon setCenter:cp];
    }
    
}

- (IBAction)doLeft {
    if( --currentViewIndex < 0) {
        currentViewIndex = 0;
    }
    
    /* 화면을 좌측으로 넘기는 부분 animation 구현.
     */
    
    NSLog(@"index: %d",currentViewIndex);
    [self playBookWithIndex:currentViewIndex];
}
- (IBAction)doRight{
    if( ++currentViewIndex >= maxViewIndex) {
        currentViewIndex = maxViewIndex - 1;
    }
    
    /* 화면을 우측으로 넘기는 부분 animation 구현.
     */
    
    NSLog(@"index: %d",currentViewIndex);
    [self playBookWithIndex:currentViewIndex];
}

- (IBAction)doPageAnimation {
    /* 페이지를 읽기 시작할 때 로딩시의 에니메이션을 설정 및 시작하는 함수.
     * 추후 구현 예정
     */
}

- (void)playBookWithIndex:(int)index{
    index < 0 ? index = 0 : index >= maxViewIndex ? index = maxViewIndex - 1 : index;
    NSLog(@"Load %@ file",[contentsList objectAtIndex:index]);
    [contentsViewer setImage:[UIImage imageNamed:[contentsList objectAtIndex:index]]];
    [self doPageAnimation];
}

- (IBAction)doMosaic{
    [self doMenu];
    [self performSegueWithIdentifier:@"doMosaic" sender:self];
}

- (IBAction)onContents:(UIStoryboardSegue *)segue {
    // Optional place to read data from closing controller
}

@end
