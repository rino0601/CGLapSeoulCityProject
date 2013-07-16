//
//  ONContentsViewController.h
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ONContentsViewController : UIViewController {
    int currentViewIndex;
    int maxViewIndex;
    
    Boolean isMenuOn;
    
    UIView *menuBar;
    UIImageView *contentsViewer;
    UIButton *menuIcon;
    
    NSArray *contentsList;
}

@property (nonatomic, retain) IBOutlet UIImageView *contentsViewer;
@property (nonatomic, retain) IBOutlet UIView *menuBar;
@property (nonatomic, retain) IBOutlet UIButton *menuIcon;

@end
