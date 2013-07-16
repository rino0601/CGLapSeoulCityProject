//
//  ONViewController.m
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "ONViewController.h"

@interface ONViewController ()
- (IBAction)doStartButton:(id)sender;
- (IBAction)home:(UIStoryboardSegue *)segue;
@end

@implementation ONViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)doStartButton:(id)sender {
    [self performSegueWithIdentifier:@"startContents" sender:self];
    
}
- (IBAction)home:(UIStoryboardSegue *)segue {
    // Optional place to read data from closing controller
}

@end
