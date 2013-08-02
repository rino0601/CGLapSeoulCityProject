//
//  UIImage+setalpha.m
//  photobook
//
//  Created by SeulBeom on 13. 8. 2..
//  Copyright (c) 2013년 SeulBeom. All rights reserved.
//

#import "UIImage+SetAlpha.h"

@implementation UIImage (SetAlpha)

- (UIImage *)imageByApplyingAlpha:(CGFloat) alpha{
    UIGraphicsBeginImageContextWithOptions(self.size, NO, 0.0f);
    
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    CGRect area = CGRectMake(0, 0, self.size.width, self.size.height);
    
    CGContextScaleCTM(ctx, 1, -1);
    CGContextTranslateCTM(ctx, 0, -area.size.height);
    
    CGContextSetBlendMode(ctx, kCGBlendModeMultiply);
    
    CGContextSetAlpha(ctx, alpha);
    
    CGContextDrawImage(ctx, area, self.CGImage);
    
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    
    UIGraphicsEndImageContext();
    
    return newImage;
}

@end
