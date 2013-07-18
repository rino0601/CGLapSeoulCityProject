//
//  ONMosaicViewController.m
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "ONMosaicViewController.h"

@interface ONMosaicViewController ()

@end

@implementation ONMosaicViewController

@synthesize backPan;
@synthesize mosaicPan;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        PaperMosaicCanvas *mosaicView = [[PaperMosaicCanvas alloc] initWithFrame:CGRectMake(100,100,100,100)];
		[backPan addSubview:mosaicView];
    }
    return self;
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
