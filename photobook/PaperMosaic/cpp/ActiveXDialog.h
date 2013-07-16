#ifndef PaperMosaic_global_h
#define PaperMosaic_global_h

#define random(num) 		(arc4random()%(num))
#define COS(x)				cos((x)*M_PI/180.0)
#define SIN(x)				sin((x)*M_PI/180.0)

#ifndef max
#define max(a,b) 			( ( (a) > (b) ) ? (a) : (b) )
#endif

#ifndef min
#define min(a,b) 			( ( (a) < (b) ) ? (a) : (b) )
#endif

#import "mSemaphore.h"
#import "InteractiveColoredPaperMosaic.h"
#import "MPoint.h"
#import <opencv2/opencv.hpp>
#import "../opengles/GLView.h"
#import "../ObjectiveC.h"
#import "PaperMosaicCanvas.h"

@class PaperMosaicCanvas;
class CPaperMosaicCanvas;

class ActiveXDialog {
	// 이름이 겁나 마음에 안들지만 어쩔수 없지...
	// 완성하고서 이름을 바꾼다.
	
	// 이하 주석처리된 멤버변수는 필요없어졌거나, 포팅해야 하는 경우.
public: //이게 public이라니.. 한숨.. old
	InteractiveColoredPaperMosaic icpm;
	mSemaphore sem;
	
	//Dialog 색종이 영역과 모자이크 영역을 상속 받기 위한 변수 //OnPaint, SetImage, OnLBDown등등에서 이용.
	// CStatic m_stColortedPaper; //UIView 상속해서 구현 해야 할듯.
	// CStatic m_stMosaic;
	CUIImageView m_stColortedPaper;
	CUIImageView m_stMosaic;
	
	//안쓰거나, 1-1=0 이라던가 해서 무용지물이더라도 일단 작성..
	int viewx;
	int viewy;
	// HGLRC m_hRC;
	// HDC m_hDC;
	
	// CBitmap m_Bitmap;
	
	//CvvImage m_cvvEdgeImg;
	//CvvImage m_cvvColordPaper;
	//CvvImage m_cvvMosaic;
	
	IplImage* m_pColordPaper;
	IplImage* m_pMosaic;
	IplImage* m_pEdge;
	IplImage* m_psource;
	IplImage* gray;
	IplImage* dis;
	IplImage* reSizedSource;
	
	Byte m_Color_R;
	Byte m_Color_G;
	Byte m_Color_B;				
	
	MPoint previousP;
	MPoint CurP;
	MPoint m_tilePoint;
	MPoint m_pMovePoint;
	
	bool isLBdown;              // LButtondown : true , LButtonup : false
	bool TileMove;				// LButtondown : true , LButtonup : false (타일 선택후 이동시 사용)
	bool m_bReSize;				// 타일 크기 조절, true : 축소, false : 확대
	bool m_bAutoGeneration;		// 자동붙이기	    true : 자동붙이기, false : 자동붙이기 취소
	bool m_bClipping;			// 에지 클리핑
	bool m_bAutoTileMove;		// 자동붙이기	    true : 자동붙이기, false : 자동붙이기 취소
	bool m_bArea;				// true: coloredpaper , false: mosaic
	
	int m_nImgBT;				// 색종이 색상 활성화(PNG 이미지 사용)
	int m_nMenuBT;				// 메뉴 버튼 활성화(png 이미지 사용)
	int m_nNum;					// tile number
	int m_nFileNum;				// 자동 붙이기시 파일명을 위한 변수
	
	float m_fside;				// 타일 지름
	
	CvScalar m_Color;
	
	vector<MPoint> AutoMovePoint;	// 자동 붙이기시 타일 이동 좌표 저장
	int m_nTileSpeed;				// 자동 붙이기시 타일 이동 속도
	
	//ULONG_PTR m_gdiplusToken; 	//Gdiplus(png 이미지 사용 위해)
	
	//lemonApple added.
	CGLView OpenGLView;
	PaperMosaicCanvas 	*m_view;
	CPaperMosaicCanvas	*c_view; //same...
	BOOL 				isWait;

public:
	ActiveXDialog();
	~ActiveXDialog();
	void init(PaperMosaicCanvas *view, CUIImageView p, CUIImageView m); //OnSize, OnInit역할
	
	void getBackBuffer(IplImage* buffer);
	void Clear(float r=1.0f, float g=1.0f, float b=1.0f);
	void DrawCone(int x, int y, Byte r, Byte g, Byte b, int depth = 0);
	void SetImage();
	void ConvertColor();
	void AutoTileMove();
	// void SetColorButton(CDC* dc);
	// void SetMenuButton(CDC* dc);
	void PasteImg(int x, int y, IplImage* img);
	// void Scatter();
	
	// 조작법. 성능에는 좋지 않지만 빠른 개발을 위해 그냥 그대로 씀.
	void OnLButtonDown(MPoint point);
	void OnLButtonUp(MPoint point);
	void OnMouseMove(MPoint point); // nFlags쓴다....만 안써도 되게 따로 변수도 만들어 뒀네; 한숨 나온다.
	
	// 그외 포팅을 위해 필요한 method.
};
int ANGLE(int x);
int roundRoutine(int i, int n);
#endif
