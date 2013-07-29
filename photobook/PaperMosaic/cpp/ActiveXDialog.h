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
	// 누가 이 '프로젝트' Encapsulation 좀 해줬으면 ......
public:
	InteractiveColoredPaperMosaic icpm;
	mSemaphore sem;
	
	//Dialog 색종이 영역과 모자이크 영역을 상속 받기 위한 변수 //OnPaint, SetImage, OnLBDown등등에서 이용.
	CUIImageView m_stColortedPaper;
	CUIImageView m_stMosaic;
	
	int viewx;
	int viewy;
	
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
	
	//lemonApple added.
	CGLView OpenGLView;
	PaperMosaicCanvas 	*m_view;
	CPaperMosaicCanvas	*c_view;
	BOOL 				isWait;
	int startSectionCode = 101;
public:
	ActiveXDialog();
	~ActiveXDialog();
	void init(PaperMosaicCanvas *view, CUIImageView p, CUIImageView m, IplImage *edge, IplImage *source, IplImage *mask, int code); //OnSize, OnInit역할
	int getStartSectionCode();
	
	void getBackBuffer(IplImage* buffer);
	void Clear(float r=1.0f, float g=1.0f, float b=1.0f);
	void DrawCone(int x, int y, Byte r, Byte g, Byte b, int depth = 0);
	void SetImage(IplImage *pImage1, IplImage *pImage2, IplImage *pImage3);
	void ConvertColor();
	void AutoTileMove();
	void PasteImg(int x, int y, IplImage* img);
	
	// 조작법. 성능에는 좋지 않지만 빠른 개발을 위해 그냥 그대로 씀.
	void OnLButtonDown(MPoint point);
	void OnLButtonUp(MPoint point);
	void OnMouseMove(MPoint point); 
};
int ANGLE(int x);
int roundRoutine(int i, int n);
#endif
