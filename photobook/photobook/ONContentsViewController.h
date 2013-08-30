//
//  ONContentsViewController.h
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import "ONTextView.h"
#import "ONSnapshotCollectionViewCell.h"
#import "ONMosaicViewController.h"
#import "ONVideoViewController.h"
#import "ONAppDelegate.h"
#import "UIImage+SetAlpha.h"

@interface ONContentsViewController : UIViewController <UICollectionViewDataSource, UICollectionViewDelegateFlowLayout, UICollectionViewDelegate, AVAudioPlayerDelegate> {
    
@private
    AVAudioPlayer* audioPlayer;
    AVAudioPlayer* efectPlayer;
    AVAudioPlayer* bgmPlayer;
    AVAudioPlayer* clickEffectPlayer;
    
    int currentViewIndex;
    int maxViewIndex;
    int code;
    int bgmCounter;
    
    NSArray *contentsList;
    NSArray *soundsList;
    NSArray *subsList;
    int audioIndex;
    
    NSString *lang;
    //UIImageView* mosaicImageView;
    
    UIImage* rightBImage;
    UIImage* leftBImage;
    
}

@property (nonatomic, retain) IBOutlet UIView*           contentsView;
@property (nonatomic, retain) IBOutlet UIView*           zoomInLayer;
@property (nonatomic, retain) IBOutlet UIImageView*      contentsImageView;
@property (nonatomic, retain) IBOutlet UIImageView*      mosaicImageView;
@property (nonatomic, retain) IBOutlet UIButton*         languageButton;
@property (nonatomic, retain) IBOutlet UIView*           menuBar;
@property (nonatomic, retain) IBOutlet UIButton*         menuIcon;
@property (nonatomic, retain) IBOutlet UICollectionView* onCollectionView;
@property (nonatomic, retain) IBOutlet UIView*           pageView;
@property (nonatomic, retain) IBOutlet UIButton*         right;
@property (nonatomic, retain) IBOutlet UIButton*         left;
@property (nonatomic, retain) IBOutlet UITextView*       subtitleView;

@end
