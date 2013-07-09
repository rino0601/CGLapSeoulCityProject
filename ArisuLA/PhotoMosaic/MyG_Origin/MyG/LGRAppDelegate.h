//
//  LGRAppDelegate.h
//  MyG
//
//  Created by 이 경록 on 12. 11. 29..
//  Copyright (c) 2012년 LGR. All rights reserved.
//
#import <CoreData/CoreData.h>
#import <UIKit/UIKit.h>

@class LGRViewController;

@interface LGRAppDelegate : UIResponder <UIApplicationDelegate>{

    NSManagedObjectModel *managedObjectModel;
    NSManagedObjectContext *managedObjectContext;
    NSPersistentStoreCoordinator *persistentStoreCoordinator;

}

@property (nonatomic, retain, readonly) NSManagedObjectModel *managedObjectModel;
@property (nonatomic, retain, readonly) NSManagedObjectContext *managedObjectContext;
@property (nonatomic, retain, readonly) NSPersistentStoreCoordinator *persistentStoreCoordinator;

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) LGRViewController *viewController;

- (NSString *)applicationDocumentsDirectory;

@end
