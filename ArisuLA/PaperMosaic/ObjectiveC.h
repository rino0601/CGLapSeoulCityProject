//
//  ObjectiveC.h
//  PaperMosaic
//
//  Created by Hanjong Ko on 13. 2. 14..
//  Copyright (c) 2013년 lemonApple. All rights reserved.
//

#ifndef __PaperMosaic__ObjectiveC__
#define __PaperMosaic__ObjectiveC__

#import <UIKit/UIKit.h>
#import "cpp/MPoint.h"

void imageTestingWithIplImage(IplImage *src, const char *text);
IplImage *IplImage_imageNamed(const char *text);
void setNeedsDisplay(UIView *sender);

class CUIImageView {
	UIImageView *inst;
public:
	CUIImageView(){}
	CUIImageView(UIImageView *m_view);
	UIImageView *getInst(){return inst;}
	void GetClientRect(CGRect *rect);
	void GetWindowRect(CGRect *rect);
	void ScreenToClient(MPoint *p);
	void ClientToScreen(MPoint *p);
};

#endif /* defined(__PaperMosaic__ObjectiveC__) */
