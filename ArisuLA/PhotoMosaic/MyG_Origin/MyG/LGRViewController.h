//
//  LGRViewController.h
//  MyG
//
//  Created by 이 경록 on 12. 11. 29..
//  Copyright (c) 2012년 LGR. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Tile_Mosaic_CV.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import "sqlite3.h"

@interface LGRViewController : UIViewController
<UIActionSheetDelegate,UINavigationControllerDelegate, UIImagePickerControllerDelegate, UIPopoverControllerDelegate>{
    IBOutlet UIImageView *imageView;
    IBOutlet UIImageView *imageView2;
    IBOutlet UIImageView *imageView3;
    
    UIImage *t_im;
    UIPopoverController *m_pop;
    
    NSString *DBFilep;
    NSString *sqlDBPath;
    
    int numodDBIM;
    NSMutableArray *assetArray;
    ALAssetsLibrary *library;
    
    NSFetchedResultsController *fetchedResultsController;
    NSManagedObjectContext *managedObjectContext;
}

// Objective-C의 명명법에 맞춰 이름을 바꿉니다.
// 이거 안 지키면 Apple 심사 통과 못 해요.

- (IBAction)DBbuttonPressed:(id)sender;
- (IBAction)PhotoButtonPressed:(id)sender;
- (IBAction)GoButtonPressed:(id)sender;
- (IplImage*)attachpictures:(pointhead*)phead input:(IplImage*)input option:(int)option;
- (void)matrot:(float[2][2])rot source:(CvPoint2D32f*)src destination:(CvPoint2D32f*)dst;
- (void)getphotos;
- (ALAssetsLibrary *)defaultAssetsLibrary;

@property (nonatomic, retain) NSFetchedResultsController *fetchedResultsController;
@property (nonatomic, retain) NSManagedObjectContext *managedObjectContext;

@end
