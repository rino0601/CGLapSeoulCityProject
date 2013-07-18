#import "ActiveXDialog.h"
#import <math.h>
#import "xDistanceTransform.h"
#import "../ObjectiveC.h"

int ANGLE(int x) {
	int ret = x;
	while (ret >= 360 || ret <0) {
		if( ret < 0 )
			ret += 360;
		if( ret >=360 )
			ret -= 360;
	}
	return ret;
}

int roundRoutine(int i, int n){
	return i>=n ? 0:(i < 0 ? n-1 : i);
}

// class implement
ActiveXDialog::ActiveXDialog(){
	// MFC 이벤트 메세지를 따라가지 않음으로 인해서 생성자에서 해야 할 것이 많아짐.
	//그러나, 생성 시점을 맞추기가 너무 힘들다. init함수로 대체.. 아 이거 안좋은데 ㅠㅠ 그런데 ㅠㅠ.
	// OnCreate, OnSize, OnInitDialog, 등을 여기에서 처리해야 할 것 같다.
	// 위에 3개 함수는 포팅할때 1:1 변환 생각하지말고, 코드 이해하고 새로 만드는게 맞는것 같음.
	isLBdown			=	false;
	TileMove			=	false;
	m_bReSize			=	false;
	m_nImgBT			=	-1;
	m_nMenuBT 			=	-1;
	m_bAutoTileMove		=	false;
	m_bArea				=	false;
	m_bAutoGeneration	=	false;
	m_bClipping			=	true;
	
	m_Color 			=	cvScalar(255,255,0);//cvScalar(255,255,255); 처음에 흰색인게 맘에 안들어서 노란색으로 바꿈.
	m_pColordPaper 		=	NULL;
	m_pMosaic 			=	NULL;
	m_pEdge 			=	NULL;
	gray				=	NULL;
	m_nTileSpeed		=	70;
	m_nFileNum			=	0;
	isWait				= 	false;
}
ActiveXDialog::~ActiveXDialog() {
	if(m_pColordPaper)	cvReleaseImage(&m_pColordPaper);
	if(m_pMosaic)		cvReleaseImage(&m_pMosaic);
	if(m_pEdge)			cvReleaseImage(&m_pEdge);
	if(m_psource)		cvReleaseImage(&m_psource);
	if(gray)			cvReleaseImage(&gray);
	if(dis)				cvReleaseImage(&dis);
	if(!AutoMovePoint.empty()) AutoMovePoint.clear();
}
void ActiveXDialog::init(PaperMosaicCanvas *view, CUIImageView p, CUIImageView m) { //OnSize, OnInit역할
	m_view 				= view;
	c_view  			= new CPaperMosaicCanvas(view);
	m_stColortedPaper 	= p;
	m_stMosaic 			= m;
	//OnSize()먼저.
	viewx = view.frame.size.width;
	viewy = view.frame.size.height;
	// 도화지 관련 if문 자리.
	OpenGLView.AllocInitWithFrame(p.getInst().frame);
	//OnInitDialog()차례.
	icpm.m_oPaper.setDlg(this);
	icpm.m_oCPM.setDlg(this);
	icpm.setDlg(this);
	
	this->SetImage();
	
	icpm.SetColoredPaper();
	icpm.m_oPaper.Render(m_pColordPaper);
}

// OpenGL 
void ActiveXDialog::getBackBuffer(IplImage * buffer) {
	// 이 메서드를 부르는 메서드 : getVoronoi(), setEdgeDistanceMap()
	
	// 이걸 호출하고 바로 밑에 memcpy for문 이 있는걸 보면, 일일히 뒤집는것 같다.
	// 왜 그렇게 짰는지 이해 불가.. 지만 혹시 나중에 결과물이 뒤집혀서 나오면, cvFlip을 뺄 것.

	OpenGLView.getBufferImage(buffer);
}
void ActiveXDialog::Clear(float r, float g, float b){
	// DrawCone을 부르기 전에, 한번 부르는 것 같다.
	// 따라서, DrawCone은 하나하나 찍어두고, 한번에 그리는 것 같다.
	OpenGLView.Clear(r, g, b);
}
void ActiveXDialog::DrawCone(int x, int y, Byte r, Byte g, Byte b, int depth){
	// void에서 바꾸기는 사실상 불가능. 그러느니 차라리 전역변수를 하나 쓰는게;
	// 파라미터 고치고 싶은데 고칠수가 없다.... 쓰는 방법좀 보소 -_-;
	
	// 필요한 전역 변수 : MFC 프로젝트에는 있지만 iOS에서는 쓸 수 없다. 순수 OpenGL 코드.
	// 이 메서드를 부르는 메서드 : getVoronoi(), setEdgeDistanceMap()
	// 트렌젝션 구분이 절실하다!.
	OpenGLView.DrawCone(x, y, r, g, b, depth);
	// 도대체 몇 중첩이야 이거 ㅋㅋㅋ.
}

void ActiveXDialog::SetImage() {
//	CDC* pdc;				// bmp와 마찬가지. 필요 없다.
	CGRect rect;			
//	CBitmap  bmp;			// 확실히 필요 없어보임.
	IplImage *pImage1;
	IplImage *pImage2;
	IplImage *pImage3;
	
	IplImage* mask;
	CvSize img_size;
	
	// edge image setting
	pImage1 = IplImage_imageNamed("edge_img.bmp");
	img_size.height = pImage1->height;
	img_size.width = pImage1->width;
	m_pEdge =cvCreateImage(img_size,IPL_DEPTH_8U, 4);
	cvFlip(pImage1, m_pEdge, 0);
		
	// source image setting
	pImage2 = IplImage_imageNamed("/source_img.bmp");
	m_psource = cvCreateImage(img_size,IPL_DEPTH_8U, 4);
	cvFlip(pImage2, m_psource, 0);

	// mask image setting
	pImage3 = IplImage_imageNamed("/mask_img.bmp");
	mask = cvCreateImage(img_size,IPL_DEPTH_8U, 4);
	cvFlip(pImage3, mask, 0);
	
	///////////////////////////////////////////////////////
	
	int w=0 ,h=0;
	
	//색종이 역역 랜더링을 위한 IplImage(m_pColordPaper)생성
	m_stColortedPaper.GetClientRect(&rect);
	w = (int)CGRectGetWidth(rect);
	h = (int)CGRectGetHeight(rect);
	m_pColordPaper = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
	
	m_fside =  sqrt((float)(w*h)/ 81);																//타일 지름
	icpm.m_oCPM.m_nTileRadius =  (int)((sqrt((m_fside*m_fside) + (m_fside*m_fside)) / 2)+ 0.5) ;	//타일 반지름

	//모자이크 영역 랜더링을 위한 IplImage(m_pMosaic)생성
	m_stMosaic.GetClientRect(&rect);
	w = (int)CGRectGetWidth(rect);
	h = (int)CGRectGetHeight(rect);
	m_pMosaic = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
	IplImage *t_pEdge = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
	cvResize(m_pEdge, m_pMosaic,CV_INTER_LINEAR);
	cvResize(m_pEdge, t_pEdge,CV_INTER_LINEAR);
	m_pEdge = t_pEdge; t_pEdge = NULL;
	
	//ColoredPaperMosaic 클래스에서 사용할 이미지들 생성 및 초기화.	
	gray = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	cvCvtColor(m_pMosaic, gray, CV_RGB2GRAY);
	
	reSizedSource = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
	cvResize(m_psource, reSizedSource, CV_INTER_LINEAR);
	
	
	xDistanceTransform dt;
	dis = dt.GetDT(gray);
	icpm.m_oCPM.m_vpEdges.push_back(dis);
	icpm.m_oCPM.m_pSource = reSizedSource;
	
	icpm.m_oCPM.LayerMask = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	IplImage* reSizedMask = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
	cvResize(mask, reSizedMask, CV_INTER_LINEAR);
	cvCvtColor(reSizedMask, icpm.m_oCPM.LayerMask, CV_RGB2GRAY);
	
	// 자동 붙이기를 위한 맵 생성
	icpm.m_oCPM.preSetting(0.3);
	
	
	//에지 탄젠트 플로우(ETF:Edge Tangent Flow), 에지의 방향성 검출, 자동 붙이기 시 사용
	/*
	icpm.m_oCPM.m_pDirectionAngle = cvCreateImage(cvGetSize(m_pMosaic), IPL_DEPTH_32F, 1);
	icpm.m_oCPM.m_pLICfromImage = cvCreateImage(cvGetSize(m_pMosaic), IPL_DEPTH_32F, 1);
	icpm.m_oCPM.m_pLICfromImage = icpm.m_oCPM.getLICfromImage(m_pMosaic, 8, 6);
	icpm.m_oCPM.m_pDirectionAngle = icpm.m_oCPM.getDirectionAngle(icpm.m_oCPM.m_pLICfromImage);
	*/
	
	// icpm.m_oCPM.m_pDirectionAngle = IplImage_imageNamed("icpm.m_oCPM.m_pDirectionAngle.PNG");
	// 이미지 넣어서 해봤으나 실패. 일단 만들어논 ETF는 분명 그레이스케일로 1채널이어야 하는데, 변환은 RGB로 되어버린다;
	// 강제로 그레이스케일로 맞추면 그냥 까만 이미지가 나온다.
	// 이미지 넣고하면 자동붙이기가 작동을 안함.
	
	icpm.m_oCPM.m_pDirectionAngle = cvCreateImage(cvGetSize(m_pMosaic), IPL_DEPTH_32F, 1);
	// ETF는 쓰지 않지만, 일단 메모리 참조하는 부분이 있어서...
	
	// 솔직히 큰 차이도 안나는데 필요하나 이거?
	
	cvReleaseImage(&pImage1);
	cvReleaseImage(&pImage2);
	cvReleaseImage(&pImage3);
	cvReleaseImage(&mask);
	cvReleaseImage(&reSizedMask);
}
void ActiveXDialog::ConvertColor() {
	icpm.m_oPaper.Render(m_pColordPaper);
	setNeedsDisplay(m_view);
}
void ActiveXDialog::AutoTileMove() {
	MPoint PapperPosition;    // 색종이 타일 위치
   	MPoint tilePosition;      // 모자이크 타일 위치
   	MPoint p;
   	
   	double vecX, vecY, Length;
   	int angle;
	
   	tilePosition.x = icpm.m_oCPM.tilePosition.x;
   	tilePosition.y = icpm.m_oCPM.tilePosition.y;
   	
	//좌표계 변경
   	m_stMosaic.ClientToScreen(&tilePosition);
	
	// 색종이 영역에 생성된 마지막 색종이 부터 붙이기(색종이 영역 오른쪽 하단 색종이 조각 부터)
   	for(int i = icpm.m_oPaper.m_nTile - 1 ; i >= 0; i--) {
   		if(i== 0) {
   			icpm.SetColoredPaper();
   			icpm.m_oPaper.Render(m_pColordPaper);
   			setNeedsDisplay(m_view);
   			i = icpm.m_oPaper.m_nTile - 1;
   		}
		
		// 찢지 않은 색종이 조각
   		if(!icpm.m_oPaper.m_pTiles[i].m_bCutted) {
   			PapperPosition.x = icpm.m_oPaper.m_pTiles[i].x ;
   			PapperPosition.y = icpm.m_oPaper.m_pTiles[i].y ;
			
			// 색종이 영역 효과
   			icpm.CutAPaper(PapperPosition);
   			setNeedsDisplay(m_view);
			
			// 좌표계 변경
   			m_stColortedPaper.ClientToScreen(&PapperPosition);
			
   			break;
   		}
   	}
	
   	vecX = PapperPosition.x - tilePosition.x ;
   	vecY = PapperPosition.y - tilePosition.y;
	
   	//normalize
   	Length = vecX*vecX + vecY*vecY;
   	Length = sqrt( Length );
   	vecX = vecX/Length;
   	vecY = vecY/Length;
	
	//base 벡터와 비교하여 angle 계산
   	angle = acos(vecX) * 180 / 3.14;
   	if( vecY < 0){
   		angle =  360 - angle;
   	}
   	
	//AutoMovePoint 저장
   	m_bAutoTileMove = true;
   	AutoMovePoint.clear();
	
   	for(int i = Length; i > 0 ; i-- )
   	{
   		p.x = tilePosition.x + i * ( COS( ANGLE( angle)));
   		p.y = tilePosition.y + i * ( SIN( ANGLE( angle)));
   		AutoMovePoint.push_back(p);
   	}
	
	// 초기화
   	m_nNum = 0;
	m_pMovePoint = AutoMovePoint[0];
  	
	// 여기까지 움직일 위치 계산 함. //
	c_view->setTimerWithTimeInterval(0);
	while(isWait) {
		[NSThread sleepForTimeInterval:0.015];
	}
}
void ActiveXDialog::PasteImg(int x, int y, IplImage *img) {
	
}
void ActiveXDialog::OnLButtonDown(MPoint point) {
	
	if(m_bAutoGeneration == false) { // 자동 붙이기 타일 이동 및 붙이기 막기 위함.
		//현재 좌표와 이전 좌표 설정.
		previousP = CurP;
		CurP = point;
		
		MPoint p1,p2;
		CGRect ColoredPaper_rect, Mosaic_rect;
		
		p1 = point;
		p2 = point;
		
		m_stMosaic.GetWindowRect(&Mosaic_rect);
		
		m_stColortedPaper.GetWindowRect(&ColoredPaper_rect);
		
		// ColoredPaper 영역
		m_stColortedPaper.ScreenToClient(&p1); // 현재 포인트를 색종이 내부 영역 좌표계로 변경
		if(CGRectContainsPoint(ColoredPaper_rect, CGPointMake(point.x, point.y))) { //ColoredPaper_rect.PtInRect(point)
			m_bArea = true;
			icpm.CutAPaper(p1);	   // 색종이 조각 생성
		}
		
		// 모자이크 영역
		m_stMosaic.ScreenToClient(&p2);
		if(CGRectContainsPoint(Mosaic_rect, CGPointMake(point.x, point.y))) { //Mosaic_rect.PtInRect(point)
			if( icpm.m_oCPM.m_pTileImageMask !=NULL && -1 < (CV_IMAGE_ELEM(icpm.m_oCPM.m_pTileImageMask, float, p2.y, p2.x))) { // 모자이크 영역 일때 색종이 붙임
				m_bArea = false;
				m_tilePoint = p2;
				icpm.AttachPaper(p2);				
			} else {
				icpm.m_opSelectedTile = NULL;
			}
		}
		
		setNeedsDisplay(m_view);
		isLBdown = true; // 하.. 이건 왜 만든건지 ㅉㅉ...;;
	}
}
void ActiveXDialog::OnLButtonUp(MPoint point) { //마우스 왼쪽 버튼을 눌렀다 띄었을 때 이벤트 발생
	if(m_bAutoGeneration == false){ // 자동 붙이기 타일 이동 및 붙이기 막기 위함
		// 현재 좌표와 이전 좌표 설정
		previousP = CurP;
		CurP = point;
		
		// 생성된 색종이 조각이 있을 시
		if(icpm.m_opSelectedTile) {
			MPoint p1;
			p1 = point;
			CGRect  Mosaic_rect;
			bool b;
			
			//좌표계 설정
			m_stMosaic.GetWindowRect(&Mosaic_rect);
			m_stMosaic.ScreenToClient(&p1); //클라이언트 내부 좌표
			
			//모자이크 영역일 때.
			if(CGRectContainsPoint(Mosaic_rect, CGPointMake(point.x, point.y))){//Mosaic_rect.PtInRect(point)
				//색종이 리필
				b = true;
				for(int i = 0; i< icpm.m_oPaper.m_nTile; i++) {
					if(!icpm.m_oPaper.m_pTiles[i].m_bCutted) {
						b = false;
						break;
					}
					
				}
				if(b == true) {
					icpm.SetColoredPaper();
					icpm.m_oPaper.Render(m_pColordPaper);
					setNeedsDisplay(m_view);
				}
				
				//색종이 붙이기
				icpm.m_oPaper.Reset();
				m_tilePoint = p1;
				icpm.AttachPaper(p1-icpm.dif_Posi);
			} else { // 바깥 영역에 타일 놓을 때
				//생성된 색종이 조각 삭제.
				icpm.m_oPaper.Reset();
				icpm.m_opSelectedTile= NULL;
				setNeedsDisplay(m_view);
 			}
 			icpm.m_oPaper.Render(m_pColordPaper);
		}
		
		TileMove = false;
		isLBdown = false;
		
		setNeedsDisplay(m_view);
	}
}
void ActiveXDialog::OnMouseMove(MPoint point) {
	previousP = CurP;
	CurP = point;

	// 마우스 왼쪽 버튼 down일때(터치 중 일때)
	if(isLBdown){
		// 생성된 타일이 있을 때
		if(icpm.m_opSelectedTile){
			TileMove = true;
			setNeedsDisplay(m_view);
		}
	}
}
