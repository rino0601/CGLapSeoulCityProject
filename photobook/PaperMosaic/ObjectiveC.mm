//
//  ObjectiveC.cpp
//  PaperMosaic
//
//  Created by Hanjong Ko on 13. 2. 14..
//  Copyright (c) 2013년 lemonApple. All rights reserved.
//

#import "ObjectiveC.h"
#import "UIImageCVArrConverter.h"

class ActiveXDialog;

IplImage *IplImage_imageNamed(const char *text) {
	//FIXME: dead code. 지워도 좋습니다. 안쓸거라면요.
	IplImage *loadImage = [UIImageCVArrConverter CreateIplImageFromUIImage:[UIImage imageNamed:[NSString stringWithCString:text encoding:NSUTF8StringEncoding]]];
	cvFlip(loadImage);
	return loadImage;
}
void setNeedsDisplay(UIView *sender) {[sender setNeedsDisplay];}

CUIImageView::CUIImageView(UIImageView *m_view) {
	this->inst = m_view;
}

void CUIImageView::GetClientRect(CGRect *rect) {
	*rect = inst.bounds;
}

void CUIImageView::GetWindowRect(CGRect *rect) {
	// 일단 부모 view로 부터 얻어지는 좌표로.
	*rect = inst.frame;
	// window에서 얻어와야 할 수도 있다. // 아닐 가능성이 더 높음.
}

void CUIImageView::ScreenToClient(MPoint *p) {
	CGPoint point = CGPointMake(p->x, p->y);
	point = [inst convertPoint:point fromView:inst.superview];
	*p = MPoint(point.x, point.y);
}

void CUIImageView::ClientToScreen(MPoint *p) {
	CGPoint point = CGPointMake(p->x, p->y);
	point = [inst convertPoint:point toView:inst.superview];
	*p = MPoint(point.x, point.y);
}
