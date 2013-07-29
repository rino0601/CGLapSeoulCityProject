//
//  ONContentsViewController.h
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

@interface ONContentsViewController : UIViewController <UICollectionViewDataSource, UICollectionViewDelegateFlowLayout, UICollectionViewDelegate, AVAudioPlayerDelegate> {
    
@private
    AVAudioPlayer* audioPlayer;
    int currentViewIndex;
    int maxViewIndex;
    
    UIView *menuBar;
    UIImageView *contentsViewer;
    UIButton *menuIcon;
    UIButton *right;
    UIButton *left;
    UITextView *subtitleView;
    
    NSArray *contentsList;
    NSArray *soundsList;
    NSArray *subsList;
    int audioIndex;
    
    NSMutableDictionary *searchResults;
    NSMutableArray *searches;
    UICollectionView *onCollectionView;
    UIView *pageView;
    
    UIImageView* mosaicImageView;
}

//@property (strong, nonatomic) IBOutlet UICollectionView *collectuinView;
@property (nonatomic, retain) IBOutlet UIImageView *contentsViewer;
@property (nonatomic, retain) IBOutlet UIView *menuBar;
@property (nonatomic, retain) IBOutlet UIButton *menuIcon;
@property (nonatomic, retain) IBOutlet UICollectionView *onCollectionView;
@property (nonatomic, retain) IBOutlet UIView *pageView;
@property (nonatomic, retain) IBOutlet UIButton *right;
@property (nonatomic, retain) IBOutlet UIButton *left;
@property (nonatomic, retain) IBOutlet UITextView *subtitleView;

@end
