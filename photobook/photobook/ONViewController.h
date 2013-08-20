//
//  ONViewController.h
//  photobook
//
//  Created by SeulBeom on 13. 7. 15..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

@interface ONViewController : UIViewController <AVAudioPlayerDelegate> {
    AVAudioPlayer* startAudio;
}

- (IBAction)doStartButton:(id)sender;
- (IBAction)home:(UIStoryboardSegue *)segue;

@end
