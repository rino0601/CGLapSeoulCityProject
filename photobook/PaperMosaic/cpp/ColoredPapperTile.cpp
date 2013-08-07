#import "ColoredPapperTile.h"
#import "ActiveXDialog.h"

#ifndef CPoint
#define CPoint MPoint
#endif


#define NEW_TILE			0
#define TILE				255
#define OVERLAP				128
#define NON_LAYER			0
#define PLUS				1
#define MINUS				-1

#define EDGE				3
#define DELETE_TILE			1
#define DELETING_TILE		10



ColoredPapperTile::ColoredPapperTile(int saveCount) //생성자
: m_nAngle(0)
, m_bSelected(false)
, m_bCutted(false)
{
	isNeiborCutted = false;
	m_type = saveCount;
	
}

ColoredPapperTile::~ColoredPapperTile(void)//소멸자
{
	Destroy();
}

void ColoredPapperTile::Destroy() //메모리 해제
{
	if(!NoEffect.empty())
		NoEffect.clear();
	
	if(!isCuttedVert.empty())
		isCuttedVert.clear();
	
	if(!CuttedVertNum.empty())
		CuttedVertNum.clear();
	
	if(!CuttedTileNum.empty())
		CuttedTileNum.clear();
	
	if(!vertex.empty())
		vertex.clear();
	
	if(!m_vAlignedVert.empty())
		m_vAlignedVert.clear();
	
	if(!m_vColor.empty())
		m_vColor.clear();
	
	if(!m_vnVertAngle.empty())
		m_vnVertAngle.clear();
	
	if(!m_vfVertLength.empty())
		m_vfVertLength.clear();
	
	if(!m_vfLength.empty())
		m_vfLength.clear();
	
	if(!m_vColorVert.empty())
		m_vColorVert.clear();
	
	if(!m_vWhiteVert.empty())
		m_vWhiteVert.clear();
	
	if(!m_vWhiteBase.empty())
		m_vWhiteBase.clear();
	
	if(!m_vbIsWhiteVertSet.empty())
		m_vbIsWhiteVertSet.clear();
	
	if(!m_vMinVert.empty())
		m_vMinVert.clear();
	
	int n = m_vpTornOfColored.size();
	for(int i = 0; i < n ; i++){
		m_vpTornOfColored[i]->Destroy();
	}
	n = m_vpTornOfWhite.size();
	for(int i = 0; i < n ; i++){
		m_vpTornOfWhite[i]->Destroy();
	}
}

// 시드점 이동 (다양한 모양의 보로노이 다각형 생성을 위해)
void ColoredPapperTile::RandomMove(int range, int maxx, int maxy)
{
	x += random( range * 2 -1 ) - range +1;
	if( x < 0)
		x = 0;
	if( x >= maxx)
		x = maxx-1;
	
	y += random( range * 2 -1 ) - range +1;
	if( y < 0)
		y = 0;
	if( y >= maxy)
		y = maxy-1;
}

void ColoredPapperTile::AddColor(Byte R, Byte G, Byte B)
{
	ar += R;
	ag += G;
	ab += B;
	ccount++;
}

void ColoredPapperTile::SetAvgColor()
{
	r = ar / ccount;
	g = ag / ccount;
	b = ab / ccount;
}

//버텍스 저장
void ColoredPapperTile::AddVertex(int x, int y)
{
	int vertex_size =vertex.size();
	for(int i = 0; i < vertex_size; i++){
		
		// 다른 버텍스와 거리가 가까우면 제외
		if((abs(vertex[i].x - x) + abs(vertex[i].y - y)) <= 2)
			return;
	}
	//버텍스 저장
	vertex.push_back(MPoint(x,y));
	
}

//버텍스 정렬
void ColoredPapperTile::AlignVerties()
{
	/////////// 정렬 방법 개선 -> 각도 평가 ///////////////
	int tempX, tempY;
	int* angle;
	int n = vertex.size();
	angle = new int[ n ];
	
	double vecX,vecY;
	double baseX,baseY;
	double temp;
	
	// calcul center
	baseX = baseY = 0;
	for(int i =0; i < n; i++){
		baseX += vertex[i].x;
		baseY += vertex[i].y;
	}
	
	tempX = baseX / n;
	tempY = baseY / n;
	
	// calcul angle
	for(int i = 0; i < n; i++){
		vecX = vertex[i].x - tempX;
		vecY = vertex[i].y - tempY;
		//normalize
		temp = vecX*vecX + vecY*vecY;
		temp = sqrt( temp );
		vecX = vecX/temp;
		vecY = vecY/temp;
		
		//base 벡터와 비교하여 angle 계산
		angle[i] = acos(vecX) * 180 / 3.14;
		if( vecY < 0){
			angle[i] = 360 - angle[i];
		}
		
	}
	// angle 순서에 따른 저장
	int minangle;
	int minIndex;
	for(int i = 0; i < n; i++){
		minangle = 360;
		for(int j = 0; j < n; j++){
			if(angle[j] < minangle){
				minIndex = j;
				minangle = angle[j];
			}
		}
		
		MPoint p;
		p.x = vertex[ minIndex ].x;
		p.y = vertex[ minIndex ].y;
		m_vAlignedVert.push_back(p);
		
		isCuttedVert.push_back( false );
		
		CuttedVertNum.push_back(-1);
		CuttedTileNum.push_back(-1);
		angle[ minIndex ] = 360;
	}
	isNeiborCutted = false;
	m_bCutted = false;
	delete [] angle;
}

void ColoredPapperTile::Clear()
{
	ar = 0;
	ag = 0;
	ab = 0;
	ccount = 0;
	vertex.clear();
}

// 프렉탈 정보 Set
void ColoredPapperTile::setFractalInfo()
{
	int n = m_vAlignedVert.size();
	if((int)m_vpTornOfColored.size() != n){
		for(int i = 0; i < (int)m_vpTornOfColored.size(); i++)
			m_vpTornOfColored[i]->Destroy();
		
		for(int i = 0; i < (int)m_vpTornOfWhite.size(); i++)
			m_vpTornOfWhite[i]->Destroy();
		
		
		m_vpTornOfColored.resize( n );
		m_vpTornOfWhite.resize( n );
		
		for(int i = 0; i < n; i++){
			m_vpTornOfColored[i] = new TornEffect();
			m_vpTornOfWhite[i] = new TornEffect();
		}
	}
	
	
	for(int i = 0; i < n; i++){
		//프렉탈효과가 없을 경우 다시 정의
		if(!m_vpTornOfColored[i]->hasEffect())
		{
			m_vpTornOfColored[i]->InitEffect(); // 프렉탈의 노드들 생성
		}
		
		if(!m_vpTornOfWhite[i]->hasEffect())
		{
			m_vpTornOfWhite[i]->InitEffect();
		}
	}
}

//프렉탈 정보 set (Invers Fractal 효과 위해)
void ColoredPapperTile::setFractalInfo(int i, bool isWhite, TornEffect* effect)
{
	if(isWhite){
		if((int)m_vpTornOfWhite.size() <= i){
			return;
		}
		m_vpTornOfWhite[i]->Destroy();
		delete m_vpTornOfWhite[i];
		m_vpTornOfWhite[i] = effect;
	}else{
		if((int)m_vpTornOfColored.size() <= i){
			return;
		}
		m_vpTornOfColored[i]->Destroy();
		delete m_vpTornOfColored[i];
		m_vpTornOfColored[i] = effect;
	}
}

//프렉탈 정보 초기화
void ColoredPapperTile::setFractalInfoAsNULL()
{
	m_vpTornOfColored.resize( m_vAlignedVert.size() );
	m_vpTornOfWhite.resize( m_vAlignedVert.size() );
	
	for(int i = 0; i < (int)m_vAlignedVert.size(); i++){
        if(m_vpTornOfColored[i] != NULL){
            delete m_vpTornOfColored[i];
        }
        if(m_vpTornOfWhite[i] != NULL){
            delete m_vpTornOfWhite[i];
        }
            
		m_vpTornOfColored[i] = new TornEffect();
		m_vpTornOfWhite[i] = new TornEffect();
	}
}

//버텍스 정보 제정의 (색종이 조각의 회전, 이동, 크기 변경시 사용)
void ColoredPapperTile::updateBaseVert()
{
	int n = m_vfVertLength.size();
	
	for(int i = 0; i < n; i++){
		m_vAlignedVert[i].x = m_oPosition.x + m_vfVertLength[i] * ( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) );
		m_vAlignedVert[i].y = m_oPosition.y + m_vfVertLength[i] * ( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) );
	}
	if(m_vbIsWhiteVertSet.size() == 0){
		m_vbIsWhiteVertSet.resize(n);
		m_vWhiteBase.resize(n);
		for(int i = 0; i < n; i++){
			m_vbIsWhiteVertSet[i] = false;
			m_vWhiteBase[i] = MPoint(0,0);
		}
	}
}

//컬러 레이어 생성
void ColoredPapperTile::ColoredLayer(IplImage* dis, int minDis, float ratio) //(IplImage* dis=NULL, int minDis=2, float ratio=1.0f)
{
	MPoint a, b;
	int n = m_vAlignedVert.size();
	
	vector<MPoint> rstList;
	
	// calculate vertex list
	m_vColorVert.clear();
	double distance;
	
	double edgeRatio;
	for(int i = 0; i < n; i++){
		
		//색종이 경계일 경우 톤이펙트 적용 안함
		int size =  NoEffect.size();
		if(size != 0)
		{
			if(NoEffect[i] == true && NoEffect[roundRoutine(i+1, n)] == true)
			{
				m_vColorVert.push_back(m_vAlignedVert[i]);
				continue;
			}
		}
		
		
		// start = a  end = b
		a = m_vAlignedVert[i];
		if(i == n-1){
			b = m_vAlignedVert[0];
		}else{
			b = m_vAlignedVert[i+1];
		}
		if(dis){
			if(a.x < 0 || int(a.x+0.5) >= dis->width || a.y < 0 || int(a.y+0.5) >= dis->height ||
			   b.x < 0 || int(b.x+0.5) >= dis->width || b.y < 0 || int(b.y+0.5) >= dis->height)
				edgeRatio = 1.0;
			else{
				distance = ((Byte)dis->imageData[int(a.y+0.5)* dis->widthStep + int(a.x+0.5)]
							+ (Byte)dis->imageData[int(b.y+0.5)* dis->widthStep + int(b.x+0.5)])/2.0;
				edgeRatio = min( 1.0, max( 0.1, (distance-5)/10.0 ) );
			}
		}else{
			edgeRatio = 1.0;
		}
		// get fractal value and store
		rstList = m_vpTornOfColored[i]->GetTornEffectPoint( a, b, minDis, ratio * edgeRatio );
		
		for(int j = 0; j < (int)rstList.size(); j++)
			m_vColorVert.push_back( rstList[j] );
	}
	rstList.clear();
}

//white layer 가 보여질 확률을 조정
void ColoredPapperTile::WhiteVertices(int n, int whiteRange, bool b)  //(int n, int whiteRange=7, bool b = true)
{
	if( m_vWhiteBase.size() == 0) {
		m_vWhiteBase.resize( n );
		m_vbIsWhiteVertSet.resize(n);
		
		for( int i = 0; i < n; i++) {
			// white layer 가 보여질 확률을 조정한것  65:35
			
			m_vWhiteBase[i].x = ( random( int(whiteRange*1.65+0.5) ) - int(whiteRange*0.35+0.5));
			m_vWhiteBase[i].y = ( random( int(whiteRange*1.65+0.5) ) - int(whiteRange*0.35+0.5));
			m_vbIsWhiteVertSet[i] = true;
		}
	} else {
		for(int i = 0; i < n; i++) {
			if(!m_vbIsWhiteVertSet[i]) {
				if(b){
					m_vWhiteBase[i].x = ( random( int(whiteRange*1.65+0.5) ) - int(whiteRange*0.35+0.5));
					m_vWhiteBase[i].y = ( random( int(whiteRange*1.65+0.5) ) - int(whiteRange*0.35+0.5));
					m_vbIsWhiteVertSet[i] = true;
				}
			}
		}
	}
}

//white layer 생성
void ColoredPapperTile::WhiteLayer(IplImage* dis, int whiteRange, int minDis, float ratio, float ratio2)
{
	int n = m_vAlignedVert.size();
	WhiteVertices(n, whiteRange, false);
	MPoint a, b;
	vector<MPoint> rstList;
	double distance;
	double edgeRatio;
	
	// calculate vertex list
	m_vWhiteVert.clear();
	for(int i = 0; i < n; i++)
	{
		int size =  NoEffect.size();
		if(size != 0)
		{
			//색종이 경계일 경우 톤이펙트 적용 안함
			if(NoEffect[i] == true && NoEffect[roundRoutine(i+1, n)] == true)
			{
				m_vWhiteVert.push_back(m_vAlignedVert[i]);
				
				continue;
			}
		}
		
		// start = a  end = b									// 타일크기에 비례하여 white layer 보여지는 범위 조절한것
		a.x = m_vAlignedVert[i].x + (m_vWhiteBase[i].x * min(1.0,((m_vfVertLength[i]+15)/75.0)))*ratio2;
		a.y = m_vAlignedVert[i].y + (m_vWhiteBase[i].y * min(1.0,((m_vfVertLength[i]+15)/75.0)))*ratio2;
		if(i == n-1)
		{
			b.x = m_vAlignedVert[0].x + (m_vWhiteBase[0].x * min(1.0,((m_vfVertLength[0]+15)/75.0)))*ratio2;
			b.y = m_vAlignedVert[0].y + (m_vWhiteBase[0].y * min(1.0,((m_vfVertLength[0]+15)/75.0)))*ratio2;
		}
		else
		{
			b.x = m_vAlignedVert[i+1].x + (m_vWhiteBase[i+1].x * min(1.0,((m_vfVertLength[i+1]+15)/75.0)))*ratio2;
			b.y = m_vAlignedVert[i+1].y + (m_vWhiteBase[i+1].y * min(1.0,((m_vfVertLength[i+1]+15)/75.0)))*ratio2;
		}
		if(dis)
		{
			if(a.x < 0 || int(a.x+0.5) >= dis->width || a.y < 0 || int(a.y+0.5) >= dis->height ||
			   b.x < 0 || int(b.x+0.5) >= dis->width || b.y < 0 || int(b.y+0.5) >= dis->height)
			{
				edgeRatio = 1.0;
			}
			else
			{
				distance = ((Byte)dis->imageData[int(a.y+0.5)* dis->widthStep + int(a.x+0.5)] + (Byte)dis->imageData[int(b.y+0.5)* dis->widthStep + int(b.x+0.5)])/2.0;
				edgeRatio = min(1.0, max(0.1, distance-5)/10.0);
			}
		}
		else
		{
			edgeRatio = 1.0;
		}
		// get fractal value and store
		rstList = m_vpTornOfWhite[i]->GetTornEffectPoint( a, b, minDis, ratio*edgeRatio);
		
		for(int j = 0; j < (int)rstList.size(); j++)
			m_vWhiteVert.push_back( rstList[j] );
	}
	rstList.clear();
}

//타일이 맞닫는 부분의 버텍스를 하나로 합치기 위해 가까운 버텍스를 찾음
int ColoredPapperTile::setChangeVert(int px, int py , int vX, int vY)
{
	MPoint p;
	
	int n = m_vAlignedVert.size();
	
	float minDis = 10000000;
	float dis;
	int minIndex = -1;
	for(int i = 0; i < n ; i++){
		dis =  sqrt((px - m_vAlignedVert[i].x)*(px - m_vAlignedVert[i].x) + (py - m_vAlignedVert[i].y)*(py - m_vAlignedVert[i].y));
		if( dis < minDis){
			minDis = dis;
			minIndex = i;
		}
	}
	
	return minIndex;
}

//찢어짐 효과를 표현하기 위해 생성되는 타일의 버텍스와 가까운 버텍스를 찾고 주변 색종이 조각(타일)의 넘버와 버텍스 저장
void ColoredPapperTile::setCuttedVert(int px, int py,  int vertexnum , int TileNum)
{
	MPoint p;
	isNeiborCutted = true;
	
	int n = m_vAlignedVert.size();
	
	float minDis = 10000;
	float dis;
	int minIndex = -1;
	for(int i = 0; i < n ; i++){
		dis =  sqrt((px - m_vAlignedVert[i].x)*(px - m_vAlignedVert[i].x) + (py - m_vAlignedVert[i].y)*(py - m_vAlignedVert[i].y));
		if( dis < minDis){
			minDis = dis;
			minIndex = i;
		}
	}
	isCuttedVert[minIndex] = true;
	CuttedVertNum[minIndex] = vertexnum;
	CuttedTileNum[minIndex] = TileNum;
	
}

//선택된 타일 그리기(타일 이동시(마우스 드래그))
void ColoredPapperTile::RenderToDC(CGContextRef ctx,MPoint position)
{
	//	MPoint* pt1 = NULL;
	//	MPoint* pt2 = NULL;
	CGPoint* pt1 = NULL;
	CGPoint* pt2 = NULL;
	
	int n,m,l;
	
	// Color setting
	CvScalar color;
	color.val[0] = color.val[1] = color.val[2] = 0;
	l = m_vColor.size();
	for(int i = 0; i < l; i++){
		color.val[0] += m_vColor[i].val[0];
		color.val[1] += m_vColor[i].val[1];
		color.val[2] += m_vColor[i].val[2];
	}
	Byte cr, cg, cb;
	Byte dcr, dcg, dcb;
	Byte wr, wg, wb;
	Byte dwr, dwg, dwb;
	cb = color.val[0] = color.val[0]/(double)l;
	cg = color.val[1] = color.val[1]/(double)l;
	cr = color.val[2] = color.val[2]/(double)l;
	
	
	CvScalar whiteColor;	// 화이트 밝아지는 정도 조절 6:4
	wb = whiteColor.val[0] = 255 * 0.6 + color.val[0] * 0.4;
	wg = whiteColor.val[1] = 255 * 0.6 + color.val[1] * 0.4;
	wr = whiteColor.val[2] = 255 * 0.6 + color.val[2] * 0.4;
	CvScalar darkenWhite;	// border
	dwb = darkenWhite.val[0] = whiteColor.val[0] * 0.92;
	dwg = darkenWhite.val[1] = whiteColor.val[1] * 0.92;
	dwr = darkenWhite.val[2] = whiteColor.val[2] * 0.92;
	
	CvScalar darkenColor;	// border
	dcb = darkenColor.val[0] = color.val[0] * 0.92;
	dcg = darkenColor.val[1] = color.val[1] * 0.92;
	dcr = darkenColor.val[2] = color.val[2] * 0.92;
	
	
	
	// set vertex
	n = m_vWhiteVert.size();
	pt1 = new CGPoint[n];
	m = m_vColorVert.size();
	pt2 = new CGPoint[m];
	
	MPoint posi_dif;
	posi_dif.x = (double)position.x - m_oPosition.x;
	posi_dif.y = (double)position.y - m_oPosition.y;
	
	// set Points Lists
	int x,y;
	for(int i = 0; i < n; i++){
		x =  int(m_vWhiteVert[i].x+posi_dif.x+0.5);
		y =  int(m_vWhiteVert[i].y+posi_dif.y+0.5);
		
		pt1[i].x = x;
		pt1[i].y = y;
	}
	for(int i = 0; i < m; i++){
		x =  int(m_vColorVert[i].x+posi_dif.x+0.5);
		y =  int(m_vColorVert[i].y+posi_dif.y+0.5);
		
		pt2[i].x = x;
		pt2[i].y = y;
	}
	
	//iOS CGContext Code.
	CGContextSetLineWidth(ctx, 1); // 1이상 값.
	CGContextSetLineCap(ctx, kCGLineCapRound);
	
	// 흰 층 //iOS가 RGB Space만 지원해서 BGR로 긁은 값은 뒤집어서 넣어야 한다.
	CGContextSetRGBFillColor(ctx, wb/255.0f, wg/255.0f, wr/255.0f, 1);
	CGContextBeginPath(ctx);
	CGContextAddLines(ctx, pt1, n);
	CGContextClosePath(ctx);
	CGContextFillPath(ctx);
	CGContextSetRGBStrokeColor(ctx, dwb/255.0f, dwg/255.0f, dwr/255.0f, 1);
	CGContextBeginPath(ctx);
	CGContextAddLines(ctx, pt1, n);
	CGContextClosePath(ctx);
	CGContextStrokePath(ctx);
	
	
	// 색 층 //iOS가 RGB Space만 지원해서 BGR로 긁은 값은 뒤집어서 넣어야 한다.
	CGContextSetRGBFillColor(ctx, cb/255.0f, cg/255.0f, cr/255.0f, 1);
	CGContextBeginPath(ctx);
	CGContextAddLines(ctx, pt2, m);
	CGContextClosePath(ctx);
	CGContextFillPath(ctx);
	CGContextSetRGBStrokeColor(ctx, dcb/255.0f, dcg/255.0f, dcr/255.0f, 1);
	CGContextBeginPath(ctx);
	CGContextAddLines(ctx, pt2, m);
	CGContextClosePath(ctx);
	CGContextStrokePath(ctx);
	
	delete [] pt1;
	delete [] pt2;
	
}

// 색종이 영역 랜더링(색종이 리필 또는 색종이 영역 생성 시 사용) 및 모자이크 영역 색종이 조각 랜더링
void ColoredPapperTile::Render(IplImage* dst,bool isShadow, IplImage* temp)
{
	CvPoint* pt1;
	CvPoint* pt2;
	CvPoint* pt3;
	CvPoint* pt4;
	int n,m,l;
	n = m_vWhiteVert.size();
	pt1 = new CvPoint[n];
	pt2 = new CvPoint[n];
	m = m_vColorVert.size();
	pt3 = new CvPoint[m];
	pt4 = new CvPoint[m];
	
	// Color setting
	CvScalar color;
	color.val[0] = color.val[1] = color.val[2] = 0;
	l = m_vColor.size();
	for(int i = 0; i < l; i++){
		color.val[0] += m_vColor[i].val[0];
		color.val[1] += m_vColor[i].val[1];
		color.val[2] += m_vColor[i].val[2];
	}
	color.val[0] = color.val[0]/(double)l;
	color.val[1] = color.val[1]/(double)l;
	color.val[2] = color.val[2]/(double)l;
	
	CvScalar whiteColor;	// 화이트 밝아지는 정도 조절 6:4
	whiteColor.val[0] = 255 * 0.6 + color.val[0] * 0.4;
	whiteColor.val[1] = 255 * 0.6 + color.val[1] * 0.4;
	whiteColor.val[2] = 255 * 0.6 + color.val[2] * 0.4;
	CvScalar darkenWhite;	// border
	darkenWhite.val[0] = whiteColor.val[0] * 0.92;
	darkenWhite.val[1] = whiteColor.val[1] * 0.92;
	darkenWhite.val[2] = whiteColor.val[2] * 0.92;
	
	CvScalar darkenColor;	// border
	darkenColor.val[0] = color.val[0] * 0.92;
	darkenColor.val[1] = color.val[1] * 0.92;
	darkenColor.val[2] = color.val[2] * 0.92;
	
	// set Points Lists
	int x,y;
	int minx,miny,maxx,maxy;
	miny = minx = 500000;
	maxy = maxx = 0;
	for(int i = 0; i < n; i++){
		
		x =  int(m_vWhiteVert[i].x+0.5);
		y =  int(m_vWhiteVert[i].y+0.5);
		
		if(x < minx)
			minx = max(0,x);
		if(x > maxx)
			maxx = min( x+1, dst->width);
		if(y < miny)
			miny = max(0,y);
		if(y > maxy)
			maxy = min(y+1, dst->height);
		pt1[i] = cvPoint(x,y);
		pt2[i] = cvPoint(x+1,y+1);
	}
	for(int i = 0; i < m; i++){
		
		x =  int(m_vColorVert[i].x+0.5);
		y =  int(m_vColorVert[i].y+0.5);
		if(x < minx)
			minx = max(0,x);
		if(x > maxx)
			maxx = min(x+1, dst->width);
		if(y < miny)
			miny = max(0,y);
		if(y > maxy)
			maxy = min(y+1, dst->height);
		pt3[i] = cvPoint(x, y);
		pt4[i] = cvPoint(x+1, y+1);
	}
	
	
	if(isShadow){
		if(temp){
			cvSetZero(temp);
			cvFillPoly(temp,&pt2,&n,1,cvScalar(1,1,1));
			cvFillPoly(temp,&pt4,&m,1,cvScalar(1,1,1));
			
			for(y=miny; y< maxy; y++){
				for(x = minx; x< maxx; x++){
					if( temp->imageData[y * temp->widthStep + x] == 1){
						dst->imageData[y * dst->widthStep + x * dst->nChannels]		= Byte((Byte)dst->imageData[y * dst->widthStep + x * dst->nChannels] * 0.8);
						dst->imageData[y * dst->widthStep + x * dst->nChannels + 1]	= Byte((Byte)dst->imageData[y * dst->widthStep + x * dst->nChannels+1] * 0.8);
						dst->imageData[y * dst->widthStep + x * dst->nChannels + 2]	= Byte((Byte)dst->imageData[y * dst->widthStep + x * dst->nChannels+2] * 0.8);
					}
				}
			}
		}else{
			IplImage* temp = cvCreateImage(cvGetSize(dst),8,1);
			cvSetZero(temp);
			cvFillPoly(temp,&pt2,&n,1,cvScalar(1,1,1));
			cvFillPoly(temp,&pt4,&m,1,cvScalar(1,1,1));
			
			for(y=miny; y< maxy; y++){
				for(x = minx; x< maxx; x++){
					if( temp->imageData[y * temp->widthStep + x] == 1){
						dst->imageData[y * dst->widthStep + x * dst->nChannels]		= Byte((Byte)dst->imageData[y * dst->widthStep + x * dst->nChannels] * 0.8);
						dst->imageData[y * dst->widthStep + x * dst->nChannels + 1]	= Byte((Byte)dst->imageData[y * dst->widthStep + x * dst->nChannels+1] * 0.8);
						dst->imageData[y * dst->widthStep + x * dst->nChannels + 2]	= Byte((Byte)dst->imageData[y * dst->widthStep + x * dst->nChannels+2] * 0.8);
					}
				}
			}
			cvReleaseImage(&temp);
		}
	}
	//render White paper layer
	cvFillPoly(dst,&pt1,&n,1,whiteColor);
	cvPolyLine(dst,&pt1,&n,1,1,darkenWhite);
	
	//render Colored paper layer
	cvFillPoly(dst,&pt3,&m,1,color);
	cvPolyLine(dst,&pt3,&m,1,1,darkenColor);
	
	delete [] pt1;
	delete [] pt2;
	delete [] pt3;
	delete [] pt4;
	
}

//모자이크 영역 타일 이동시 타일 선택 영역 타일 랜더링
void ColoredPapperTile::SelectRender(IplImage* dst, CPoint Max, CPoint Min, bool isShadow, IplImage* temp, IplImage* temp2)
{
	CvPoint* pt1;
	CvPoint* pt2;
	CvPoint* pt3;
	CvPoint* pt4;
	int n,m,l;
	n = m_vWhiteVert.size()  ;
	pt1 = new CvPoint[n];
	pt2 = new CvPoint[n];
	m = m_vColorVert.size()  ;
	pt3 = new CvPoint[m];
	pt4 = new CvPoint[m];
	
	
	if(!temp2)
	{
		temp2 = cvCreateImage(cvGetSize(dst),IPL_DEPTH_8U,4);
		cvResize(dst, temp2,CV_INTER_LINEAR);
	}
	
	// Color setting
	CvScalar color;
	color.val[0] = color.val[1] = color.val[2] = 0;
	l = m_vColor.size();
	for(int i = 0; i < l; i++){
		color.val[0] += m_vColor[i].val[0];
		color.val[1] += m_vColor[i].val[1];
		color.val[2] += m_vColor[i].val[2];
	}
	color.val[0] = color.val[0]/(double)l;
	color.val[1] = color.val[1]/(double)l;
	color.val[2] = color.val[2]/(double)l;
	
	CvScalar whiteColor;	// 화이트 밝아지는 정도 조절 6:4
	whiteColor.val[0] = 255 * 0.6 + color.val[0] * 0.4;
	whiteColor.val[1] = 255 * 0.6 + color.val[1] * 0.4;
	whiteColor.val[2] = 255 * 0.6 + color.val[2] * 0.4;
	CvScalar darkenWhite;	// border
	darkenWhite.val[0] = whiteColor.val[0] * 0.92;
	darkenWhite.val[1] = whiteColor.val[1] * 0.92;
	darkenWhite.val[2] = whiteColor.val[2] * 0.92;
	
	CvScalar darkenColor;	// border
	darkenColor.val[0] = color.val[0] * 0.92;
	darkenColor.val[1] = color.val[1] * 0.92;
	darkenColor.val[2] = color.val[2] * 0.92;
	
	// set Points Lists
	
	// 박스영역으로 설정해서 포인트 저장 !!! 수정하면 될것이다.. 2013/4/10
	int x,y;
	int minx,miny,maxx,maxy;
	miny = minx = 50000;
	maxy = maxx = 0;
	
	for(int i = 0; i < n; i++){
		
		x =  int(m_vWhiteVert[i].x+0.5);
		y =  int(m_vWhiteVert[i].y+0.5);
		
		if(x < minx)
			minx = max(0,x);
		if(x > maxx)
			maxx = min( x+1, dst->width);
		if(y < miny)
			miny = max(0,y);
		if(y > maxy)
			maxy = min(y+1, dst->height);
		pt1[i] = cvPoint(x,y);
		pt2[i] = cvPoint(x+1,y+1);
	}
	for(int i = 0; i < m; i++){
		
		x =  int(m_vColorVert[i].x+0.5);
		y =  int(m_vColorVert[i].y+0.5);
		if(x < minx)
			minx = max(0,x);
		if(x > maxx)
			maxx = min(x+1, dst->width);
		if(y < miny)
			miny = max(0,y);
		if(y > maxy)
			maxy = min(y+1, dst->height);
		pt3[i] = cvPoint(x, y);
		pt4[i] = cvPoint(x+1, y+1);
	}
	
	//render White paper layer
	cvFillPoly(temp2,&pt1,&n,1,whiteColor);
	cvPolyLine(temp2,&pt1,&n,1,1,darkenWhite);
	
	//render Colored paper layer
	cvFillPoly(temp2,&pt3,&m,1,color);
	cvPolyLine(temp2,&pt3,&m,1,1,darkenColor);
	
	
	Min.y = Min.y - 50;
	if(Min.y < 0 )
		Min.y = 0;
	
	Min.x = Min.x - 50;
	if( Min.x < 0)
		Min.x = 0;
	
	Max.y =Max.y + 50;
	if(Max.y > dst->height)
		Max.y = dst->height;
	
	Max.x = Max.x + 50;
	if(Max.x > dst->width)
		Max.x = dst->width;
	
	
	for(int j = Min.y;  j < Max.y; j++)
	{
		for(int i = Min.x; i < Max.x; i++)
		{
			int index_mosaic = ((j)*temp2->widthStep)+i*4;
			
			dst->imageData[index_mosaic]   = temp2->imageData[index_mosaic];
			dst->imageData[index_mosaic+1] = temp2->imageData[index_mosaic+1];
			dst->imageData[index_mosaic+2] = temp2->imageData[index_mosaic+2];
			
		}
	}
	
	//	cvResize(temp2, dst, CV_INTER_LINEAR);
	
	
	delete [] pt1;
	delete [] pt2;
	delete [] pt3;
	delete [] pt4;
	
}

// 모자이크 영역 타일 맵 생성(타일 번호로 타일 색상 지정), 타일의 유무 판단을 위해 사용
void  ColoredPapperTile::setTileMap(float x, float y, int angle, IplImage* tileMap, float val)
{
	if(m_type < NEW_TILE)
		return;
	CvPoint* vertList;
	int n = m_vAlignedVert.size();
	
	vertList = new CvPoint[n];
	for(int i = 0; i < n; i++){
		vertList[i].x = int( m_vfVertLength[i]*( COS( ANGLE( angle + m_vnVertAngle[i]) ) ) + x + 0.5 );
		vertList[i].y = int( m_vfVertLength[i]*( SIN( ANGLE( angle + m_vnVertAngle[i]) ) ) + y + 0.5 );
	}
	cvFillPoly(tileMap,&vertList,&n,1,CV_RGB(val,val,val));
	delete [] vertList;
}

// 타일 사이즈 조정
void  ColoredPapperTile::Resize(float lengthRatio)
{
	float Length;
	bool resize;
	
	
	
	int n = m_vfVertLength.size();
	for(int i = 0; i < n ; i++)
	{
		//타일의 크기 최대 크기 조절
		Length = m_vfVertLength[i] ;
		if(Length > 60)
		{
			resize = false;
			break;
		}
		else
		{
			resize = true;
		}
		
	}
	
	//
	for(int i = 0; i < n ; i++)
	{
		Length = m_vfVertLength[i] *lengthRatio;
		if(resize == true)
		{
			// VertLength 가 70 보다 작을 경우 크기 키움
			if(Length < 70 &&  lengthRatio > 1 )
			{
			 	m_vfVertLength[i] =  Length;
			 	m_vfLength[i] = Length;
			}
			
		}
		
		// VertLength 가 6 보다 클 경우 크기 키움
		if(Length >6  &&  lengthRatio < 1)
		{
			m_vfVertLength[i] =  Length;
			m_vfLength[i] = Length;
		}
	}
	
	
}

// 자동 붙이기시 전처리 작업으로 후보점 찾기 위해 사용할 맵 생성
void ColoredPapperTile::setMaps(IplImage* tileMap, IplImage* overlapMap, IplImage* positionMap, float overlapRatio, float regenRatio, IplImage* temp)
{
	if(m_type < NEW_TILE)
		return;
	CvPoint* overList;
	CvPoint* baseList;
	CvPoint* positionList;
	CvPoint* midPositionList;
	CvPoint* innerPositionList;
	
	int n = m_vAlignedVert.size();
	
	double tempLength;
	overList = new CvPoint[n];
	baseList = new CvPoint[n];
	positionList = new CvPoint[n];
	midPositionList = new CvPoint[n];
	innerPositionList = new CvPoint[n];
	
	for(int i = 0; i < n; i++){
		tempLength = m_vfVertLength[i]*overlapRatio;
		overList[i].x = ( tempLength*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		overList[i].y = ( tempLength*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		
		tempLength = m_vfVertLength[i]*regenRatio;
		baseList[i].x = ( tempLength*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		baseList[i].y = ( tempLength*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		
		tempLength = (m_vfVertLength[i] + m_vfVertLength[i])*0.95;
		positionList[i].x = ( tempLength*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		positionList[i].y = ( tempLength*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		midPositionList[i].x = ( (tempLength - 3)*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		midPositionList[i].y = ( (tempLength - 3)*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		innerPositionList[i].x = ( (tempLength - 7)*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		innerPositionList[i].y = ( (tempLength - 7)*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
	}
	if(!temp){
		temp = cvCreateImage(cvGetSize(tileMap),32,1);
		cvSetZero(temp);
		
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-3),7);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-2),5);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-1),3);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(0),1);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(1),5);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(2),3);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(3),1);
		
		cvAdd(positionMap,temp,positionMap);
		cvReleaseImage(&temp);
	}else{
		cvSetZero(temp);
		
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-3),7);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-2),5);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-1),3);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(0),1);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(1),5);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(2),3);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(3),1);
		
		cvAdd(positionMap,temp,positionMap);
	}
	cvFillPoly(positionMap,&innerPositionList,&n,1,cvScalar(-4));
	cvFillPoly(tileMap,&baseList,&n,1,CV_RGB(TILE,TILE,TILE));
	cvFillPoly(overlapMap,&overList,&n,1,CV_RGB(OVERLAP,OVERLAP,OVERLAP));
	
	delete [] baseList;
	delete [] positionList;
	delete [] overList;
	delete [] innerPositionList;
	delete [] midPositionList;
}

void ColoredPapperTile::reSetMaps(IplImage* tileMap, IplImage* overlapMap, IplImage* positionMap, float overlapRatio, float regenRatio, IplImage* temp)
{
	if(m_type < NEW_TILE)
		return;
	CvPoint* overList;
	CvPoint* baseList;
	CvPoint* positionList;
	CvPoint* midPositionList;
	CvPoint* innerPositionList;
	
	int n = m_vAlignedVert.size();
	
	double tempLength;
	overList = new CvPoint[n];
	baseList = new CvPoint[n];
	positionList = new CvPoint[n];
	midPositionList = new CvPoint[n];
	innerPositionList = new CvPoint[n];
	
	for(int i = 0; i < n; i++){
		tempLength = m_vfVertLength[i]*overlapRatio;
		overList[i].x = ( tempLength*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		overList[i].y = ( tempLength*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		
		tempLength = m_vfVertLength[i]*regenRatio;
		baseList[i].x = ( tempLength*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		baseList[i].y = ( tempLength*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		
		tempLength = (m_vfVertLength[i] + m_vfVertLength[i])*0.95;
		positionList[i].x = ( tempLength*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		positionList[i].y = ( tempLength*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		midPositionList[i].x = ( (tempLength - 3)*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		midPositionList[i].y = ( (tempLength - 3)*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		innerPositionList[i].x = ( (tempLength - 7)*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		innerPositionList[i].y = ( (tempLength - 7)*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
	}
	if(!temp){
		temp = cvCreateImage(cvGetSize(tileMap),32,1);
		cvSetZero(temp);
		
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-3),7);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-2),5);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-1),3);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(0),1);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(1),5);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(2),3);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(3),1);
		
		cvSub(positionMap,temp,positionMap);
		
		cvReleaseImage(&temp);
	}else{
		cvSetZero(temp);
		
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-3),7);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-2),5);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(-1),3);
		cvPolyLine(temp,&midPositionList,&n,1,1,cvScalar(0),1);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(1),5);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(2),3);
		cvPolyLine(temp,&positionList,&n,1,1, cvScalar(3),1);
		
		cvSub(positionMap,temp,positionMap);
		cvReleaseImage(&temp);
	}
	cvFillPoly(positionMap,&innerPositionList,&n,1,cvScalar(0));
	cvFillPoly(tileMap,&baseList,&n,1,CV_RGB(0,0,0));
	cvFillPoly(overlapMap,&overList,&n,1,CV_RGB(0,0,0));
	
	delete [] baseList;
	delete [] positionList;
	delete [] overList;
	delete [] innerPositionList;
	delete [] midPositionList;
	
}

void ColoredPapperTile::setMaskMaps(IplImage* tileMap, IplImage* overlapMap, IplImage* positionMap, float overlapRatio, float regenRatio, IplImage* temp)
{
	if(m_type < NEW_TILE)
		return;
	CvPoint* overList;
	CvPoint* baseList;
	CvPoint* innerPositionList;
	
	int n = m_vAlignedVert.size();
	
	double tempLength;
	overList = new CvPoint[n];
	baseList = new CvPoint[n];
	innerPositionList = new CvPoint[n];
	
	for(int i = 0; i < n; i++){
		tempLength = m_vfVertLength[i]*overlapRatio;
		overList[i].x = ( tempLength*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		overList[i].y = ( tempLength*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		
		tempLength = m_vfVertLength[i]*regenRatio;
		baseList[i].x = ( tempLength*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		baseList[i].y = ( tempLength*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
		
		tempLength = (m_vfVertLength[i] + m_vfVertLength[i])*0.95;
		innerPositionList[i].x = ( (tempLength - 7)*( COS( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.x +0.5 );
		innerPositionList[i].y = ( (tempLength - 7)*( SIN( ANGLE( m_nAngle + m_vnVertAngle[i]) ) ) + m_oPosition.y +0.5 );
	}
	
	cvFillPoly(positionMap,&innerPositionList,&n,1,cvScalar(-4));
	cvFillPoly(tileMap,&baseList,&n,1,CV_RGB(TILE,TILE,TILE));
	cvFillPoly(overlapMap,&overList,&n,1,CV_RGB(OVERLAP,OVERLAP,OVERLAP));
	
	delete [] baseList;
	delete [] overList;
	delete [] innerPositionList;
	
}

//자동 붙이기시 선택된 위치에 대한 정보 저장
void ColoredPapperTile::setPosition(float x, float y, int angle, int maxx, int maxy, CvScalar color)
{
	m_oPosition.x = x;
	m_oPosition.y = y;
	m_mImageSize.x = maxx;
	m_mImageSize.y = maxy;
	m_nAngle = angle;
	m_vColor.push_back(color);
}

// 자동 붙이기시 타일 생성 (선택된 위치에서 에지와 주변 타일들을 고려하여 버텍스 생성)
void ColoredPapperTile::generation(IplImage* overmap, IplImage* edge, IplImage* layer, int radius, float minRatio, int addtionalEdge)
{
	m_vfLength.clear();
	m_vfVertLength.clear();
	m_vnVertAngle.clear();
	
	m_nRadius = radius;
	m_fMinRatio = minRatio;
	m_nAdditionalEdge = addtionalEdge;
	
	int step = 15;
	vector< float > lengths;
	float x, y;
	
	// intersection test
	for(int i = 0; i < 360; i= i+step){
		int j;
		for(j = 1; j < radius+1; j++){
			x = (j* ( COS( ANGLE( m_nAngle + i ) ) ) + m_oPosition.x);
			y = (j* ( SIN( ANGLE( m_nAngle + i ) ) ) + m_oPosition.y);
			if( isIntersect(x, y, overmap, edge, layer) ){
				j++;
				break;
			}
		}
		lengths.push_back(j-1);
	}
	
	// 변곡점 찾기
	int n = 360/step;
	float leftflag, rightflag;
	for(int i = 0; i < n; i++){
		int j = i+1;
		if(j == n)
			j = 0;
		int h = i-1;
		if(h == -1)
			h = n-1;
		leftflag = lengths[i] - lengths[h];
		rightflag = lengths[j] - lengths[i];
		
		if(leftflag > 0)
			leftflag = PLUS;
		if(leftflag < 0)
			leftflag = MINUS;
		if(rightflag > 0)
			rightflag = PLUS;
		if(rightflag < 0)
			rightflag = MINUS;
		
		// 변곡점
		if(leftflag != rightflag){
			// vertex 추가
			m_vfVertLength.push_back( lengths[i] );
			m_vfLength.push_back( lengths[i] );
			m_vnVertAngle.push_back( i*step );
		}
	}
	
	// 변곡점 검사 및 추가 생성
	int shape = 6;		// 5~6
	int baseAngle = 360/shape;
	// 예외 상황 추가
	if(m_vfVertLength.size() == 0){
		m_vfVertLength.push_back(lengths[0]);
		m_vfLength.push_back( lengths[0]);
		m_vnVertAngle.push_back(0);
	}
	if(m_vfVertLength.size() == 1){
		for(int i = 1; i < shape; i++){
			// need some random
			m_vnVertAngle.push_back( ANGLE( m_vnVertAngle[0] + i*baseAngle) );
			m_vfVertLength.push_back(lengths[ (int)((i* baseAngle) /(double)step +0.5) ]);
			m_vfLength.push_back(lengths[ (int)((i* baseAngle) /(double)step +0.5) ]);
		}
	}
	
	for(int i = 0; i < (int)m_vfVertLength.size(); i++)
	{
		m_vfVertLength[i] = max( m_vfVertLength[i], m_nRadius * m_fMinRatio );
	    m_vfLength[i] = max( m_vfLength[i], m_nRadius * m_fMinRatio );
	}
	
	// 각도 검사 및 추가
	int tempAngle;
	int insertedAngle;
	// 길이 검사용 변수
	int l, m;
	float fl, fm, fn;
	for(int i = 0; i < (int)m_vnVertAngle.size(); i++){
		int j = i+1;
		if(j == m_vnVertAngle.size())
			j = 0;
		rightflag = m_vnVertAngle[j] - m_vnVertAngle[i];
		if(rightflag < 0)
			rightflag +=360;
		
		tempAngle = m_vnVertAngle[i];
		if( rightflag < 100)
			continue;
		if (rightflag >= 100 && rightflag < 160){
			baseAngle = rightflag/2;
			insertedAngle = ANGLE( tempAngle+baseAngle );
			m_vnVertAngle.insert(m_vnVertAngle.begin()+j, insertedAngle);
			// 길이 검색 검색
			l = insertedAngle/step;
			m = insertedAngle%step;
			n = l+1;
			if( n == 360/step )
				n = 0;
			fl = lengths[l];
			fn = lengths[n];
			
			fm = ((step-m)/(double)step) * fl + (m/(double)step) * fn;
			m_vfVertLength.insert(m_vfVertLength.begin()+j, fm);
			m_vfLength.insert(m_vfLength.begin()+j, fm);
		}else if( rightflag >= 160 && rightflag < 230){
			baseAngle = rightflag/3;
			
			for(int k = 2; k > 0; k--){
				insertedAngle = ANGLE( tempAngle+(baseAngle*k) );
				m_vnVertAngle.insert(m_vnVertAngle.begin()+j, insertedAngle );
				
				l = insertedAngle/step;
				m = insertedAngle%step;
				n = l+1;
				if( n == 360/step )
					n = 0;
				fl = lengths[l];
				fn = lengths[n];
				
				fm = ((step-m)/(double)step) * fl + (m/(double)step) * fn;
				m_vfVertLength.insert(m_vfVertLength.begin()+j, fm);
				m_vfLength.insert(m_vfLength.begin()+j, fm);
			}
		}else if( rightflag >= 230 && rightflag < 294){
			baseAngle = rightflag/4;
			
			for(int k = 3; k > 0; k--){
				insertedAngle = ANGLE( tempAngle+(baseAngle*k) );
				m_vnVertAngle.insert(m_vnVertAngle.begin()+j, insertedAngle );
				
				l = insertedAngle/step;
				m = insertedAngle%step;
				n = l+1;
				if( n == 360/step )
					n = 0;
				fl = lengths[l];
				fn = lengths[n];
				
				fm = ((step-m)/(double)step) * fl + (m/(double)step) * fn;
				m_vfVertLength.insert(m_vfVertLength.begin()+j, fm);
				m_vfLength.insert(m_vfLength.begin()+j, fm);
			}
		}else if( rightflag >= 294 && rightflag < 320){
			baseAngle = rightflag/5;
			
			for(int k = 4; k > 0; k--){
				insertedAngle = ANGLE( tempAngle+(baseAngle*k) );
				m_vnVertAngle.insert(m_vnVertAngle.begin()+j, insertedAngle );
				
				l = insertedAngle/step;
				m = insertedAngle%step;
				n = l+1;
				if( n == 360/step )
					n = 0;
				fl = lengths[l];
				fn = lengths[n];
				
				fm = ((step-m)/(double)step) * fl + (m/(double)step) * fn;
				m_vfVertLength.insert(m_vfVertLength.begin()+j, fm);
				m_vfLength.insert(m_vfLength.begin()+j, fm);
			}
		}else if( rightflag >= 320 && rightflag < 360){
			baseAngle = rightflag/6;
			
			for(int k = 5; k > 0; k--){
				insertedAngle = ANGLE( tempAngle+(baseAngle*k) );
				m_vnVertAngle.insert(m_vnVertAngle.begin()+j, insertedAngle );
				
				l = insertedAngle/step;
				m = insertedAngle%step;
				n = l+1;
				if( n == 360/step )
					n = 0;
				fl = lengths[l];
				fn = lengths[n];
				
				fm = ((step-m)/(double)step) * fl + (m/(double)step) * fn;
				m_vfVertLength.insert(m_vfVertLength.begin()+j, fm);
				m_vfLength.insert(m_vfLength.begin()+j, fm);
			}
		}
	}
	m_vAlignedVert.resize( m_vfVertLength.size());
	updateBaseVert();
	
	lengths.clear();
}

//intersection test
bool ColoredPapperTile::isIntersect(float fx, float fy, IplImage* overmap, IplImage* edge, IplImage* layer)
{
	int x,y;
	x = int(fx + 0.5);
	y = int(fy + 0.5);
	
	// 이미지 경계와 교점
	if(x < 1 || x >= m_mImageSize.x -1 || y < 1 || y >= m_mImageSize.y -1)
		return false;
	
	// 에지와 교점
	if((Byte)edge->imageData[ y * edge->widthStep + x ] <= m_nAdditionalEdge
	   || (Byte)edge->imageData[ (y-1)* edge->widthStep + x ] <= m_nAdditionalEdge
	   || (Byte)edge->imageData[ (y+1)* edge->widthStep + x ] <= m_nAdditionalEdge
	   || (Byte)edge->imageData[ y * edge->widthStep + (x+1) ] <= m_nAdditionalEdge
	   || (Byte)edge->imageData[ y * edge->widthStep + (x-1) ] <= m_nAdditionalEdge
	   || (Byte)edge->imageData[ (y-1) * edge->widthStep + (x-1) ] <= m_nAdditionalEdge
	   || (Byte)edge->imageData[ (y-1) * edge->widthStep + (x+1) ] <= m_nAdditionalEdge
	   || (Byte)edge->imageData[ (y+1) * edge->widthStep + (x-1) ] <= m_nAdditionalEdge
	   || (Byte)edge->imageData[ (y+1) * edge->widthStep + (x+1) ] <= m_nAdditionalEdge )
		return true;
	if(layer){
		// 레이어 경계와 교점
		if((Byte)layer->imageData[ y * layer->widthStep + x ] == NON_LAYER)
			return true;
	}
	if(overmap){	// 다른 타일과의 허용량에 교점
		if((Byte)overmap->imageData[ y * overmap->widthStep + x ] == OVERLAP)
			return true;
	}
	return false;
}

//모자이크 영역, 색종이 붙이기시 누적효과
void ColoredPapperTile::setAccumulatedTile(IplImage* dst, IplImage* temp)
{
	// 지우고 있는 타일 누적맵 안그림
	if(m_type < NEW_TILE)
		return;
	if(!temp){
		temp = cvCreateImage(cvGetSize(dst),8,1);	//for the speed up we'd better create and release this image before calling this function
		cvSetZero(temp);
		CvPoint* pt;
		int n = m_vWhiteVert.size();
		pt = new CvPoint[n];
		int x,y;
		for(int i = 0; i < n; i++){
			x =  int(m_vWhiteVert[i].x+0.5);
			y =  int(m_vWhiteVert[i].y+0.5);
			pt[i] = cvPoint(x,y);
		}
		cvFillPoly(temp,&pt,&n,1,CV_RGB(1,1,1));
		
		delete [] pt;
		
		//render Colored paper layer
		n = m_vColorVert.size();
		pt = new CvPoint[n];
		for(int i = 0; i < n; i++){
			x =  int(m_vColorVert[i].x+0.5);
			y =  int(m_vColorVert[i].y+0.5);
			pt[i] = cvPoint(x, y);
		}
		cvFillPoly(temp,&pt,&n,1,CV_RGB(1,1,1));
		cvAdd(dst,temp,dst);
		delete [] pt;
		cvReleaseImage(&temp);
	}else{
		cvSetZero(temp);
		CvPoint* pt;
		int n = m_vWhiteVert.size();
		pt = new CvPoint[n];
		int x,y;
		for(int i = 0; i < n; i++){
			x =  int(m_vWhiteVert[i].x+0.5);
			y =  int(m_vWhiteVert[i].y+0.5);
			pt[i] = cvPoint(x,y);
		}
		cvFillPoly(temp,&pt,&n,1,CV_RGB(1,1,1));
		delete [] pt;
		
		//render Colored paper layer
		n = m_vColorVert.size();
		pt = new CvPoint[n];
		for(int i = 0; i < n; i++){
			x =  int(m_vColorVert[i].x+0.5);
			y =  int(m_vColorVert[i].y+0.5);
			pt[i] = cvPoint(x, y);
		}
		cvFillPoly(temp,&pt,&n,1,CV_RGB(1,1,1));
		cvAdd(dst,temp,dst);
		delete [] pt;
	}
}

void ColoredPapperTile:: setAccumulatedMap(IplImage* dst, IplImage* temp)
{
	if(!temp){
		temp = cvCreateImage(cvGetSize(dst),8,1);	//for the speed up we'd better create and release this image before calling this function
		cvSetZero(temp);
		CvPoint* pt;
		int n = m_vWhiteVert.size();
		pt = new CvPoint[n];
		int x,y;
		for(int i = 0; i < n; i++){
			x =  int(m_vWhiteVert[i].x+0.5);
			y =  int(m_vWhiteVert[i].y+0.5);
			pt[i] = cvPoint(x,y);
		}
		cvFillPoly(temp,&pt,&n,1,CV_RGB(1,1,1));
		
		delete [] pt;
		
		
		//render Colored paper layer
		n = m_vColorVert.size();
		pt = new CvPoint[n];
		for(int i = 0; i < n; i++){
			x =  int(m_vColorVert[i].x+0.5);
			y =  int(m_vColorVert[i].y+0.5);
			pt[i] = cvPoint(x, y);
		}
		cvFillPoly(temp,&pt,&n,1,CV_RGB(1,1,1));
		cvAdd(dst,temp,dst);
		delete [] pt;
		cvReleaseImage(&temp);
	}
}

//에지 클리핑, 에지위에 타일이 놓일때 알맞는 모양으로 타일의 변경
void ColoredPapperTile::edgeClipping(IplImage* edge, int deltaLength, IplImage* tile, bool isInit)
{
	if(m_type < NEW_TILE)
		return;
	
	m_mImageSize.x = edge->width;
	m_mImageSize.y = edge->height;
	
	m_nMinLength = 6;
	
	double length,curlength,templength;
	double dx,dy;
	double x,y;
	int n = m_vAlignedVert.size();;
	
	// - & | direction
	MPoint a,b, mid;
	
	int j;
	int angleData[7] = {0,0,0,60,45,36,30};
	
	double flength;
	int ilength;
	
	for(int i = 0; i < n; i++){
		a = m_vAlignedVert[i];
		b = i == n-1 ? m_vAlignedVert[0] : m_vAlignedVert[i+1];
		mid.x = (a.x + b.x)/2.0;
		mid.y = (a.y + b.y)/2.0;
		
		// calculate actual length
		length = (mid.x - m_oPosition.x)*(mid.x - m_oPosition.x) + (mid.y - m_oPosition.y)*(mid.y - m_oPosition.y);
		curlength = length = sqrt(length);
		if(length == 0){
			if(m_type == NEW_TILE){
				m_type = DELETE_TILE;
			}
			continue;
		}
		
		// calculate direction vector
		dx = length == 0? 0 : (mid.x - m_oPosition.x) / length;
		dy = length == 0? 0 : (mid.y - m_oPosition.y) / length;
		
		// templength is tile width/2
		templength = m_vfLength[i]*( COS( angleData[n] ) );
		
		// for tile grow up
		if( length < templength && m_type == NEW_TILE){
			length = min( templength, length + deltaLength*0.5);
		}
		
		
		ilength = (int)length;
		flength = length - ilength;
		
		for(j = 0; j < ilength; j++){
			x = m_oPosition.x + dx * j;
			y = m_oPosition.y + dy * j;
			
			x = max(1, min( m_mImageSize.x-2, int(x+0.5)));
			y = max(1, min( m_mImageSize.y-2, int(y+0.5)));
			
			// if meet edge
			if( isEdge(x,y,edge) ){
				j = getEdgeClippedLength(j, curlength, isInit, deltaLength)+1;// if all passed then j is over length so +1 for default decrease
				break;
			}
			if(tile != NULL)
			{
				// if meet other tile
				if( (Byte)tile->imageData[ int(y) * edge->widthStep + int(x) ] == TILE){// && isInit){
					j = getTileClippedLength(j, curlength, isInit, deltaLength)+1;// if all passed then j is over length so +1 for default decrease
					break;
				}
			}
		}
		
		// ¡ﬂΩ…¡°
		x = m_oPosition.x + dx * (j-1 + flength);
		y = m_oPosition.y + dy * (j-1 + flength);
		
		
		////// maybe it need to fix about the cross movement of two tiles ////////////////////////
		m_vAlignedVert[i].x = x + a.x - mid.x;
		m_vAlignedVert[i].y = y + a.y - mid.y;
		
		m_vfVertLength[i] = sqrt((double)((m_vAlignedVert[i].x - m_oPosition.x) * (m_vAlignedVert[i].x - m_oPosition.x)
										  + (m_vAlignedVert[i].y - m_oPosition.y) * (m_vAlignedVert[i].y - m_oPosition.y)));
		
		if(i == n-1){
			m_vAlignedVert[0].x = x + b.x - mid.x;
			m_vAlignedVert[0].y = y + b.y - mid.y;
			m_vfVertLength[0] = sqrt((double)((m_vAlignedVert[0].x - m_oPosition.x) * (m_vAlignedVert[0].x - m_oPosition.x)
											  + (m_vAlignedVert[0].y - m_oPosition.y) * (m_vAlignedVert[0].y - m_oPosition.y)));
			
		}else{
			m_vAlignedVert[i+1].x = x + b.x - mid.x;
			m_vAlignedVert[i+1].y = y + b.y - mid.y;
			m_vfVertLength[i+1] = sqrt((double)((m_vAlignedVert[i+1].x - m_oPosition.x) * (m_vAlignedVert[i+1].x - m_oPosition.x)
												+ (m_vAlignedVert[i+1].y - m_oPosition.y) * (m_vAlignedVert[i+1].y - m_oPosition.y)));
		}
		////////////////////////////////////////////////////////////////////////////////////////////
	}
	
	// diagonal (/ & \) direction
	for(int i = 0; i < n; i++)
	{
		if(m_vfVertLength[i] == 0){
			if(m_type == NEW_TILE){
				m_type = DELETE_TILE;
			}
			continue;
		}
		
		dx = m_vfVertLength[i] == 0? 0 : (m_vAlignedVert[i].x - m_oPosition.x) / m_vfVertLength[i];
		dy = m_vfVertLength[i] == 0? 0 : (m_vAlignedVert[i].y - m_oPosition.y) / m_vfVertLength[i];
		
		if( m_vfVertLength[i] < m_vfLength[i] && m_type == NEW_TILE){
			length = min( m_vfLength[i], m_vfVertLength[i] + deltaLength*0.5);
		}else{
			length = m_vfVertLength[i];
		}
		
		ilength = (int)length;
		flength = length - ilength;
		
		for( j = 0; j < ilength; j++){
			x = m_oPosition.x + dx * j;
			y = m_oPosition.y + dy * j;
			
			x = max(1, min( m_mImageSize.x-2, int(x+0.5)));
			y = max(1, min( m_mImageSize.y-2, int(y+0.5)));
			
			// if meet edge
			if( isEdge(x,y,edge) ){
				j = getDiagonalEdgeClippedLength(j, m_vfVertLength[i], isInit, deltaLength)+1;
				break;
			}
			if(tile != NULL)
			{
				// if meet other tile
				if( (Byte)tile->imageData[int(y) * edge->widthStep + int(x) ] == TILE){// && isInit){
					j = getDiagonalTileClippedLength(j, m_vfVertLength[i], isInit, deltaLength)+1;
					break;
				}
			}
		}
		
		m_vAlignedVert[i].x = m_oPosition.x + dx * (j-1 + flength);
		m_vAlignedVert[i].y = m_oPosition.y + dy * (j-1 + flength);
		
		m_vfVertLength[i] = sqrt((double)((m_vAlignedVert[i].x - m_oPosition.x) * (m_vAlignedVert[i].x - m_oPosition.x)
										  + (m_vAlignedVert[i].y - m_oPosition.y) * (m_vAlignedVert[i].y - m_oPosition.y)));
	}
}

//에지 클리핑 시 사용되는 함수들
// 에지 확인
bool ColoredPapperTile::isEdge(int x, int y, IplImage* edgeMap)
{
	if((Byte)edgeMap->imageData[ y * edgeMap->widthStep + x ] == EDGE
	   || (Byte)edgeMap->imageData[ (y-1)* edgeMap->widthStep + x ] == EDGE
	   || (Byte)edgeMap->imageData[ (y+1)* edgeMap->widthStep + x ] == EDGE
	   || (Byte)edgeMap->imageData[ y * edgeMap->widthStep + (x+1) ] == EDGE
	   || (Byte)edgeMap->imageData[ y * edgeMap->widthStep + (x-1) ] == EDGE
	   || (Byte)edgeMap->imageData[ (y-1) * edgeMap->widthStep + (x-1) ] == EDGE
	   || (Byte)edgeMap->imageData[ (y-1) * edgeMap->widthStep + (x+1) ] == EDGE
	   || (Byte)edgeMap->imageData[ (y+1) * edgeMap->widthStep + (x-1) ] == EDGE
	   || (Byte)edgeMap->imageData[ (y+1) * edgeMap->widthStep + (x+1) ] == EDGE)
	{
		return true;
	}
	
	return false;
}

int ColoredPapperTile::getDiagonalEdgeClippedLength(int val, double curLength, bool isInit, int deltaLength)
{
	int n = m_vAlignedVert.size();
	int angleData[7] = {0,0,0,60,45,36,30};
	double tempMinLength = (m_nMinLength)/( COS(angleData[n]) );
	if(isInit){
		return max(tempMinLength, min(curLength-1, val+3));
	}else{
		if(m_type == NEW_TILE)
			return max(tempMinLength, max( curLength - deltaLength, val ) );
		else
			return max(val, curLength-deltaLength);
	}
}

int ColoredPapperTile::getDiagonalTileClippedLength(int val, double curLength, bool isInit, int deltaLength)
{
	int n = m_vAlignedVert.size();
	int angleData[7] = {0,0,0,60,45,36,30};
	double tempMinLength = (m_nMinLength)/( COS(angleData[n]) );
	if(isInit){
		return max(tempMinLength, min(curLength-1, val+5));
	}else{
		if(m_type == NEW_TILE)
			return max(tempMinLength, max( curLength-1, val-1 ));
		else
			return max(val, curLength-deltaLength);
	}
}

int ColoredPapperTile::getEdgeClippedLength(int val, double curLength, bool isInit, int deltaLength)
{
	if(isInit){
		return max(m_nMinLength, min(curLength-1, val+3));
	}else{
		if(m_type == NEW_TILE)
			return max(m_nMinLength, max( curLength - deltaLength, val ) );
		else
			return max(val, curLength - deltaLength);
	}
}

int ColoredPapperTile::getTileClippedLength(int val, double curLength, bool isInit, int deltaLength)
{
	if(isInit){
		return max(m_nMinLength, min(curLength-1, val+5));
	}else{
		if(m_type == NEW_TILE)
			return max(m_nMinLength, max( curLength-1, val-1 ));
		else
			return max(val, curLength-deltaLength);
	}
}

