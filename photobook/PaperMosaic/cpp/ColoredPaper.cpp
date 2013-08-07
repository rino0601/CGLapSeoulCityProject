
#import "ColoredPaper.h"
#import "ActiveXDialog.h"


ColoredPaper::ColoredPaper(void) //생성자
{
	m_pIndex = NULL;
	m_pVoronoi= NULL;
	m_pVoronoib= NULL;
	m_pColoredPaperImg = NULL; 
	m_pTiles = NULL;
	m_pTemporary = NULL;
	m_pCPT= NULL;
}

ColoredPaper::~ColoredPaper(void) // 소멸자
{
	if(m_pIndex)
		cvReleaseImage(&m_pIndex);
	
	if(m_pVoronoi)
		cvReleaseImage(&m_pVoronoi);

	if(m_pVoronoib)
		cvReleaseImage(&m_pVoronoib);

	if(m_pColoredPaperImg)
		cvReleaseImage(&m_pColoredPaperImg);

	if(m_pTiles)
		delete [] m_pTiles;
	
	if(m_pCPT)
		delete [] m_pCPT;

/*	if(m_pTemporary)
		delete m_pTemporary;*/

/*	if(m_pDlg)
		delete m_pDlg;	*/

// 	int n = m_vpPaperTiles.size();
// 	for(int i = 0; i < n; i++)
// 	{
// 		delete m_vpPaperTiles[i];
// 	}
// 	m_vpPaperTiles.clear();

	/*if(!CutTile.empty())
		CutTile.clear();*/
	
}

void ColoredPaper::setDlg(ActiveXDialog* dlg)
{
	m_pDlg = dlg;
}

// ªˆ¡æ¿Ã øµø™ ∫–«“
void ColoredPaper::InitPaper(int ntile)
{
	//CRect rect;
 	int side;
 	int w,h;
 
	MPoint mpoint;
 	m_nTile = ntile;  // 색종이 조각 갯수 사용자 지정 (81개)

	//이전에 생성된 타일이 있다면 삭제.
	if(m_pTiles)
	{
		delete [] m_pTiles;

	}	

	m_pTiles = new ColoredPapperTile[ntile];

	if(m_pCPT)
	{		
		m_pCPT = NULL;
	}	

	m_pCPT = new ColoredPapperTile[ntile];	
 	
 	w =  (m_pDlg->m_pColordPaper)->width;	
 	h =  (m_pDlg->m_pColordPaper)->height;	
	
	// 보르노이 이미지
 	m_pVoronoi = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
 	m_pVoronoib = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
	
	// 색종이 영역 이미지
	m_pColoredPaperImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
	cvSet(m_pColoredPaperImg,cvScalar(m_pDlg->m_Color_B, m_pDlg->m_Color_G, m_pDlg->m_Color_R));  // ºˆ¡§ !!
 	
	// 타일 넘버 저장을 위한 이미지
	m_pIndex = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1); 
 	cvSet(m_pIndex,cvScalar(-1.0));
	
	//타일 반지름
 	side = sqrt((float)(ntile));	
	int delta_h =  (h / side) / 2;
	int delta_w =  (w / side) / 2;
	
 	int k = 0;

	// 시드 점 생성
 	for(int j = delta_h; j <= h; j = j + h/ side)
 	{
 		for(int i = delta_w; i <= w; i = i + w/side, k++)
		{
			m_pTiles[k].x = i;
			m_pTiles[k].y = j;
 		}
 	}
 	
	// 시드 점 랜덤 이동 (여러 모양의 타일 생성을 위해)
 	int randsize = sqrt(float(h/side * w/side))/2  *0.6;
 	for(int i = 0; i < ntile; i++){
 		m_pTiles[i].RandomMove(randsize, w, h);
 	}
 	
	// 보르노이 적용
 	getVoronoi();
 	

	// ≈∏¿œ Index º≥¡§
 	for(int i = 0; i < ntile; i++){
		CV_IMAGE_ELEM(m_pIndex, float, m_pTiles[i].y, m_pTiles[i].x) = i;
 	}				
}

// 보로노이
void ColoredPaper::getVoronoi()
{
	// 좌표를 RGB 색상으로 변경하여 각 시드점에 DrawCone
 	Byte r,g,b;
 	m_pDlg->Clear(0,0,0);
 	for(int i = 0; i < m_nTile; i++){
 		r=Byte(m_pTiles[i].x % 256);
 		g=Byte(m_pTiles[i].y % 256);
 		b=Byte((m_pTiles[i].x / 256) * 16 + m_pTiles[i].y / 256);
 		m_pDlg->DrawCone( m_pTiles[i].x, m_pTiles[i].y, r,g,b);
 	}

	// 버퍼 이미지에 보로노이 영상 저장.
	m_pDlg->getBackBuffer(m_pVoronoib);
	
	// 버퍼 이미지에서 복사
	for(int i = 0; i < m_pVoronoib->height; i++){
 		memcpy( &m_pVoronoi->imageData[i*m_pVoronoi->widthStep],
				&m_pVoronoib->imageData[(m_pVoronoib->height-1-i)*m_pVoronoib->widthStep],
				sizeof(Byte)*m_pVoronoib->widthStep);
 	} 
}

// 보로노이 다각형에서 버텍스 추출
void ColoredPaper::ReadVoronoiPixels()
{
	int x,y,i,j,tx,ty,temp;
	int sizeX, sizeY;
	sizeX = m_pVoronoi->width;
	sizeY = m_pVoronoi->height;
	Byte r,g,b,rr,gg,bb;
	int nr,ng,nb;

	int neiborCount;
	bool isVertex;

//	m_vpPaperTiles.resize(m_nTile);

	for(i = 0; i < m_nTile; i++){
		m_pTiles[i].Clear();
	}


	for( y = 0; y < sizeY; y++){
		for( x = 0; x < sizeX; x++){
			r = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4 +2];
			g = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4 +1];
			b = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4];
			// BGRA로 가져오고 있다. 좌표는 검증이 필요.

			if(r==255 && g==255 && b == 255)
				continue;

			// 색상에서 -> 좌표로 변환
			
			tx = (int)(Byte)r + ((int)(Byte)b/16)*256; //max 326
			ty = (int)(Byte)g + ((int)(Byte)b%16)*256; //max 327

			temp = CV_IMAGE_ELEM(m_pIndex, float, ty, tx);

			// paper color 지정 위한 색상 평균 계산
			rr = (Byte)m_pColoredPaperImg->imageData[y*m_pColoredPaperImg->widthStep + x*4 +2];
			gg = (Byte)m_pColoredPaperImg->imageData[y*m_pColoredPaperImg->widthStep + x*4 +1];
			bb = (Byte)m_pColoredPaperImg->imageData[y*m_pColoredPaperImg->widthStep + x*4];

			m_pTiles[temp].AddColor(rr,gg,bb);

			// init
			neiborCount = 0;
			isVertex = false;
			//decide neibor pixel
			for(i = -1; i < 2; i++){
				for(j = -1; j < 2; j++){
					if(i == 0 && j == 0)
						continue;
					// 배열 밖 예외
					if(x+i < 0){
						if(neiborCount == 0){
							neiborCount++;
							nr = -1;
							ng = -1;
							nb = -1;
							continue;
						}
						if(neiborCount == 1){
							if(nr == -1 && ng == -1 && nb == -1)
								continue;
							isVertex = true;
							break;
						}
					}
					if(x+i >= sizeX){
						if(neiborCount == 0){
							neiborCount++;
							nr = -1;
							ng = -1;
							nb = -1;
							continue;
						}
						if(neiborCount == 1){
							if(nr == -1 && ng == -1 && nb == -1)
								continue;
							isVertex = true;
							break;
						}
					}
					if(y+j < 0){
						if(neiborCount == 0){
							neiborCount++;
							nr = -2;
							ng = -2;
							nb = -2;
							continue;
						}
						if(neiborCount == 1){
							if(nr == -2 && ng == -2 && nb == -2)
								continue;
							isVertex = true;
							break;
						}
					}
					if(y+j >= sizeY){
						if(neiborCount == 0){
							neiborCount++;
							nr = -2;
							ng = -2;
							nb = -2;
							continue;
						}
						if(neiborCount == 1){
							if(nr == -2 && ng == -2 && nb == -2)
								continue;
							isVertex = true;
							break;
						}
					}

					// 정상 경우 색 비교.
					rr = (Byte)m_pVoronoi->imageData[(y+j)*m_pVoronoi->widthStep + (x+i)*4 +2];
					gg = (Byte)m_pVoronoi->imageData[(y+j)*m_pVoronoi->widthStep + (x+i)*4 +1];
					bb = (Byte)m_pVoronoi->imageData[(y+j)*m_pVoronoi->widthStep + (x+i)*4];

					if(rr==255 && gg==255 && bb==255)
						continue;

					if(rr != r || gg != g || bb != b){
						if(neiborCount == 0){
							neiborCount++;
							nr = rr;
							ng = gg;
							nb = bb;
						}else{
							if(nr != rr || ng != gg || nb != bb){
								isVertex = true;
								break;
							}
						}
					}
				}
				if(isVertex)
					break;
			}
			// voroni vertex 추가.
			if(isVertex){
				m_pTiles[temp].AddVertex(x,y);
			}
		}
	}

	

	MPoint p;
	double vecX, vecY;
	int angle;
	double Length;

	for(i = 0; i < m_nTile; i++){

		m_pTiles[i].SetAvgColor();  // 색상 저장
		m_pTiles[i].AlignVerties(); // vertex 정렬
	}


	int wid = m_pVoronoi->width;
	int hei = m_pVoronoi->height;	

// 	CRect rect;
// 	m_pDlg->m_stColoredPaper.GetClientRect(rect);
// 	wid = rect.Width();
// 	hei = rect.Height();	
	
	for(i = 0; i < m_nTile; i++){
		
		CombineVert(i); // vertex 합치기
		
		//타일 중심점 저장
		m_pCPT[i].m_bSelected = false;
		m_pCPT[i].m_nAngle = 0;
		m_pCPT[i].m_oPosition.x = m_pTiles[i].x;
		m_pCPT[i].m_oPosition.y = m_pTiles[i].y;

		m_pTiles[i].m_bSelected = false;
		m_pTiles[i].m_nAngle = 0;
		m_pTiles[i].m_oPosition.x = m_pTiles[i].x;
		m_pTiles[i].m_oPosition.y = m_pTiles[i].y;


		// resize
		int n = m_pTiles[i].m_vAlignedVert.size();

		m_pCPT[i].m_vAlignedVert.resize(n);
		m_pCPT[i].m_vnVertAngle.resize(n);
		m_pCPT[i].m_vfVertLength.resize(n);
		m_pCPT[i].m_vfLength.resize(n);
		m_pCPT[i].NoEffect.resize(n);

	
		m_pTiles[i].m_vnVertAngle.resize(n);
		m_pTiles[i].m_vfVertLength.resize(n);
		m_pTiles[i].m_vfLength.resize(n);

		// 초기화
		m_pCPT[i].setFractalInfoAsNULL();
		m_pTiles[i].setFractalInfoAsNULL();

		// 각 타일의 vertex angle, length 재정의
		for(int j = 0 ; j < n ; j ++)
		{
			p.x = m_pTiles[i].m_vAlignedVert[j].x ;
			p.y = m_pTiles[i].m_vAlignedVert[j].y ;
			m_pCPT[i].m_vAlignedVert[j] = p;

			//색종이 경계 부분 톤 이펙트 설정
			if( p.x <= 0 || p.x >= wid - 2  || p.y <= 0 || p.y >= hei - 2 )
			{
				m_pCPT[i].NoEffect[j] = true;
			}
			else
			{
				m_pCPT[i].NoEffect[j] = false;
			}

			vecX = p.x - m_pCPT[i].m_oPosition.x;
			vecY = p.y - m_pCPT[i].m_oPosition.y;

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
			m_pCPT[i].m_vnVertAngle[j] = angle;
			m_pCPT[i].m_vfVertLength[j] = Length;
			m_pCPT[i].m_vfLength[j] = Length;

			m_pTiles[i].m_vnVertAngle[j] = angle;
			m_pTiles[i].m_vfVertLength[j] = Length;
			m_pTiles[i].m_vfLength[j] = Length;
		}

		// 색종이 영역
		m_pCPT[i].updateBaseVert();  // vertex 재정의
		m_pCPT[i].ColoredLayer();    // colorLayer 생성
		m_pCPT[i].WhiteLayer();		 // whiteLayer 생성
		
		//색종이 조각
		m_pTiles[i].updateBaseVert();
		m_pTiles[i].ColoredLayer();
		m_pTiles[i].WhiteLayer();		
	}
}

//vertex 합치기
void ColoredPaper::CombineVert(int TileNum)
{
// 보로노이 다각형이 맞닿는 부분을 한 버텍스로 합침
	int x, y; 
	Byte r;
	Byte g;
	Byte b;

	int n;	
	float sumX, sumY;
	int Cx, Cy;
	MPoint AV_p;

	r = m_pTiles[TileNum].r;
	g = m_pTiles[TileNum].g;
	b = m_pTiles[TileNum].b;

	n = m_pTiles[TileNum].m_vAlignedVert.size();

	for(int i = 0 ; i < n; i ++)
	{
		x = m_pTiles[TileNum].m_vAlignedVert[i].x;
		y = m_pTiles[TileNum].m_vAlignedVert[i].y;
		AV_p.x = TileNum;
		AV_p.y = i;

		//일정 영역 탐색
		for(int k = x-2; k <= x+2; k++){
			for(int l = y-2; l <= y+2; l++){
				SearchNeibVert(k, l, r, g, b ,x , y, TileNum ); // ¡÷∫Ø πˆ≈ÿΩ∫ ≈Ωªˆ
			}
		} 

		// Vertex 위치 변경

		m_pTiles[TileNum].m_vMinVert.push_back(AV_p);
		int indexsiez = m_pTiles[TileNum].m_vMinVert.size();
		
		MPoint p;
		sumX = sumY = 0;	
		// 주변의 vertex 의 합
		for(int j = 0 ; j < indexsiez; j++) 
		{
			 p = m_pTiles[TileNum].m_vMinVert[j];
			
			 sumX = sumX + m_pTiles[p.x].m_vAlignedVert[p.y].x;
			 sumY = sumY + m_pTiles[p.x].m_vAlignedVert[p.y].y;	
		}

		//중심점 계산
		Cx = (int)((sumX / indexsiez) /*+ 0.5*/);
		Cy = (int)((sumY / indexsiez) /*+ 0.5*/);

		//vertex 변경
		for(int j = 0 ; j < indexsiez; j++)
		{
			p = m_pTiles[TileNum].m_vMinVert[j];

		    m_pTiles[p.x].m_vAlignedVert[p.y].x = Cx;
			m_pTiles[p.x].m_vAlignedVert[p.y].y = Cy;	
		}

		 m_pTiles[TileNum].m_vMinVert.clear();

	}
	

}
// 주변 버텍스 탐색
void ColoredPaper::SearchNeibVert(int x, int y, Byte r, Byte g, Byte b, int vX, int vY,int tileNum)
{
	bool save = false;
	int index;
	int wid = m_pVoronoi->width;
	int hei = m_pVoronoi->height;

	//경계 제외
	if( x < 0 || x >= wid || y < 0 || y >= hei)
		return;

	Byte nr, ng, nb;

	nr = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4 +2];
	ng = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4 +1];
	nb = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4   ];

	//같은 보로노이 다각형일 경우 제외
	if( nr == r && ng == g && nb == b)
		return;

	int tx,ty;
	tx = (int)(Byte)nr + ((int)(Byte)nb/16)*256;
	ty = (int)(Byte)ng + ((int)(Byte)nb%16)*256;
	index = CV_IMAGE_ELEM(m_pIndex, float, ty, tx);
	

	//같은 보로노이 다각형일 경우 제외
	if(index == tileNum)
		return;
	
	MPoint p; // x = Tile_index , y = minVert
	MPoint tempP;
	
	// 가장 가까운 버텍스를 찾아 저장
	p.x = index;
	p.y = m_pTiles[index].setChangeVert(x, y , vX, vY); //retun minVert, 

	int vsize = m_pTiles[tileNum].m_vMinVert.size();
	if(vsize > 0)
	{
		for(int i = 0 ; i < vsize; i++)
		{
			 tempP = m_pTiles[tileNum].m_vMinVert[i];

			 if(p.x == tempP.x && p.y == tempP.y )
			 {
				 return;
			 }
			 else
			 {
				 save = true;
			 }
		}
		if(save == true)
		{
			m_pTiles[tileNum].m_vMinVert.push_back(p);
		}

	}
	else
	{
		m_pTiles[tileNum].m_vMinVert.push_back(p);
	}
	
}

//색종이 영역 랜더링
void ColoredPaper::Render(IplImage* dst)
{
 	int n = m_nTile;
 
 	cvSet(dst,cvScalar(255,255,255));
 	//Color setting
 	//CvScalar color = cvScalar(m_pDlg->m_Color_B, m_pDlg->m_Color_G, m_pDlg->m_Color_R);
	CvScalar color = m_pDlg->m_Color;  
 	CvScalar whiteColor;
	
 	whiteColor.val[0] = 255 * 0.6 + color.val[0] * 0.4;
 	whiteColor.val[1] = 255 * 0.6 + color.val[1] * 0.4;
 	whiteColor.val[2] = 255 * 0.6 + color.val[2] * 0.4;
 	CvPoint* pt1 = NULL;
 	int m; 

	CvPoint p1,p2,p3,p4;
	p1.x = 0; p1.y = 0;
	p2.x = dst->width; p2.y= 0;
	p3.x = 0; p3.y= dst->height;
	p4.x = dst->width;  p4.y= dst->height;

	// 흰 층 부터 그릴거야!!
  	for(int i = 0; i < n; i++){
  		if(!m_pTiles[i].m_bCutted){
  		// 그릴 거야!!
  		m = m_pCPT[i].m_vWhiteVert.size();
  	     
  		pt1 = new CvPoint[m];
  
  		for(int j = 0; j < m; j++)
  		{
  			pt1[j].x = int(m_pCPT[i].m_vWhiteVert[j].x + 0.5);
  			pt1[j].y = int(m_pCPT[i].m_vWhiteVert[j].y + 0.5);
  		} 
  		cvFillPoly(dst,&pt1,&m,1,whiteColor);
			
  		delete [] pt1;
  		}
  	}  
   
   	// 이제 색층 차례다!!
   	for(int i = 0; i < n; i++){
   		if(!m_pTiles[i].m_bCutted){
   			// 그릴거야!!
   			m = m_pCPT[i].m_vColorVert.size();
   			pt1 = new CvPoint[m];
   
   			for(int j = 0; j < m; j++){
   				pt1[j].x = int(m_pCPT[i].m_vColorVert[j].x+0.5);
   				pt1[j].y = int(m_pCPT[i].m_vColorVert[j].y+0.5);
   			} 
   			cvFillPoly(dst,&pt1,&m,1,color);   
   			delete [] pt1;
   		}
   	}

	cvLine(dst,p1,p2,cvScalar(255,255,255),2);
	cvLine(dst,p1,p3,cvScalar(255,255,255),1);
	cvLine(dst,p2,p4,cvScalar(255,255,255),2);
	cvLine(dst,p3,p4,cvScalar(255,255,255),2);

	
}

//색종이 영역에서 마우스 클릭시 색종이 조각 생성
ColoredPapperTile* ColoredPaper::GetACut(MPoint position)
{

	int n;
	//int TempPoint;
	int tx, ty;
	Byte r,g,b;
	MPoint p;
	MPoint point;
	CvScalar Color;
	double vecX, vecY;

	point.x = position.x;
	point.y = position.y;

	// 포지션을 색상으로 바꿔서 선택된 타일 찾음
	r = (Byte)m_pVoronoi->imageData[point.y*m_pVoronoi->widthStep + point.x*4 +2];
	g = (Byte)m_pVoronoi->imageData[point.y*m_pVoronoi->widthStep + point.x*4 +1];
	b = (Byte)m_pVoronoi->imageData[point.y*m_pVoronoi->widthStep + point.x*4];

	tx = (int)(Byte)r + ((int)(Byte)b/16)*256;
	ty = (int)(Byte)g + ((int)(Byte)b%16)*256;

	if(tx != 0 && ty != 0) //해당 자리 색종이가 있을 때.
	{
	
		m_nIndex = CV_IMAGE_ELEM(m_pIndex, float, ty, tx);

		//타일이 없을 경우
		if(m_pTiles[m_nIndex].m_bCutted)
			return NULL;
		
		// 임시 tile 객체 생성
		if(m_pTemporary)
			m_pTemporary = NULL;

		if(!m_pTemporary)
			m_pTemporary = &m_pCPT[m_nIndex];  //new MCPMTile();	

		// 색종이 조각 정보 정의

		Color = m_pDlg->m_Color;

		//색상
		m_pTemporary->m_vColor.clear();
		m_pTemporary->m_vColor.push_back(Color);

		m_pTemporary->m_nAngle = 0; //각도

		//중점
		m_pTemporary->m_oPosition.x = m_pTiles[m_nIndex].x;
		m_pTemporary->m_oPosition.y = m_pTiles[m_nIndex].y;

		m_pTiles[m_nIndex].m_bCutted = true;


		// 사이즈 재정의
		n =  m_pTiles[m_nIndex].m_vAlignedVert.size(); ///!!!!!!!
		int angle;
		double Length;

		m_pTemporary->m_vAlignedVert.resize(n);
		m_pTemporary->m_vnVertAngle.resize(n);
		m_pTemporary->m_vfVertLength.resize(n);
		m_pTemporary->m_vfLength.resize(n);
		
		

		for(int i = 0 ; i < n ; i++ )
		{
			p.x = m_pTiles[m_nIndex].m_vAlignedVert[i].x;
			p.y = m_pTiles[m_nIndex].m_vAlignedVert[i].y;

			vecX = p.x - m_pTemporary->m_oPosition.x;
			vecY = p.y - m_pTemporary->m_oPosition.y;
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
			m_pTemporary->m_vnVertAngle[i] = angle;
			m_pTemporary->m_vfVertLength[i] = Length;
			m_pTemporary->m_vfLength[i] = Length;
			

		}	//기본 정보 입력

		m_pTemporary->setFractalInfo();  //색종이 조각에 찢어진 효과(Fractal) setting

		// 색종이에 찢어진 효과를 적용할 에지(색종이 조각과 맞닫는 색종이 부분)search
		SearchCuttdEdge(m_nIndex);  


		// 외곽 버텍스 추출, 색종이 조각과 맞닫는 색종이 부분 찢어진 효과(Fractal) setting
		n = m_pTemporary->m_vAlignedVert.size();
		m_pTemporary->WhiteVertices(n); //white layer 가 보여질 확률을 조정


		for(int i = 0 ; i < m_nTile; i++)
		{		
			if(!m_pTiles[i].m_bCutted)// 색종이 조각 유무 판별
			{
				n = m_pTiles[i].isCuttedVert.size();  
				for(int j = 0 ; j < n ; j ++)
				{
					if(m_pTiles[i].isCuttedVert[j] == true && m_pTiles[i].isCuttedVert[roundRoutine(j+1, n)] == true)
					{
						int vertexindex = m_pTiles[i]. CuttedVertNum[roundRoutine(j+1,n)];

						int numofVert = m_pTemporary->m_vAlignedVert.size();

						vertexindex = roundRoutine(vertexindex-1, numofVert);

						//찢어진 효과 적용, getInvers()를 통해 색종이 조각의 찢어진 효과와 반대되는 효과 적용
						m_pTiles[i].setFractalInfo(j, false, m_pTemporary->m_vpTornOfColored[vertexindex]->GetInvers()); // 윗면(color paper)
						m_pTiles[i].setFractalInfo(j, true, m_pTemporary->m_vpTornOfWhite[vertexindex]->GetInvers());    // 아랫면(white paper)

						m_pTiles[i].m_vWhiteBase[j].x = m_pTemporary->m_vWhiteBase[roundRoutine(vertexindex+1, m_pTemporary->m_vWhiteBase.size())].x;
						m_pTiles[i].m_vWhiteBase[j].y = m_pTemporary->m_vWhiteBase[roundRoutine(vertexindex+1, m_pTemporary->m_vWhiteBase.size())].y;
						m_pTiles[i].m_vbIsWhiteVertSet[j] = true;
						
						m_pTiles[i].m_vWhiteBase[roundRoutine(j+1,n)].x = m_pTemporary->m_vWhiteBase[vertexindex].x;
						m_pTiles[i].m_vWhiteBase[roundRoutine(j+1,n)].y = m_pTemporary->m_vWhiteBase[vertexindex].y;			
						m_pTiles[i].m_vbIsWhiteVertSet[roundRoutine(j+1,n)] = true;

						point.x = i;
						point.y = j;
						CutTile.push_back(point);

						point.x = i;
						point.y = j+1;
						CutTile.push_back(point);

					}
				}
				//선택된 타일
				for(int j = 0; j < n; j++){				
					m_pTiles[i].isCuttedVert[j] = false;
					m_pTiles[i]. CuttedVertNum[j] = -1;
				}
			}			

		}	

		m_pTemporary->updateBaseVert();
		m_pTemporary->ColoredLayer();
		m_pTemporary->WhiteLayer();

		// 주변부 색종이들 이펙트 적용된 vertex 세팅
		for(int i = 0; i < m_nTile; i++) {
			if(!m_pTiles[i].m_bCutted){
				m_pTiles[i].updateBaseVert();
				m_pTiles[i].ColoredLayer();
				m_pTiles[i].WhiteLayer();
			}
		}
	}
	
	// 색종이 조각 랜더링
	n = m_nTile;

	cvSet(m_pDlg->m_pColordPaper,cvScalar(255,255,255));
	//Color setting	
	CvScalar color = m_pDlg->m_Color;
	CvScalar whiteColor;
	whiteColor.val[0] = 255 * 0.6 + color.val[0] * 0.4;
	whiteColor.val[1] = 255 * 0.6 + color.val[1] * 0.4;
	whiteColor.val[2] = 255 * 0.6 + color.val[2] * 0.4;
	CvPoint* pt1 = NULL;
	int m; 

	// 흰 층 부터 그릴거야!!
	for(int i = 0; i < n; i++){
		if(!m_pTiles[i].m_bCutted){
			// 그릴거야!!
			m = m_pTiles[i].m_vWhiteVert.size();

			pt1 = new CvPoint[m];

			for(int j = 0; j < m; j++)
			{
				pt1[j].x = int(m_pTiles[i].m_vWhiteVert[j].x + 0.5);
				pt1[j].y = int(m_pTiles[i].m_vWhiteVert[j].y + 0.5);

			} 
			cvFillPoly(m_pDlg->m_pColordPaper,&pt1,&m,1,whiteColor);

			delete [] pt1;
		}
	}  

	// 이제 색층 차례다!!
	for(int i = 0; i < n; i++){
		if(!m_pTiles[i].m_bCutted){
			// 그릴거야!!
			m = m_pTiles[i].m_vColorVert.size();
			pt1 = new CvPoint[m];

			for(int j = 0; j < m; j++){
				pt1[j].x = int(m_pTiles[i].m_vColorVert[j].x+0.5);
				pt1[j].y = int(m_pTiles[i].m_vColorVert[j].y+0.5);
			} 
			cvFillPoly(m_pDlg->m_pColordPaper,&pt1,&m,1,color);


			delete [] pt1;
		}
	}

	CvPoint p1,p2,p3,p4;

	p1.x = 0; p1.y = 0;
	p2.x = m_pDlg->m_pColordPaper->width; p2.y= 0;
	p3.x = 0; p3.y= m_pDlg->m_pColordPaper->height;
	p4.x = m_pDlg->m_pColordPaper->width;  p4.y= m_pDlg->m_pColordPaper->height;

	cvLine(m_pDlg->m_pColordPaper,p1,p2,cvScalar(255,255,255),2);
	cvLine(m_pDlg->m_pColordPaper,p1,p3,cvScalar(255,255,255),1);
	cvLine(m_pDlg->m_pColordPaper,p2,p4,cvScalar(255,255,255),2);
	cvLine(m_pDlg->m_pColordPaper,p3,p4,cvScalar(255,255,255),2);


	return m_pTemporary;	

}

//색종이 영역의 찢겨진 부분 reset
void ColoredPaper:: Reset()
{
	int ColorSize, WhiteSize;

	for(int i = 0 ; i < m_nTile; i++)
	{
		if(!m_pTiles[i].m_bCutted)
		{
			ColorSize =  m_pTiles[i].m_vColorVert.size();

			WhiteSize =  m_pTiles[i].m_vWhiteVert.size();
			m_pCPT[i].m_vColorVert.clear();
			m_pCPT[i].m_vWhiteVert.clear();
			m_pCPT[i].m_vColorVert.resize(ColorSize);
			m_pCPT[i].m_vWhiteVert.resize(WhiteSize);

			for(int m = 0; m < ColorSize; m++)
			{
				m_pCPT[i].m_vColorVert[m] = m_pTiles[i].m_vColorVert[m];
			}
			for(int n = 0; n < WhiteSize; n++)
			{
				m_pCPT[i].m_vWhiteVert[n] = m_pTiles[i].m_vWhiteVert[n];
			}

		}
	}
}

// 색종이에 찢어진 효과를 적용할 에지(색종이 조각과 맞닫는 색종이 부분) search
void ColoredPaper::SearchCuttdEdge(int TileNum)
{
	int x, y; 
	Byte r,g,b;
	int n;	

	x = m_pTiles[TileNum].x;
	y = m_pTiles[TileNum].y;

	r = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4 +2];
	g = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4 +1];
	b = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4   ];

	n = m_pTiles[TileNum].m_vAlignedVert.size();

	for(int i = 0 ; i < n; i ++)

	{
		x = m_pTiles[TileNum].m_vAlignedVert[i].x;
		y = m_pTiles[TileNum].m_vAlignedVert[i].y;
		
		for(int k = x-4; k <= x+4; k++){
			for(int l = y-4; l <= y+4; l++){
				searchNeibVert(k, l, r, g, b, i, TileNum);  // 주변 버텍스 탐색
			}
		}
	}	
}
//주변 버텍스 탐색
void ColoredPaper::searchNeibVert(int x, int y, Byte r, Byte g, Byte b, int num, int TileNum)
{
	//bool save = false;
	int index;
	int wid = m_pVoronoi->width;
	int hei = m_pVoronoi->height;
	
	if( x < 0 || x >= wid || y < 0 || y >= hei)
		return;	

	Byte nr, ng, nb;

	nr = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4 +2];
	ng = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4 +1];
	nb = (Byte)m_pVoronoi->imageData[y*m_pVoronoi->widthStep + x*4   ];

	if( nr == r && ng == g && nb == b)
		return;

	int tx,ty;
	tx = (int)(Byte)nr + ((int)(Byte)nb/16)*256;
	ty = (int)(Byte)ng + ((int)(Byte)nb%16)*256;
	index = CV_IMAGE_ELEM(m_pIndex, float, ty, tx);

	if(m_pTiles[index].m_bCutted)
		return;

	m_pTiles[index].setCuttedVert(x, y , num , TileNum);	//찢어진 효과를 표현할 버텍스 저장
}