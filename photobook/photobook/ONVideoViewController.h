//
//  ONVideoViewController.h
//  photobook
//
//  Created by SeulBeom on 13. 8. 21..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>

/*struct sub {
    CMTime time;
    NSString *string;
}Sub;
*/
@interface ONVideoViewController : UIViewController {
    BOOL isPlay;
    BOOL isOnSubs;
    AVPlayer *player;
    int playIndex;
    NSTimer *playTimer;
    CMTime maxPlayTime;
    CMTime minPlayTime;
    NSMutableArray *subData;
}

@property (nonatomic, retain) IBOutlet UIView* videoView;
@property (nonatomic, retain) IBOutlet UITextView* subtitleView;
@property (nonatomic, retain) IBOutlet UIButton* playButton;
@property (nonatomic, retain) IBOutlet UISlider* playbackSlider;
@property (nonatomic, retain) IBOutlet UIButton* subtitleButton;

@end
