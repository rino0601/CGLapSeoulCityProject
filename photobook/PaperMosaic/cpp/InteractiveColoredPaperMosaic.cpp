#import "InteractiveColoredPaperMosaic.h"
#import "MPoint.h"
#import "ActiveXDialog.h"

InteractiveColoredPaperMosaic::InteractiveColoredPaperMosaic(void)
{
	m_opSelectedTile = NULL;
    m_oCPM = new ColoredPaperMosaic();
}

InteractiveColoredPaperMosaic::~InteractiveColoredPaperMosaic(void)
{
	if(m_opSelectedTile)
		delete m_opSelectedTile;

	/*if(m_pDlg)
		delete m_pDlg;*/

	delete m_oCPM;
}

void InteractiveColoredPaperMosaic::setDlg(ActiveXDialog* dlg)
{
	m_pDlg = dlg;
}

//색종이 영역 전처리 과정
void InteractiveColoredPaperMosaic::SetColoredPaper()
{
	m_oPaper.InitPaper();										 // 색종이 영역 분활 및 색상 지정
	m_oPaper.ReadVoronoiPixels();								 // 보로노이 vertex 추출
}

// 마우스 선택 영역 색종이 조각 생성
void InteractiveColoredPaperMosaic::CutAPaper(MPoint Position)   
{
	MPoint mp;
	mp.x = Position.x;
	mp.y = Position.y;
	
	m_opSelectedTile = NULL;
	m_opSelectedTile =  m_oPaper.GetACut(mp);
}

bool InteractiveColoredPaperMosaic::HasSelectedTile()
{
	return true;
}

//선택된 타일 그리기(타일 이동시(마우스 드래그))
void InteractiveColoredPaperMosaic::DrawSelectedTileAt(CGContextRef ctx, MPoint position)
{
	m_opSelectedTile->RenderToDC(ctx, position);
}

//자동붙이기시, 색종이 영역에서 모자이크 영역으로 타일 이동 표현 
void InteractiveColoredPaperMosaic::DrawAutoTileAt(CGContextRef ctx, MPoint position)
{
	m_oCPM->m_optile->RenderToDC(ctx, position);
}

// 색종이 붙이기.
void InteractiveColoredPaperMosaic::AttachPaper(MPoint position)
{
	if(m_opSelectedTile)  // 타일 붙이기
	{
		m_opSelectedTile->m_oPosition.x = position.x;
		m_opSelectedTile->m_oPosition.y = position.y;
		
		m_opSelectedTile->updateBaseVert();
		
		// 레이어 생성
		m_opSelectedTile->ColoredLayer();
		m_opSelectedTile->WhiteLayer();
		
		//타일 추가
		m_oCPM->AddTile(m_opSelectedTile);
		
		m_opSelectedTile = NULL;		
	}
	else //타일 이동
	{
		double index = CV_IMAGE_ELEM(m_oCPM->m_pTileImageMask, float, position.y, position.x);
		if(index == -1)
		{
			return;
		}
		else
		{
			m_opSelectedTile = m_oCPM->m_vpTileList[index];
			
			
			
			dif_Posi.x  = position.x - m_opSelectedTile->m_oPosition.x;
			dif_Posi.y  = position.y - m_opSelectedTile->m_oPosition.y;
			
			
			m_opSelectedTile->updateBaseVert();
			
			m_opSelectedTile->ColoredLayer();
			m_opSelectedTile->WhiteLayer();
			
			
			//마스크맵 리셋 및 선택 된 타일 삭제
			
			m_oCPM->setMosaic(index, m_opSelectedTile->m_oPosition);
		}
	}
	
	
}

void InteractiveColoredPaperMosaic::MoveTile() { // 타일 흔들기 효과
	int x,y;
	int tilesize;
	int w,h;

	int m,n;

	int r;

	tilesize = m_oCPM->m_vpTileList.size();	
	
	srand((unsigned)time(NULL));	

	for(int i = 0; i < tilesize; i++)
	{
		
		r = rand()%8;

		if(r == 0)
		{
			x = 30;
			x = -x;
			y = -x;
		}
		else if( r == 1)
		{


			x = 30;
			y = -x;
			x = 0;
		}
		else if( r == 2)
		{
			x = 30;
			x = +x;
			y = -x;
		}
		else if( r == 3)
		{
			x = 30;
			x = +x;
			y = 0;
		}
		else if( r == 4)
		{
			x = 30;
			x = +x;
			y = +x;
		}
		else if( r == 5)
		{
			x = 30;
			y = +x;
			x = 0;
		}
		else if( r == 6)
		{
			x = 30;
			x = -x;
			y = +x;
		}
		else if( r == 7)
		{
			x = 30;
			x = -x;
			y = 0;
		}
		

		m_opSelectedTile = m_oCPM->m_vpTileList[i];
		

		w = m_pDlg->m_pMosaic->width;	
		h = m_pDlg->m_pMosaic->height;

		m = m_opSelectedTile->m_oPosition.x + x;
		n = m_opSelectedTile->m_oPosition.y + y;
		
		if(m > 10 && m < w-10 && n > 10 && n < h-10 )
		{
			m_opSelectedTile->m_oPosition.x += x;
			m_opSelectedTile->m_oPosition.y += y;
		}
		

		m_opSelectedTile->updateBaseVert();	
		m_opSelectedTile->ColoredLayer();
		m_opSelectedTile->WhiteLayer(); 

		m_oCPM->m_vpTileList[i] = m_opSelectedTile;
	}

	m_oCPM->AddTile();
	m_opSelectedTile = NULL;

}
/*
void InteractiveColoredPaperMosaic::ResizeTileAt(MPoint position, float lengthRatio) // 타일 크기 조절
{

	double index = CV_IMAGE_ELEM(m_oCPM.m_pTileImageMask, float, position.y, position.x);
	if(index < 0) {return ;}
	m_opSelectedTile = m_oCPM.m_vpTileList[index];
	m_opSelectedTile->m_oPosition.x = position.x;		
	m_opSelectedTile->m_oPosition.y = position.y;
	

	m_opSelectedTile->Resize(lengthRatio);							// 타일 크기 조절

	m_oCPM.m_vpTileList.erase(m_oCPM.m_vpTileList.begin() + index); // 이동 되기전 타일 제거

	m_oCPM.AddTile();												// 타일 추가
	AttachPaper(position);											// 붙이기

}

void InteractiveColoredPaperMosaic::RotateTileAt(MPoint position, int deltaAngle)  //타일 회전 조절
{
	MPoint p;
	double index = CV_IMAGE_ELEM(m_oCPM.m_pTileImageMask, float, position.y, position.x);
	if(index<0) { return ; }
	if(m_opSelectedTile) {
		m_opSelectedTile = NULL;
	}
	m_opSelectedTile = m_oCPM.m_vpTileList[index];

	p.x = m_opSelectedTile->m_oPosition.x;	
	p.y = m_opSelectedTile->m_oPosition.y;

	m_opSelectedTile->m_nAngle = ANGLE(m_opSelectedTile->m_nAngle + deltaAngle);		// 색종이 조각의 vertex 각도 정의

	m_oCPM.m_vpTileList.erase(m_oCPM.m_vpTileList.begin() + index);						// 회전 되기전 타일 제거

	m_oCPM.AddTile();																	// 타일 추가
	AttachPaper(p);																		// 붙이기
}
*/
void InteractiveColoredPaperMosaic::AutoGeneration(ActiveXDialog *flagOwner)  // 자동붙이기
{
	// 색종이 자동 붙이기
	if(flagOwner->m_bAutoGeneration==true) {
 		m_oCPM->setOdering(); // 전 처리 작업, 후보 위치 지정, EdgeDistanceMap 생성
		flagOwner->c_view->killAlert();
	}
	while(m_oCPM->Odering()) { // 타일이 붙기 좋은 위치 지정 및 타일 생성
 		if(flagOwner->m_bAutoGeneration==false)
 			break;
	}	
}
