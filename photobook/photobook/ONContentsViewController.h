//
//  ONContentsViewController.h
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ONContentsViewController : UIViewController <UICollectionViewDataSource, UICollectionViewDelegateFlowLayout, UICollectionViewDelegate> {
    int currentViewIndex;
    int maxViewIndex;
    
    Boolean isMenuOn;
    
    UIView *menuBar;
    UIImageView *contentsViewer;
    UIButton *menuIcon;
    
    NSArray *contentsList;
    
    NSMutableDictionary *searchResults;
    NSMutableArray *searches;
    UICollectionView *onCollectionView;
    UIView *pageView;
}

//@property (strong, nonatomic) IBOutlet UICollectionView *collectuinView;
@property (nonatomic, retain) IBOutlet UIImageView *contentsViewer;
@property (nonatomic, retain) IBOutlet UIView *menuBar;
@property (nonatomic, retain) IBOutlet UIButton *menuIcon;
@property (nonatomic, retain) IBOutlet UICollectionView *onCollectionView;
@property (nonatomic, retain) IBOutlet UIView *pageView;

@end
