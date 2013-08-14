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
#import "UIImage+SetAlpha.h"

@interface ONContentsViewController ()

- (IBAction)doMenu;
- (IBAction)doPrev;
- (IBAction)doNext;
- (IBAction)doMosaic;
- (IBAction)doPageSelection;
- (IBAction)doAudioPlay;
- (IBAction)doReplay:(id)sender;
- (IBAction)doChangeLang;
- (IBAction)onContents:(UIStoryboardSegue *)segue;
- (IBAction)playButtonClickedEffectSound:(id)sender;
- (void)playBookWithIndex:(int)index;

@end

@implementation ONContentsViewController

@synthesize mosaicImageView;
@synthesize right;
@synthesize left;
@synthesize pageView;
@synthesize onCollectionView;
@synthesize menuBar;
@synthesize menuIcon;
@synthesize contentsView;
@synthesize contentsImageView;
@synthesize subtitleView;
@synthesize languageButton;
@synthesize zoomInLayer;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    bgmCounter = 0;
    
    NSString *path = [[NSBundle mainBundle] bundlePath];
    NSString *settingPath = [path stringByAppendingPathComponent:@"settings.plist"];
    NSDictionary *contentsPlist = [[NSMutableDictionary alloc] initWithContentsOfFile:settingPath];
    contentsList = [contentsPlist objectForKey:@"contentsList"];
    currentViewIndex = -1;
    maxViewIndex = contentsList.count;
    
    [self.onCollectionView registerClass:[ONSnapshotCollectionViewCell class] forCellWithReuseIdentifier:@"PhotoCell"];
    
    //[mosaicImageView setTranslatesAutoresizingMaskIntoConstraints:NO];
    ONAppDelegate * appDelegate = [[UIApplication sharedApplication] delegate];
    appDelegate.lang == nil ? lang = [contentsPlist objectForKey:@"language"] : lang = appDelegate.lang;
    
    audioIndex = 0;
    
    NSString *temp = [[NSBundle mainBundle] pathForResource:lang ofType:@"png"];
    NSLog(@"%@",temp);
    
    [languageButton setImage:[[UIImage imageWithContentsOfFile:temp] imageByApplyingAlpha:0.5] forState:UIControlStateNormal];
    [languageButton setImage:[UIImage imageWithContentsOfFile:temp] forState:UIControlStateHighlighted];
    rightBImage = [right imageForState:UIControlStateNormal];
    leftBImage = [left imageForState:UIControlStateNormal];
    [right setImage:[rightBImage imageByApplyingAlpha:0.5] forState:UIControlStateNormal];
    [left setImage:[leftBImage imageByApplyingAlpha:0.5] forState:UIControlStateNormal];
    
    [subtitleView setFont:[UIFont fontWithName:@"KoPubBatangBold" size:30]];
    
    [self playBookWithIndex:currentViewIndex];

}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
- (void)setLanguege:(NSString* )lng{
    ONAppDelegate * appDelegate = [[UIApplication sharedApplication] delegate];
    if([lng isEqualToString:@"kor"]){
        lang = lng;
        [appDelegate setLang:lng];
    } else if ([lng isEqualToString:@"eng"]){
        lang = lng;
        [appDelegate setLang:lng];
    }
    
    subsList = [[contentsList objectAtIndex:currentViewIndex] objectForKey:[NSString stringWithFormat:@"subs_%@",lang]];
    [self doAudioPlay];
}
- (IBAction)playButtonClickedEffectSound:(id)sender {
    if(clickEffectPlayer == nil){
        clickEffectPlayer = [[AVAudioPlayer alloc]
                             initWithContentsOfURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:@"but_click" ofType:@"wav"]] error:nil];
        [clickEffectPlayer prepareToPlay];
        [clickEffectPlayer setVolume:0.1];
    }
    [clickEffectPlayer play];
}
- (IBAction)doChangeLang {
    if([lang isEqualToString:@"kor"]){
        [self setLanguege:@"eng"];
    } else {
        [self setLanguege:@"kor"];
    }
    
    NSString *temp = [[NSBundle mainBundle] pathForResource:lang ofType:@"png"];
    
    [languageButton setImage:[[UIImage imageWithContentsOfFile:temp] imageByApplyingAlpha:0.6] forState:UIControlStateNormal];
    [languageButton setImage:[UIImage imageWithContentsOfFile:temp] forState:UIControlStateHighlighted];
}

- (IBAction)doMenu {
    
    if( [menuBar isHidden] ){
        [audioPlayer pause];
        [bgmPlayer pause];
        
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
        [audioPlayer play];
        [bgmPlayer play];
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
    if( ++index >= maxViewIndex ) {
        index = maxViewIndex - 1;
        return;
    }
    [self playBookWithIndex:index];
}

- (IBAction)doReplay:(id)sender {
    audioIndex = 0;
    [self pageAnimation];
    [self doAudioPlay];
}
- (IBAction)doAudioPlay {
    [audioPlayer stop];
    
    if(audioPlayer == nil)
        audioPlayer = [AVAudioPlayer alloc];

    NSString *audioPath = [lang isEqualToString:@"eng"] ? [soundsList objectAtIndex:audioIndex] : [NSString stringWithFormat:@"%@_%@",[soundsList objectAtIndex:audioIndex],lang];
    
    NSLog(@"audio %d, %@ play", audioIndex, audioPath);
    
    [[audioPlayer initWithContentsOfURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:audioPath ofType:@"wav"]] error:NULL] setDelegate:self];
    
    
    //NSLog(@"x %f, y %f, width %f, height %f", subtitleView.frame.origin.x, subtitleView.frame.origin.y, subtitleView.frame.size.width, subtitleView.frame.size.height);
    
    
    [bgmPlayer stop];
    if(bgmPlayer == nil) {
        bgmPlayer = [AVAudioPlayer alloc];
    }
    
    if(bgmCounter == 0) {
        [[bgmPlayer initWithContentsOfURL:[NSURL fileURLWithPath: [[NSBundle mainBundle] pathForResource:@"bgm1" ofType:@"wav"]] error:NULL] setDelegate:self];
        [bgmPlayer play];
        bgmCounter = 1;
    } else {
        if(![bgmPlayer.description hasPrefix:@"bgm2.wav"]) {
            [bgmPlayer stop];
            [[bgmPlayer initWithContentsOfURL:[NSURL fileURLWithPath: [[NSBundle mainBundle] pathForResource:@"bgm2" ofType:@"wav"]] error:NULL] setNumberOfLoops:-1];
            [bgmPlayer play];
        }
        
        [subtitleView setText:[subsList objectAtIndex:audioIndex]];
        CGRect rect          = subtitleView.frame;
        rect.size.height     = subtitleView.contentSize.height;
        subtitleView.frame   = rect;
        
        subtitleView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.001, 0.001);
        [UIView animateWithDuration:0.5 animations:^{
            subtitleView.transform = CGAffineTransformIdentity;
        }];
        
        [audioPlayer play];
    }
    
    
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer*)avp successfully:(BOOL)flag {
    //NSLog(@"%@",avp.url);
    if(flag && avp == audioPlayer) {
        if( ++audioIndex >= soundsList.count) {
            audioIndex = 0;
            bgmCounter = 0;
            [right setImage:[rightBImage imageByApplyingAlpha:1] forState:UIControlStateNormal];
            [left setImage:[leftBImage imageByApplyingAlpha:1] forState:UIControlStateNormal];
            [bgmPlayer stop];
            [[bgmPlayer initWithContentsOfURL:[NSURL fileURLWithPath: [[NSBundle mainBundle] pathForResource:@"bgm3" ofType:@"wav"]] error:NULL] play];
        } else {
            [self doAudioPlay];
        }
    } else if(avp == bgmPlayer && bgmCounter == 1){
        [self doAudioPlay];
    }
}
- (void)pageAnimationDidStop:(NSString *)anim finished:(NSNumber* )finished{
    if([finished isEqual:@YES] && [anim isEqualToString:@"page"]) {
        [self pageAnimation];
        [self doAudioPlay];
    }
}

- (void)pageAnimation{
    [right setImage:[rightBImage imageByApplyingAlpha:0.5] forState:UIControlStateNormal];
    [left setImage:[leftBImage imageByApplyingAlpha:0.5] forState:UIControlStateNormal];
    [UIView animateWithDuration:1.5 animations:^{
        [UIView setAnimationDelay:.5];
        [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
        [zoomInLayer setTransform:CGAffineTransformIdentity];
    }];
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
    
    //NSLog(@"Load %@ file",[contentsList objectAtIndex:index]);
    
    NSDictionary *thisDic = [contentsList objectAtIndex:index];
    
    [contentsImageView setImage:[UIImage imageNamed:[thisDic objectForKey:@"backImage"]]];
    [contentsImageView setFrame:CGRectMake(-[[thisDic objectForKey:@"xPos"] floatValue], 0, 1200, 768)];
    [zoomInLayer.layer removeAllAnimations];
    
    NSLog(@"%f",contentsView.frame.origin.x);
    NSLog(@"%f",contentsImageView.frame.origin.x);
    currentViewIndex = index;

    NSDictionary *mosaicDic = [thisDic objectForKey:@"mosaic"];
    [mosaicImageView setImage:[UIImage imageNamed:[NSString stringWithFormat:@"%@%@",[mosaicDic objectForKey:@"prefix"],@"_ch.png"]]];
    
    NSDictionary *mosaicFrame = [mosaicDic objectForKey:@"frame"];
    
    [mosaicImageView setFrame:CGRectMake
     ([[NSNumber numberWithInt:[[mosaicFrame objectForKey:@"x"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[mosaicFrame objectForKey:@"y"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[mosaicFrame objectForKey:@"width"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[mosaicFrame objectForKey:@"height"] intValue]] floatValue])];
    

    subsList = [thisDic objectForKey:[NSString stringWithFormat:@"subs_%@",lang]];
    soundsList = [thisDic objectForKey:@"sounds"];
    audioIndex = 0;
   
    NSDictionary *subtitleFrame = [thisDic objectForKey:@"subtitleFrame"];
    
    [subtitleView setFrame:CGRectMake
     ([[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"x"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"y"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"width"] intValue]] floatValue],
      [[NSNumber numberWithInt:[[subtitleFrame objectForKey:@"height"] intValue]] floatValue])];
    
    //NSLog(@"x %f, y %f, width %f, height %f", subtitleView.frame.origin.x, subtitleView.frame.origin.y, subtitleView.frame.size.width, subtitleView.frame.size.height);
    
    if(animationStart) {
        [subtitleView setText:@""];
        [audioPlayer stop];
        
        if(efectPlayer == nil) {
            efectPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath: [[NSBundle mainBundle] pathForResource:@"page-flip-1" ofType:@"wav"]] error:NULL];
        }
        [efectPlayer play];
        
        NSDictionary *zoomInFrame = [thisDic objectForKey:@"zoomInFrame"];
            
        
        CGRect ori = [contentsView frame];
        CGRect wannabe = CGRectMake
        ([[NSNumber numberWithInt:[[zoomInFrame objectForKey:@"x"] intValue]] floatValue],
            [[NSNumber numberWithInt:[[zoomInFrame objectForKey:@"y"] intValue]] floatValue],
            [[NSNumber numberWithInt:[[zoomInFrame objectForKey:@"width"] intValue]] floatValue],
            [[NSNumber numberWithInt:[[zoomInFrame objectForKey:@"height"] intValue]] floatValue]);
            
        double widthRatio = wannabe.size.width != 0 ? ori.size.width /wannabe.size.width  : 1;
        double heightRatio= wannabe.size.height!= 0 ? ori.size.height/wannabe.size.height : 1;
            
        double ratio = widthRatio > heightRatio ? heightRatio : widthRatio;
        
        CGAffineTransform temp = CGAffineTransformMakeScale(ratio, ratio);
        [zoomInLayer setTransform:CGAffineTransformTranslate(temp, 512 - wannabe.origin.x - wannabe.size.width/2, 768/2 - wannabe.origin.y - wannabe.size.height/2)];
        
        [UIView beginAnimations:@"page" context:NULL];
        {
            [UIView setAnimationDuration:1.2];
            [UIView setAnimationTransition:(isLeft ? UIViewAnimationTransitionCurlDown : UIViewAnimationTransitionCurlUp) forView:contentsView cache:YES];
            [UIView setAnimationBeginsFromCurrentState:NO];
            [UIView setAnimationDelegate:self];
            [UIView setAnimationDidStopSelector:@selector(pageAnimationDidStop:finished:)];
        }
        [UIView commitAnimations];
    }
    
    //NSLog(@"x %f, y %f, width %f, height %f", subtitleView.frame.origin.x, subtitleView.frame.origin.y, subtitleView.frame.size.width, subtitleView.frame.size.height);
}

- (IBAction)doMosaic{
    [audioPlayer pause];
    [bgmPlayer pause];
    [self setMenuBarHidden];
    
    NSString *prefix = [[[contentsList objectAtIndex:currentViewIndex] objectForKey:@"mosaic"] objectForKey:@"prefix"];
	/**************리소스 이미지 결정하는 코드입니다. 한줄이라도 빠지면 안되요.**************/
	ONAppDelegate * appDelegate = [[UIApplication sharedApplication] delegate];
	[appDelegate setMosaicSource:[UIImage imageNamed:[NSString stringWithFormat:@"%@%@",prefix,@"_source_img.bmp"]]];
	[appDelegate setMosaicMask:[UIImage imageNamed:[NSString stringWithFormat:@"%@%@",prefix,@"_mask_img.bmp"]]];
	[appDelegate setMosaicEdge:[UIImage imageNamed:[NSString stringWithFormat:@"%@%@",prefix,@"_edge_img.bmp"]]];
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
        [bgmPlayer play];
        [audioPlayer play];
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
