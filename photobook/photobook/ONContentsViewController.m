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
    
    NSUserDefaults *defaultsValues = [NSUserDefaults standardUserDefaults];
    
    
    contentsList = [defaultsValues arrayForKey:@"contentsList"];
    
    currentViewIndex = 0;
    maxViewIndex = contentsList.count;
    
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
    
    /* 화면을 좌측으로 넘기는 부분 구현.
     */
    
    NSLog(@"index: %d",currentViewIndex);

}
- (IBAction)doRight{
    if( ++currentViewIndex > maxViewIndex) {
        currentViewIndex = maxViewIndex;
    }
    
    /* 화면을 우측으로 넘기는 부분 구현.
     */
    
    NSLog(@"index: %d",currentViewIndex);
    
}


- (void)playBookWithIndex:(int)index{
    //[contentsViewer ]
}

- (IBAction)doMosaic{
    [self doMenu];
    [self performSegueWithIdentifier:@"doMosaic" sender:self];
}

- (IBAction)onContents:(UIStoryboardSegue *)segue {
    // Optional place to read data from closing controller
}

@end
