//
//  ONContentsViewController.m
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "ONContentsViewController.h"
#import "ONSnapshotCollectionViewCell.h"
#import "ONMosaicViewController.h"
#import "ONAppDelegate.h"

@interface ONContentsViewController ()

- (IBAction)doMenu;
- (IBAction)doPrev;
- (IBAction)doNext;
- (IBAction)doMosaic;
- (IBAction)doPageSelection;
- (IBAction)doAudioPlay;
- (IBAction)onContents:(UIStoryboardSegue *)segue;

- (void)playBookWithIndex:(int)index;

@end

@implementation ONContentsViewController

@synthesize right;
@synthesize left;
@synthesize pageView;
@synthesize onCollectionView;
@synthesize menuBar;
@synthesize menuIcon;
@synthesize contentsViewer;
@synthesize subtitleView;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
           audioPlayer = [[AVAudioPlayer alloc] init];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    NSString *path = [[NSBundle mainBundle] bundlePath];
    
    NSString *finalPath = [path stringByAppendingPathComponent:@"settings.plist"];
    
    NSMutableDictionary *contentsPlist = [[NSMutableDictionary alloc] initWithContentsOfFile:finalPath];
    
    contentsList = [contentsPlist objectForKey:@"contentsList"];
    
    currentViewIndex = 0;
    
    maxViewIndex = contentsList.count;
    [self playBookWithIndex:currentViewIndex];
	
    [self.onCollectionView registerClass:[ONSnapshotCollectionViewCell class] forCellWithReuseIdentifier:@"PhotoCell"];
    
    mosaicImageView = [[UIImageView alloc] init];
    
    subsList = [[contentsList objectAtIndex:currentViewIndex] objectForKey:@"subs"];
    soundsList = [[contentsList objectAtIndex:currentViewIndex] objectForKey:@"sounds"];
    
    //[mosaicImageView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    audioIndex = 0;
    
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)doMenu {
    
    if( [menuBar isHidden] ){
        
        CGPoint cp = [menuIcon center];
        CGPoint menuBarCenterPoint = [menuBar center];
        CGPoint menuBarOriginPoint = [menuBar center];
        
        NSLog(@"MenuBarOn");
        
        menuBarCenterPoint.y += 100;
        [menuBar setCenter:menuBarCenterPoint];
        [menuBar setHidden:false];
        
        cp.y -= 50;
        
        [UIView animateWithDuration:0.5 animations:^{
            menuBar.transform = CGAffineTransformMakeTranslation(0.5, 0.5);
            [menuBar setCenter:menuBarOriginPoint];
            [menuIcon setCenter:cp];
        }];
        
    } else {
    
        [self setMenuBarHidden];
    
    }
    
}

- (void)setMenuBarHidden {
    [pageView setHidden:YES];
    
    CGPoint cp, mcp, mop;
    
    mop = [menuBar center];
    mcp = [menuBar center];
    
    NSLog(@"MenuBarOff");
    
    cp = [menuIcon center];
    cp.y += 50;
    
    mcp.y += 100;
    
    [UIView animateWithDuration:0.5 animations:^{
        menuBar.transform = CGAffineTransformMakeTranslation(0.5, 0.5);
        [menuBar setCenter:mcp];
        [menuIcon setCenter:cp];
    } completion:^(BOOL finished) {
        [menuBar setHidden:true];
        [menuBar setCenter:mop];
    }];
}

- (IBAction)doPrev{
    int index = currentViewIndex;
    if( --index < 0) {
        index = 0;
        return;
    }
    [self playBookWithIndex:index];
    
}
- (IBAction)doNext{
    int index = currentViewIndex;
    if( ++index >= maxViewIndex) {
        index = maxViewIndex - 1;
        return;
    }
    [self playBookWithIndex:index];
    
}

- (IBAction)doAudioPlay {
    [audioPlayer stop];
    
    if(audioPlayer == nil)
        audioPlayer = [AVAudioPlayer alloc];

    NSString *audioPath = [soundsList objectAtIndex:audioIndex];
    
    NSLog(@"audio %d, %@ play", audioIndex, audioPath);
    
    [[audioPlayer initWithContentsOfURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:audioPath ofType:@"wav"]] error:NULL] setDelegate:self];
    
    
    NSLog(@"x %f, y %f, width %f, height %f", subtitleView.frame.origin.x, subtitleView.frame.origin.y, subtitleView.frame.size.width, subtitleView.frame.size.height);
    
    subtitleView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.001, 0.001);
    [subtitleView setText:[subsList objectAtIndex:audioIndex]];
    
    [UIView animateWithDuration:0.5 animations:^{
        subtitleView.transform = CGAffineTransformIdentity;
    }];
    
    [audioPlayer play];
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer*)avp successfully:(BOOL)flag {
    //NSLog(@"asdf");
    if(flag) {
        if( ++audioIndex >= soundsList.count) {
            audioIndex = 0;
            [self doNext];
        } else {
            [self doAudioPlay];
        }
    }
}

- (void)playBookWithIndex:(int)index{
    
    Boolean animationStart = NO;
    Boolean isLeft = NO;
    
    index < 0 ? index = 0 : index >= maxViewIndex ? index = maxViewIndex - 1 : index;
    
    (index == 0) ? [left setHidden:YES] : [left setHidden:NO];
    (index == maxViewIndex -1) ? [right setHidden:YES] : [right setHidden:NO];
    
    if(index != currentViewIndex) {
        animationStart = YES;
        index < currentViewIndex ? isLeft = YES : isLeft = NO;
    }
    
    NSLog(@"Load %@ file",[contentsList objectAtIndex:index]);
    [contentsViewer setImage:[UIImage imageNamed:[[contentsList objectAtIndex:index] objectForKey:@"backImage"]]];
    currentViewIndex = index;

    subsList = [[contentsList objectAtIndex:index] objectForKey:@"subs"];
    soundsList = [[contentsList objectAtIndex:index] objectForKey:@"sounds"];
    audioIndex = 0;
   
    NSDictionary *subtitleFrame = [[contentsList objectAtIndex:index] objectForKey:@"subtitleFrame"];
    
    
    //[subtitleView setTranslatesAutoresizingMaskIntoConstraints:YES];
    
    /*[subtitleView setCenter:CGPointMake
     ([[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"x"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"y"] intValue]] floatValue])];
    */
    [subtitleView setFrame:CGRectMake
     ([[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"x"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"y"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"width"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"height"] intValue]] floatValue])];
    

    
    NSLog(@"x %f, y %f, width %f, height %f", subtitleView.frame.origin.x, subtitleView.frame.origin.y, subtitleView.frame.size.width, subtitleView.frame.size.height);
    
    //if(animationStart) {
        [subtitleView setText:@""];
        [mosaicImageView removeFromSuperview];

        [UIView beginAnimations:nil context:NULL];
        [UIView setAnimationDuration:1.0];
        [UIView setAnimationTransition:(isLeft ? UIViewAnimationTransitionCurlDown : UIViewAnimationTransitionCurlUp) forView:contentsViewer cache:YES];
        [UIView setAnimationBeginsFromCurrentState:NO];
        [UIView setAnimationDelegate:self];
        [UIView setAnimationDidStopSelector:@selector(doAudioPlay)];
        [UIView commitAnimations];
    //}
    
    NSLog(@"x %f, y %f, width %f, height %f", subtitleView.frame.origin.x, subtitleView.frame.origin.y, subtitleView.frame.size.width, subtitleView.frame.size.height);
}

- (IBAction)doMosaic{
    [audioPlayer stop];
    [self setMenuBarHidden];
    [self doMenu];
	/**************리소스 이미지 결정하는 코드입니다. 한줄이라도 빠지면 안되요.**************/
	ONAppDelegate * appDelegate = [[UIApplication sharedApplication] delegate];
	[appDelegate setMosaicSource:[UIImage imageNamed:@"source_img.bmp"]];
	[appDelegate setMosaicMask:[UIImage imageNamed:@"mask_img.bmp"]];
	[appDelegate setMosaicEdge:[UIImage imageNamed:@"edge_img.bmp"]];
	[appDelegate setStartSectionCode:102];
	/*************************************************************************/
	
    [self performSegueWithIdentifier:@"doMosaic" sender:self];
}

- (IBAction)doPageSelection {
    [pageView setHidden:!([pageView isHidden])];
}

- (IBAction)onContents:(UIStoryboardSegue *)segue {
    if([[[[segue sourceViewController] class] description] isEqual: @"ONMosaicViewController"]){
        ONMosaicViewController* t = [segue sourceViewController];
        //[mosaicImageView setImage:[UIImage imageNamed:@"mask_img.bmp"]];
        CGFloat maskColor[6] = {255,255,255,255,255,255};
        [mosaicImageView setImage:[UIImage imageWithCGImage:CGImageCreateWithMaskingColors([[t resultImage] CGImage], maskColor)]];
        
        NSDictionary *mosaicFrame = [[[contentsList objectAtIndex:currentViewIndex] objectForKey:@"mosaic"] objectForKey:@"frame"];
        
        [mosaicImageView setFrame:CGRectMake
         ([[NSNumber numberWithInt:[[mosaicFrame objectForKey:@"x"] intValue]] floatValue],
          [[NSNumber numberWithInt:[[mosaicFrame objectForKey:@"y"] intValue]] floatValue],
          [[NSNumber numberWithInt:[[mosaicFrame objectForKey:@"width"] intValue]] floatValue],
          [[NSNumber numberWithInt:[[mosaicFrame objectForKey:@"height"] intValue]] floatValue])];
        
        [mosaicImageView removeFromSuperview];
        [contentsViewer addSubview:mosaicImageView];
    }
}


#pragma mark - UICollectionView Datasource
- (NSInteger)collectionView:(UICollectionView *)view numberOfItemsInSection:(NSInteger)section
{
    return contentsList.count;
}
- (NSInteger)numberOfSectionsInCollectionView: (UICollectionView *)collectionView
{
    return 1;
}
- (UICollectionViewCell *)collectionView:(UICollectionView *)cv cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"PhotoCell";
    ONSnapshotCollectionViewCell *cell = [cv dequeueReusableCellWithReuseIdentifier:CellIdentifier forIndexPath:indexPath];
    NSString *searchTerm;
    searchTerm = [[contentsList objectAtIndex:indexPath.row] objectForKey:@"snapshot"];
    
    NSLog(@"imageName \"%@\"",searchTerm);
    
    [cell.imageViewCell setHidden:NO];
    [cell.imageViewCell setImage:[UIImage imageNamed:searchTerm]];
    return cell;
}


#pragma mark - UICollectionViewDelegateFlowLayout
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    CGSize retval = CGSizeMake(300, 240);
    return retval;
}
- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout insetForSectionAtIndex:(NSInteger)section {
    return UIEdgeInsetsMake(5, 5, 5, 5);
}

#pragma mark - UICollectionViewDelegate
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    [self playBookWithIndex:indexPath.row];
    [self setMenuBarHidden];
}
- (BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    return YES;
}
@end
