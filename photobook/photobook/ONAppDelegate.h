//
//  ONAppDelegate.h
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ONAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property UIImage *mosaicSource, *mosaicEdge, *mosaicMask;
@property NSUInteger startSectionCode;
@property NSString *lang;
@property NSInteger playIndex;

@end
