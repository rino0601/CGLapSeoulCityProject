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
- (IBAction)doPageAnimation;
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
    
    /*self.searches = [@[] mutableCopy];
    self.searchResults = [@{} mutableCopy];
    */
    [self.onCollectionView registerClass:[ONSnapshotCollectionViewCell class] forCellWithReuseIdentifier:@"PhotoCell"];
    
    mosaicImageView = [[UIImageView alloc] init];
    
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return YES;
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
        [self setMenuBarHidden];
    }
    
}

- (void)setMenuBarHidden {
    [pageView setHidden:YES];
    CGPoint cp;
    isMenuOn = !isMenuOn;
    NSLog(@"MenuBarOff");
    [menuBar setHidden:true];
    cp = [menuIcon center];
    cp.y += 30;
    [menuIcon setCenter:cp];
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

- (IBAction)doPageAnimation {
    /* 페이지를 읽기 시작할 때 로딩시의 에니메이션을 설정 및 시작하는 함수.
     * 추후 구현 예정
     */
}

- (IBAction)doAudioPlay {
    [audioPlayer stop];
    int index = currentViewIndex+1;
    NSString* audioPath = (index < 10 ? [[NSString alloc] initWithFormat:@"0%d",index ] : [[NSString alloc] initWithFormat:@"%d",index ]);
    
    
    audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:audioPath ofType:@"wav"]] error:NULL];
    
    [audioPlayer setDelegate:self];
    
    [audioPlayer play];
    
}
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer*)avp successfully:(BOOL)flag {
    //NSLog(@"asdf");
    if(flag) {
        [self doNext];
    }
}

- (void)playBookWithIndex:(int)index{
    
    Boolean animationStart = NO;
    Boolean isLeft = NO;
    
    index < 0 ? index = 0 : index >= maxViewIndex ? index = maxViewIndex - 1 : index;
    
    if(index == 0) {
        [left setHidden:YES];
    } else if (index == maxViewIndex -1) {
        [right setHidden:YES];
    } else {
        [left setHidden:NO];
        [right setHidden:NO];
    }
    if(index != currentViewIndex) {
        animationStart = YES;
        
        index < currentViewIndex ? isLeft = YES : isLeft = NO;
    }
    NSLog(@"Load %@ file",[contentsList objectAtIndex:index]);
    [contentsViewer setImage:[UIImage imageNamed:[contentsList objectAtIndex:index]]];
    [self doPageAnimation];
    currentViewIndex = index;

    if(animationStart) {
        [UIView beginAnimations:nil context:NULL];
        [UIView setAnimationDuration:1.0];
        [UIView setAnimationTransition:(isLeft ? UIViewAnimationTransitionCurlDown : UIViewAnimationTransitionCurlUp) forView:contentsViewer cache:YES];
        [UIView setAnimationBeginsFromCurrentState:NO];
        [UIView setAnimationDelegate:self];
        [UIView commitAnimations];
    }

    [self doPageAnimation];
    [self doAudioPlay];
}

- (IBAction)doMosaic{
    [audioPlayer stop];
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
        [mosaicImageView setFrame:CGRectMake(50, 50, 500, 500)];
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
    NSString *searchTerm = (indexPath.row + 1 < 10 ? [NSString stringWithFormat:@"page_img0%d.png",indexPath.row + 1]: [NSString stringWithFormat:@"page_img%d.png",indexPath.row + 1]);
    
    NSLog(@"imageName \"%@\"",searchTerm);
//    cell.imageViewCell.image = [UIImage imageNamed:searchTerm];
    
    [cell.imageViewCell setHidden:NO];
    [cell.imageViewCell setImage:[UIImage imageNamed:searchTerm]];
    //[cell.imageViewCell setBackgroundColor:[UIColor yellowColor]];

    //contentsViewer.image = [UIImage imageNamed:searchTerm];
    //[cell setBackgroundColor:[UIColor whiteColor]];
//    cell.backgroundColor = [UIColor whiteColor];
    return cell;
}


#pragma mark - UICollectionViewDelegateFlowLayout
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    
/*    NSString *searchTerm = (indexPath.row + 1 < 10 ? [NSString stringWithFormat:@"page_img0%d.png",indexPath.row + 1]: [NSString stringWithFormat:@"page_img%d.png",indexPath.row + 1]);
    
    UIImage *thumbnail = [UIImage imageNamed:searchTerm];
    
    NSLog(@"Thumbnail%d Size: %f",indexPath.row,thumbnail.size.width); = thumbnail.size.width < 100 ? thumbnail.size : 
  */  
    CGSize retval = CGSizeMake(300, 240);
    //retval.height += 35; retval.width += 35;
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
