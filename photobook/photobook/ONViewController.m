//
//  ONViewController.m
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "ONViewController.h"
#import "ONAppDelegate.h"


@implementation ONViewController

#pragma mark - For IOS 5.1< landscape.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation{
    return (toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft || toInterfaceOrientation == UIInterfaceOrientationLandscapeRight);
}


#pragma mark - For IOS 6.0> landscape.
-(BOOL)shouldAutorotate {
    return YES;
}
- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscapeLeft;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    NSString *path = [[NSBundle mainBundle] bundlePath];
    
    NSString *finalPath = [path stringByAppendingPathComponent:@"settings.plist"];
    
    NSMutableDictionary *contentsPlist = [[NSMutableDictionary alloc] initWithContentsOfFile:finalPath];
    
    ONAppDelegate * appDelegate = [[UIApplication sharedApplication] delegate];
    
    appDelegate.lang = [contentsPlist objectForKey:@"language"];
    
    [self audioPlay];
    
}

- (void)audioPlay {
    if(startAudio == nil)
        startAudio = [AVAudioPlayer alloc];
    
    ONAppDelegate * appDelegate = [[UIApplication sharedApplication] delegate];
    
    
    NSString* audioPath = [appDelegate.lang isEqualToString:@"eng"] ? [NSString stringWithFormat:@"01"] : [NSString stringWithFormat:@"01_%@",appDelegate.lang];
    
    [[startAudio initWithContentsOfURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:audioPath ofType:@"wav"]] error:NULL] play];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)doStartButton:(id)sender {
    [startAudio stop];
    [self performSegueWithIdentifier:@"startContents" sender:self];
    
}
- (IBAction)home:(UIStoryboardSegue *)segue {
    // Optional place to read data from closing controller
    [self audioPlay];
}

@end
