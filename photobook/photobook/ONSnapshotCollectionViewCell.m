//
//  ONSnapshotCollectionViewCell.m
//  photobook
//
//  Created by SeulBeom on 13. 7. 18..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "ONSnapshotCollectionViewCell.h"

@implementation ONSnapshotCollectionViewCell

@synthesize imageViewCell;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        imageViewCell = [[UIImageView alloc] initWithFrame:self.contentView.bounds];
        [self.contentView addSubview:imageViewCell];
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/
- (void) prepareForReuse
{
    [super prepareForReuse];
    //self.imageViewCell.image = nil;
    //self.imageViewCell.frame = self.contentView.bounds;
}
                         
@end
