//
//  ONModelController.h
//  ON
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ONDataViewController;

@interface ONModelController : NSObject <UIPageViewControllerDataSource>

- (ONDataViewController *)viewControllerAtIndex:(NSUInteger)index storyboard:(UIStoryboard *)storyboard;
- (NSUInteger)indexOfViewController:(ONDataViewController *)viewController;

@end
