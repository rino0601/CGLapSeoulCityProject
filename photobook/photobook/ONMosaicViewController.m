//
//  ONMosaicViewController.m
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "ONMosaicViewController.h"
#import "UIImageCVArrConverter.h"

@interface ONMosaicViewController ()

@end

@implementation ONMosaicViewController
@synthesize paperMosaicCanvas;
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation{
 //   [image_signature setImage:[self resizeImage:image_signature.image]];
    return (toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft);
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
		// nothing to do.
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}
- (UIImage*)resultImage {
    return [paperMosaicCanvas resultImage];
}

- (IBAction)doAsAutoMode:(UIButton *)sender {
	[paperMosaicCanvas doAsAutoMode];
	//[paperMosaicCanvas saveResult];
}

- (IBAction)colorChangeFrom:(UIButton *)sender {
	UIImage *temp = [sender imageForState:UIControlStateNormal];
	IplImage *buttonIplImage = [UIImageCVArrConverter CreateIplImageFromUIImage:temp];
	CGPoint centerPoint = CGPointMake(80, 80);
	CvScalar centerColor = cvGet2D(buttonIplImage, (int)centerPoint.x, (int)centerPoint.y);
	cvReleaseImage(&buttonIplImage);
	[paperMosaicCanvas setPaperColor:[UIColor colorWithRed:centerColor.val[0]/255.0f green:centerColor.val[1]/255.0f blue:centerColor.val[2]/255.0f alpha:centerColor.val[3]/255.0f]];
}

@end
