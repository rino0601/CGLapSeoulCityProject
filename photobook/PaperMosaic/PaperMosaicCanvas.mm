//
//  PaperMosaicCanvas.m
//  PaperMosaic
//
//  Created by Hanjong Ko on 13. 1. 11..
//  Copyright (c) 2013년 lemonApple. All rights reserved.
//

#import "PaperMosaicCanvas.h"
#import "UIImageCVArrConverter.h"
#import "ONAppDelegate.h"

@implementation PaperMosaicCanvas
@synthesize Paper,Mosaic,ADelegate;

- (void)layoutSubviews {
	[super layoutSubviews];
	ADelegate = new ActiveXDialog();
	CUIImageView paper(Paper),canvas(Mosaic);
	ONAppDelegate *appDelegate = [[UIApplication sharedApplication] delegate];
	ADelegate->init(self,paper,canvas,
					[UIImageCVArrConverter CreateIplImageFromUIImage:appDelegate.mosaicEdge],
					[UIImageCVArrConverter CreateIplImageFromUIImage:appDelegate.mosaicSource],
					[UIImageCVArrConverter CreateIplImageFromUIImage:appDelegate.mosaicMask],
					appDelegate.startSectionCode);
	[self setMultipleTouchEnabled:YES];
}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *tch = [touches anyObject];
	touchPos = [tch locationInView:self];
	ADelegate->OnLButtonDown(MPoint(touchPos.x, touchPos.y));
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *tch = [touches anyObject];
	touchPos = [tch locationInView:self];
	ADelegate->OnLButtonUp(MPoint(touchPos.x, touchPos.y));
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *tch = [touches anyObject];
	touchPos = [tch locationInView:self];
	ADelegate->OnMouseMove(MPoint(touchPos.x, touchPos.y));
}
- (void)drawRect:(CGRect)rect {
	//더블 버퍼링을 위한 Graphic Context 할당
	CGColorSpaceRef cs=CGColorSpaceCreateDeviceRGB();
	ctx = CGBitmapContextCreate(NULL,
								(int)[self bounds].size.width,
								(int)[self bounds].size.height,
								8,
								4*(int)[self bounds].size.width,
								cs,
								kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(cs);
	
	// 모자이크 영역에 밑그림(에지) && 색종이 조각 그리기
	CGContextTranslateCTM(ctx, 0, Mosaic.bounds.size.height); CGContextScaleCTM(ctx, 1.0, -1.0);
	CGContextDrawImage(ctx, Mosaic.frame, [[UIImageCVArrConverter UIImageFromIplImage:ADelegate->m_pEdge] CGImage]); 	//밑그림(에지)
	/* 웃긴게, 엣지를 버려도 엣지가 그려진다?! 게다가 그릴때 깜빡이는것도 사라짐. 허헣ㅎ허허*/
	CGContextDrawImage(ctx, Mosaic.frame, [[UIImageCVArrConverter UIImageFromIplImage:ADelegate->m_pMosaic] CGImage]); 	//색종이 조각 그리기
	CGContextTranslateCTM(ctx, 0, Mosaic.bounds.size.height); CGContextScaleCTM(ctx, 1.0, -1.0);
	
	// 색종이 영역 그리기
	CGContextTranslateCTM(ctx, 0, Paper.bounds.size.height); CGContextScaleCTM(ctx, 1.0, -1.0);
	CGContextDrawImage(ctx, Paper.frame, [[UIImageCVArrConverter UIImageFromIplImage:ADelegate->m_pColordPaper] CGImage]);
	CGContextTranslateCTM(ctx, 0, Paper.bounds.size.height); CGContextScaleCTM(ctx, 1.0, -1.0);
	
	if(ADelegate->m_bAutoTileMove || ADelegate->m_bAutoGeneration) { // 자동 붙이기시 타일 이동 (색종이 영역 -> 모자이크 영역)
		if(ADelegate->icpm.m_oCPM.m_optile) {
			ADelegate->icpm.DrawAutoTileAt(ctx, ADelegate->m_pMovePoint);
		}
	} else if(ADelegate->icpm.m_opSelectedTile) { // 선택한 타일 이동 (수동 상태 일때.)
		int w,h;
		w = self.frame.size.width+10;
		h = self.frame.size.height+10;
		if(w < ADelegate->CurP.x || h < ADelegate->CurP.y || -10 > ADelegate->CurP.x ||	-10 > ADelegate->CurP.y ) {
			ADelegate->icpm.m_opSelectedTile = NULL;
		} else {
			ADelegate->icpm.DrawSelectedTileAt(ctx, ADelegate->CurP - ADelegate->icpm.dif_Posi);
		}
	}
	
	//화면에 그리기
	CGImageRef i=CGBitmapContextCreateImage(ctx);
	CGContextDrawImage(UIGraphicsGetCurrentContext(), [self bounds], i);
	CGImageRelease(i);
	
	// 다 그렸으니 메모리 해제.
	CGContextRelease(ctx);
	return ;
}
- (void)thread_AutoGeneration:(id)object {
	ADelegate->m_nMenuBT = 6;
	ADelegate->m_bAutoGeneration = true;
	
	ADelegate->icpm.AutoGeneration(ADelegate); //thread필요.
	
	ADelegate->icpm.m_opSelectedTile= NULL;
	ADelegate->m_bAutoGeneration = false;
	ADelegate->m_nMenuBT = -1;
	[self setNeedsDisplay];
	[NSThread exit];
}
- (void)doAsAutoMode {
	// 자동 붙이기
	if(!ADelegate->m_bAutoGeneration) {
		UIAlertView *alertview = [[UIAlertView alloc] initWithTitle:@"계산 중..." message:nil delegate:self cancelButtonTitle:nil otherButtonTitles:nil];
		UIActivityIndicatorView *activityIndicator = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(130.0f, 50.0f, 20.0f, 20.0f)/*(130, 55, 30,30)*/];
		[activityIndicator startAnimating];
		[alertview addSubview:activityIndicator];
		[alertview show];
		[NSThread detachNewThreadSelector:@selector(thread_AutoGeneration:) toTarget:self withObject:nil];
	} else { // 자동 붙이기 취소
		ADelegate->m_nMenuBT = -1;
		ADelegate->m_bAutoGeneration = false;
		
		// 마지막 타일이 붙여 지고 타이머를 죽임.
		ADelegate->icpm.AutoGeneration(ADelegate);
		
		ADelegate->m_bAutoTileMove = false;
	}
}
- (void)OnTimer:(NSTimer *)timer {
	int size = ADelegate->AutoMovePoint.size();
	//자동 붙이기 시 타일 이동 좌표 설정
	if(ADelegate->m_nNum < size) {
		ADelegate->m_pMovePoint = ADelegate->AutoMovePoint[ADelegate->m_nNum];
		ADelegate->m_nNum = ADelegate->m_nNum + ADelegate->m_nTileSpeed;
		[self setNeedsDisplay];
	} else {
		ADelegate->m_bAutoTileMove = false;
		ADelegate->AutoMovePoint.clear();
		[self setNeedsDisplay];
		[timer invalidate];
		timer = nil;
		ADelegate->isWait=false;
	}
}
- (BOOL)setPaperColor:(UIColor *)color {
	CGFloat red,green,blue,alpha;
	[color getRed:&red green:&green blue:&blue alpha:&alpha];
	if(alpha!=1.0f) {
		@throw [NSException exceptionWithName:@"PaperMosaicCanvas_Exception" reason:@"alpha value of PaperColor must be 1" userInfo:nil];
		return NO;
	}
	ADelegate->m_Color_R = (Byte)(255*red);
	ADelegate->m_Color_G = (Byte)(255*green);
	ADelegate->m_Color_B = (Byte)(255*blue);
	ADelegate->m_Color = cvScalar(ADelegate->m_Color_R,ADelegate->m_Color_G,ADelegate->m_Color_B);
	ADelegate->ConvertColor();
	return YES;
}
- (void) SetTimer:(NSNumber *) second {
	ADelegate->isWait = true;
	[NSTimer scheduledTimerWithTimeInterval:[second doubleValue] target:self selector:@selector(OnTimer:) userInfo:nil repeats:YES];
}

- (void)didPresentAlertView:(UIAlertView *)alertView {
	static UIAlertView *marker = nil;
	if(alertView==nil) {
		[marker performSelectorOnMainThread:@selector(dismissWithClickedButtonIndex:animated:) withObject:nil waitUntilDone:NO];
	}
	marker = alertView ;
}
- (void)setMosaicImage:(UIImage *)source Edge:(UIImage *)edge Mask:(UIImage *)mask{
	[self setMosaicImage:source Edge:edge Mask:mask StartSection:102];
}
- (void)setMosaicImage:(UIImage *)source Edge:(UIImage *)edge Mask:(UIImage *)mask StartSection:(NSUInteger)sectionCode {
	
}

- (UIImage*)resultImage{
    return [UIImageCVArrConverter UIImageFromIplImage:ADelegate->m_pMosaic];
}
- (void)saveResult {
	UIImage *image =[UIImageCVArrConverter UIImageFromIplImage:ADelegate->m_pMosaic];
	UIImageWriteToSavedPhotosAlbum(image, self, @selector(image:finishedSavingWithError:contextInfo:), nil);
}
- (void)image:(UIImage *)image finishedSavingWithError:(NSError *)error contextInfo:(void *)contextInfo {
	if(error) {
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"save fail" message:@"fail to save image" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
		[alert show];
	} else {
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"info" message:@"결과이미지가 저장되었습니다." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
		[alert show];
	}
}
- (void)didMoveToSuperview{
    [super didMoveToSuperview];
//    if(ADelegate != NULL)        delete ADelegate;
}

/*- (void) dealloc {
    delete(ADelegate);
}*/

@end


void CPaperMosaicCanvas::setTimerWithTimeInterval(NSTimeInterval second) {
	[instance performSelectorOnMainThread:@selector(SetTimer:) withObject:[NSNumber numberWithDouble:second] waitUntilDone:YES];
}

void CPaperMosaicCanvas::killAlert() {
	[instance didPresentAlertView:nil];
}