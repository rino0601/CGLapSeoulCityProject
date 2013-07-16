//
//  UIImageCVMatConverter.h
//  DrawPicture
//
//  Created by Hanjong Ko on 12. 11. 1..
//  Copyright (c) 2012년 lemonApple. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UIImageCVArrConverter : NSObject {
	
}

+ (UIImage *)UIImageFromCVMat:(cv::Mat)cvMat;
+ (UIImage *)UIImageFromCVMat:(cv::Mat)cvMat withUIImage:(UIImage*)image;
+ (cv::Mat)cvMatFromUIImage:(UIImage *)image;
+ (cv::Mat)cvMatGrayFromUIImage:(UIImage *)image;
+ (UIImage *)scaleAndRotateImageFrontCamera:(UIImage *)image;
+ (UIImage *)scaleAndRotateImageBackCamera:(UIImage *)image;
+ (IplImage *)CreateIplImageFromUIImage:(UIImage *)image;
+ (UIImage *)UIImageFromIplImage:(IplImage *)image; // didn't work

@end

