#import "ColoredPaperMosaic.h"
#import "dwLIC.h"
#import "fdog.h"
#import "ActiveXDialog.h"

#ifndef CPoint
#define CPoint MPoint
#endif

#define BYTE Byte

#define NON_LAYER			0
#define NORMAL				1
#define PREMIUM				2
#define EDGE_PRESERVING		3
#define EDGE				0
#define EMPTY				0

#define TILE				255
#define OVERLAP				128

#define PI 3.141592

ColoredPaperMosaic::ColoredPaperMosaic(void) //생성자
{
	m_pTileImageMask = NULL;
	m_pSource = NULL;
	m_pEdge  = NULL;
	tempMap = NULL;
	overlapMap = NULL;
	tileMap = NULL;
	positionMap = NULL;
	m_pRegion  = NULL;
	m_optile = NULL;
	m_pDirectionAngle = NULL;
	m_pLICfromImage = NULL;
    m_pEdgeDistanceVal = NULL;
    m_pBestPosition = NULL;
	m_bHasMap = false;

	m_nLayerIndex = 0;

	m_fMinRatio = 0.32;
	m_nAdditionalEdgeWidth = 1;

	m_fOverlapRate = 0.88;
	m_fRegenRatio = 1.15;

	m_dIndex = -1;
}

ColoredPaperMosaic::~ColoredPaperMosaic(void) // 소멸자
{
	if(m_pSource)
		cvReleaseImage(&m_pSource);
    m_pSource = NULL;
	if(m_pEdge)
		cvReleaseImage(&m_pEdge);
	if(m_pTileImageMask)
		cvReleaseImage(&m_pTileImageMask);
	if(tempMap)
		cvReleaseImage(&tempMap);
	if(overlapMap)
		cvReleaseImage(&overlapMap);
	if(tileMap)
		cvReleaseImage(&tileMap);
	if(positionMap)
		cvReleaseImage(&positionMap);
	if(m_pRegion)
		cvReleaseImage(&m_pRegion);
	if(LayerMask)
		cvReleaseImage(&LayerMask);
	if(m_pEdgeDistanceVal)
		cvReleaseImage(&m_pEdgeDistanceVal);
	if(m_pBestPosition)
		cvReleaseImage(&m_pBestPosition);
	if(m_pDirectionAngle)
		cvReleaseImage(&m_pDirectionAngle);
	if(m_pLICfromImage)
		cvReleaseImage(&m_pLICfromImage);
/*	if(m_pDlg)
		delete [] m_pDlg;	*/
	if(m_optile)    delete m_optile;
/*	int n = m_vpTileList.size();
	for(int i = 0; i < n-1; i++)
		delete m_vpTileList[i];
	m_vpTileList.clear();
*/

/*	for(int i = 0; i < n; i++)
		cvReleaseImage(&(m_vpEdges[i]));*/
    
    /*for(vector<IplImage*>::iterator temp = m_vpEdges.begin(); temp != m_vpEdges.end(); temp ++) {
        IplImage* tt = *temp;
        if(tt)
            cvReleaseImage(&tt);
    }*/
    
    //m_vpEdges.clear();
    
	//m_vpBestPosition.clear();

	
}

void ColoredPaperMosaic::setDlg(ActiveXDialog* dlg)  //다이얼 로그 클래스 접근
{
	//m_pDlg를 통해 타일로그 클래스 접근
	m_pDlg = dlg;
}

//모자이크 영역 타일 추가
void ColoredPaperMosaic::AddTile(ColoredPapperTile* tile)
{
	int n;
	
	// 타일맵 초기화
	if(!m_pTileImageMask)
		m_pTileImageMask = cvCreateImage(cvGetSize(m_pDlg->m_pMosaic), 32, 1);
	
	
	
	IplImage* AccumulatedMap = cvCreateImage(cvGetSize(m_pDlg->m_pMosaic), 8, 1);
	cvSetZero(AccumulatedMap);
	
	if(m_dIndex == -1)
	{
		
		//타일 추가
		
		if(tile)
			m_vpTileList.push_back(tile);
		
		n = m_vpTileList.size();
		
		if(n > 0)
		{
			m_vpTileList[n-1]->Render(m_pDlg->m_pMosaic);
			m_vpTileList[n-1]->setTileMap(m_vpTileList[n-1]->m_oPosition.x, m_vpTileList[n-1]->m_oPosition.y, m_vpTileList[n-1]->m_nAngle, m_pTileImageMask, n-1);
			m_vpTileList[n-1]->setMaps(tileMap,overlapMap,positionMap,m_fOverlapRate, m_fRegenRatio);
			
			m_vpTileList[n-1]->setAccumulatedMap(AccumulatedMap);//
		}
		else
		{
			cvResize(m_pDlg->m_pEdge, m_pDlg->m_pMosaic,CV_INTER_LINEAR);
			
			
			// 맵 초기화
			
			cvSet(m_pTileImageMask, cvScalar(-1,-1,-1));
			
			cvSetZero(tileMap);
			cvSetZero(overlapMap);
			cvSetZero(positionMap);
			
			m_vpTileList[0]->Render(m_pDlg->m_pMosaic);
			m_vpTileList[0]->setTileMap(m_vpTileList[0]->m_oPosition.x, m_vpTileList[0]->m_oPosition.y, m_vpTileList[0]->m_nAngle, m_pTileImageMask, 0);
			m_vpTileList[0]->setMaps(tileMap,overlapMap,positionMap,m_fOverlapRate, m_fRegenRatio);
			
			m_vpTileList[0]->setAccumulatedMap(AccumulatedMap);//
		}
	}
	else // 타일 이동 시
	{
		m_vpTileList[m_dIndex] = NULL;
		m_vpTileList[m_dIndex] = tile;
		
		m_vpTileList[m_dIndex]->Render(m_pDlg->m_pMosaic);// 선택 영역 부분만 랜더링
		m_vpTileList[m_dIndex]->setTileMap(m_vpTileList[m_dIndex]->m_oPosition.x, m_vpTileList[m_dIndex]->m_oPosition.y, m_vpTileList[m_dIndex]->m_nAngle, m_pTileImageMask, m_dIndex);
		m_vpTileList[m_dIndex]->setMaps(tileMap,overlapMap,positionMap,m_fOverlapRate, m_fRegenRatio);
		
		m_vpTileList[m_dIndex]->setAccumulatedMap(AccumulatedMap);
	}
	
	
	// 	// Overlap Effect(누적효과)
	// 	// get height map
	//
  	IplImage* heightMap = cvCreateImage(cvGetSize(m_pDlg->m_pMosaic), 8, 1);
  	cvSetZero(heightMap);
  	n = m_vpTileList.size();
	
	
  	for(int i = 0; i < n; i++){
  	    //::DoEvents();
  	    m_vpTileList[i]->setAccumulatedTile(heightMap);
		
  	}
    int val,val1,val2,val3;
    int temp;
    
	Byte color0,color1,color2;
	Byte tilePosition=0;
    for(int y = 1; y < heightMap->height-1; y++){
     	for(int x = 1; x < heightMap->width-1; x++){
			tilePosition = CV_IMAGE_ELEM(AccumulatedMap, Byte, y, x);
			if(tilePosition)
			{
				val = (BYTE)heightMap->imageData[y*heightMap->widthStep + x];
				val1 = (BYTE)heightMap->imageData[(y-1)*heightMap->widthStep + x];
				val2 = (BYTE)heightMap->imageData[(y-1)*heightMap->widthStep + x-1];
				val3 = (BYTE)heightMap->imageData[y*heightMap->widthStep + x-1];
				// is shadow?
				if(val < val1 || val < val2 || val < val3){
					temp = val1 > val2 ? val1 : val2;
					temp = temp > val3 ? temp : val3;
					temp = temp - val;
					
					color0 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0);
					color1 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1);
					color2 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2);
					
					color0 = min( 255, color0 * (1-0.1*temp));
					color1 = min( 255, color1 * (1-0.1*temp));
					color2 = min( 255, color2 * (1-0.1*temp));
					
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0) = color0;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1) = color1;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2) = color2;
					continue;
				}
				
				// is high light?
				if(val > val1 || val > val2 || val > val3){
					color0 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0);
					color1 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1);
					color2 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2);
					
					color0 = min( 255, color0 * 1.1);
					color1 = min( 255, color1 * 1.1);
					color2 = min( 255, color2 * 1.1);
					
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0) = color0;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1) = color1;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2) = color2;
					
					continue;
				}
				
				val1 = (BYTE)heightMap->imageData[(y-1)*heightMap->widthStep + x +1];
				val2 = (BYTE)heightMap->imageData[(y+1)*heightMap->widthStep + x -1];
				
				// is edge?
				if(val > val1 || val > val2){
					color0 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0);
					color1 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1);
					color2 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2);
					
					color0 = min( 255, color0 * 0.95);
					color1 = min( 255, color1 * 0.95);
					color2 = min( 255, color2 * 0.95);

					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0) = color0;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1) = color1;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2) = color2;
				}
			}
			
		}
	}
	
	cvReleaseImage(&heightMap);
	cvReleaseImage(&AccumulatedMap);
	
	if(m_dIndex != -1)
		m_vpTileList[m_dIndex]->m_vTileAround.clear();
	
	m_dIndex = -1;
	
	
	
} 

void ColoredPaperMosaic::setMosaic(double index, MPoint p) // 타일 인덱스, 타일 중심점
{
	float maxDis = 0;
	float dis;
	
	double tileNum;
	double TAround_index;
	bool save = true;
	
	m_dIndex = index;
	
	
	IplImage* AccumulatedMap = cvCreateImage(cvGetSize(m_pDlg->m_pMosaic), 8, 1);
	cvSetZero(AccumulatedMap);
	
	
	int n = m_vpTileList[index]->m_vfVertLength.size();
	
	for(int i = 0; i < n; i++)
	{
		dis = m_vpTileList[index]->m_vfVertLength[i];
		if(dis > maxDis)
			maxDis = dis;
	}
	
	CPoint Max,Min;
	
	// 검색 영역 설정 !!!!
	
	Min.y = p.y - (maxDis+3);
	if(Min.y < 0 )
		Min.y = 0;
	
	Min.x = p.x - (maxDis+3);
	if( Min.x < 0)
		Min.x = 0;
	
	Max.y = p.y + (maxDis+3);
	if(Max.y > m_pDlg->m_pMosaic->height)
		Max.y = m_pDlg->m_pMosaic->height;
	
	Max.x = p.x + (maxDis+3);
	if(Max.x > m_pDlg->m_pMosaic->width)
		Max.x = m_pDlg->m_pMosaic->width;
	
	
	
	//주변 타일 검색 및 저장
 	for(int j = Min.y;  j < Max.y; j++)
 	{
		for(int i = Min.x; i < Max.x; i++)
		{
			
			tileNum = CV_IMAGE_ELEM(m_pTileImageMask, float, j, i);
			
			if(tileNum == -1)
				continue;
			
			save = true;
			
			if(index != tileNum) //
			{
				n =  m_vpTileList[index]->m_vTileAround.size();
				if(n > 0)
				{
					for(int k = 0; k < n; k++)
					{
						TAround_index =m_vpTileList[index]->m_vTileAround[k];
						
						if(tileNum == TAround_index)
						{
							save = false;
							break;
						}
					}
					
					if(save == true)
					{
						m_vpTileList[index]->m_vTileAround.push_back(tileNum);
					}
					else
					{
						continue;
					}
				}
				else
				{
					m_vpTileList[index]->m_vTileAround.push_back(tileNum);
				}
				
			}
			
		}
	}
	
	// 선택 영역 초기화
	for(int j = Min.y;  j < Max.y; j++)
	{
		for(int i = Min.x; i < Max.x; i++)
		{
			int index_mosaic = ((j)*m_pDlg->m_pMosaic->widthStep)+i*4;
			
			m_pDlg->m_pMosaic->imageData[index_mosaic]   = m_pDlg->m_pEdge->imageData[index_mosaic];
			m_pDlg->m_pMosaic->imageData[index_mosaic+1] = m_pDlg->m_pEdge->imageData[index_mosaic+1];
			m_pDlg->m_pMosaic->imageData[index_mosaic+2] = m_pDlg->m_pEdge->imageData[index_mosaic+2];
			
		}
	}
	
	// 마스크맵 초기화
	m_vpTileList[index]->setTileMap(m_vpTileList[index]->m_oPosition.x, m_vpTileList[index]->m_oPosition.y, m_vpTileList[index]->m_nAngle, m_pTileImageMask, -1);
	m_vpTileList[index]->reSetMaps(tileMap,overlapMap,positionMap,m_fOverlapRate, m_fRegenRatio);
	
	n =  m_vpTileList[index]->m_vTileAround.size();
	
	IplImage* heightMap = cvCreateImage(cvGetSize(m_pDlg->m_pMosaic), 8, 1);
	cvSetZero(heightMap);
	
	// 주변타일 랜더링, 마스크맵 재 생성
	for(int i = 0 ; i < n ; i ++)
	{
		tileNum = m_vpTileList[index]->m_vTileAround[i];
		
		m_vpTileList[tileNum]->SelectRender(m_pDlg->m_pMosaic, Max, Min);
		
		m_vpTileList[tileNum]->setTileMap(m_vpTileList[tileNum]->m_oPosition.x, m_vpTileList[tileNum]->m_oPosition.y, m_vpTileList[tileNum]->m_nAngle, m_pTileImageMask, tileNum);
		
		m_vpTileList[tileNum]->setMaskMaps(tileMap,overlapMap,positionMap,m_fOverlapRate, m_fRegenRatio);
		
		m_vpTileList[tileNum]->setAccumulatedTile(heightMap);
		
		m_vpTileList[tileNum]->setAccumulatedMap(AccumulatedMap);//
	}
	
	
	//누적효과
	
	int val,val1,val2,val3;
	int temp;
	
	Byte color0,color1,color2;
	Byte tilePosition=0;
	for(int y = 1; y < heightMap->height-1; y++){
		for(int x = 1; x < heightMap->width-1; x++){
			tilePosition = CV_IMAGE_ELEM(AccumulatedMap, Byte, y, x);
			if(tilePosition)
			{
				val = (BYTE)heightMap->imageData[y*heightMap->widthStep + x];
				val1 = (BYTE)heightMap->imageData[(y-1)*heightMap->widthStep + x];
				val2 = (BYTE)heightMap->imageData[(y-1)*heightMap->widthStep + x-1];
				val3 = (BYTE)heightMap->imageData[y*heightMap->widthStep + x-1];
				// is shadow?
				if(val < val1 || val < val2 || val < val3){
					temp = val1 > val2 ? val1 : val2;
					temp = temp > val3 ? temp : val3;
					temp = temp - val;
					
					color0 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0);
					color1 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1);
					color2 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2);
					
					color0 = min( 255, color0 * (1-0.1*temp));
					color1 = min( 255, color1 * (1-0.1*temp));
					color2 = min( 255, color2 * (1-0.1*temp));
					
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0) = color0;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1) = color1;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2) = color2;
					continue;
				}
				
				// is high light?
				if(val > val1 || val > val2 || val > val3){
					color0 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0);
					color1 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1);
					color2 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2);
					
					color0 = min( 255, color0 * 1.1);
					color1 = min( 255, color1 * 1.1);
					color2 = min( 255, color2 * 1.1);
					
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0) = color0;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1) = color1;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2) = color2;
					continue;
				}
				
				val1 = (BYTE)heightMap->imageData[(y-1)*heightMap->widthStep + x +1];
				val2 = (BYTE)heightMap->imageData[(y+1)*heightMap->widthStep + x -1];
				
				// is edge?
				if(val > val1 || val > val2){
					color0 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0);
					color1 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1);
					color2 = CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2);
					
					color0 = min( 255, color0 * 0.95);
					color1 = min( 255, color1 * 0.95);
					color2 = min( 255, color2 * 0.95);
					
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +0) = color0;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +1) = color1;
					CV_IMAGE_ELEM(m_pDlg->m_pMosaic, Byte, y, m_pDlg->m_pMosaic->nChannels*x +2) = color2;
				}
			}
			
		}
	}
	
	cvReleaseImage(&heightMap);
	cvReleaseImage(&AccumulatedMap);
	
	m_vpTileList[index]->m_vTileAround.clear();
	
}

// 자동 붙이기를 위한 맵 생성
void ColoredPaperMosaic::preSetting(float edgeProtection)
{
	
	if(m_pRegion)
		cvReleaseImage(&m_pRegion);
	m_pRegion = cvCreateImage( cvGetSize(m_vpEdges[0]), 8, 1);
	
	cvSetZero(m_pRegion); // init all region as non layer
	
	
	int idis;
	int minTile = m_nTileRadius * m_fMinRatio;
	
	int premium1 = int(m_nTileRadius * SIN( 50 ) - 1 + m_nAdditionalEdgeWidth);
	int premium2 = int(m_nTileRadius * SIN( 50 ) + 1 + m_nAdditionalEdgeWidth + 0.5);
	
	int minRegion = int(m_nAdditionalEdgeWidth + minTile * edgeProtection);
	for(int y = 0; y < m_pRegion->height; y++){
		for(int x= 0; x < m_pRegion->width; x++){
			idis = (BYTE)m_vpEdges[0]->imageData[ y* m_vpEdges[0]->widthStep + x];
			
			if( idis > 0 && idis <= minRegion)			//by edge protection
				m_pRegion->imageData[y* m_pRegion->widthStep + x] = EDGE_PRESERVING;
			
			if( idis > minRegion)						//set normal layer
				m_pRegion->imageData[y* m_pRegion->widthStep + x] = NORMAL;
			
			if(idis >= premium1 && idis <= premium2)	//by tile radius
				m_pRegion->imageData[y* m_pRegion->widthStep + x] = PREMIUM;
		}
	}
	
	overlapMap = cvCreateImage(cvGetSize(m_pRegion), 8, 1);
	tileMap = cvCreateImage(cvGetSize(m_pRegion), 8, 1);
	positionMap = cvCreateImage(cvGetSize(m_pRegion), 32, 1);
	
	if(!m_pTileImageMask) //왜하는건진 모르겠지만 이미 했을 가능성이 매우 높음.
		m_pTileImageMask = cvCreateImage(cvGetSize(m_pDlg->m_pMosaic), 32, 1);
	
	cvSet(m_pTileImageMask, cvScalar(-1,-1,-1));
	
	cvSetZero(tileMap);
	cvSetZero(overlapMap);
	cvSetZero(positionMap);
	
}

// 자동 붙이기 전처리 작업, 후보점 정의, EdgeDistanceMap 생성 
void ColoredPaperMosaic::setOdering()
{
	// 타일 생성 후보맵 생성
	m_pBestPosition = cvCreateImage(cvGetSize(m_pDlg->m_pMosaic), IPL_DEPTH_32F, 1); 
	cvSet(m_pBestPosition,cvScalar(0.0));

	
	tilePosition.x = tilePosition.y = 0;

	// 후보점 탐색
	setBestPosition(tileMap, overlapMap, positionMap, true, tilePosition.x, tilePosition.y); 
	setBestPosition(tileMap, overlapMap, positionMap, false, tilePosition.x, tilePosition.y); 
	
	// EdgeDistanceMap 생성
	setEdgeDistanceMap();

}

// 자동 붙이기 시 초기 작업으로 타일이 붙기 좋은 위치들 생성(모자이크 영역에 붙여진 타일, 오버랩, 에지를 고려하여 선정)
void ColoredPaperMosaic::setBestPosition(IplImage* tileMap, IplImage* overlapMap, IplImage* positionMap, bool isEdge, int x, int y, int border)
{

	MPoint ret;
	ret.y = ret.x = -1;

	int width = m_pRegion->width;
	int height = m_pRegion->height;

	int i,j; 
	int max_x, max_y, max_val;
	int wstart, wend, hstart, hend;
	// 영역별 베스트 포지션 탐색
	for(int m = 0; m < height;  m = m + (m_pDlg->m_fside)/4 +0.5 )
	for(int n = 0; n < width;   n = n + (m_pDlg->m_fside)/4 +0.5 )
	{
		for(y = m; y < m + (m_pDlg->m_fside)/4 +0.5; y++)
		for(x = n; x < n + (m_pDlg->m_fside)/4 +0.5; x++)
		{

			int p = y*m_pRegion->widthStep+x;

			if((Byte)m_pBestPosition->imageData[p] != 0)
				continue;	

			if((Byte)tileMap->imageData[p] == TILE)
				continue;
			if((Byte)overlapMap->imageData[p] == OVERLAP)
				continue;
			if(isEdge){
				if((Byte)m_pRegion->imageData[p] != PREMIUM)
					continue;
			}else{
				if((Byte)m_pRegion->imageData[p] == NON_LAYER || (Byte)m_pRegion->imageData[p] == EDGE_PRESERVING)
					continue;			
			}		

			i = p % m_pRegion->widthStep;
			j = p / m_pRegion->widthStep;

			if(i < border || j < border || i >= width-border || j >= height-border)
				continue;

			wstart = max(border, i-m_nTileRadius/2);
			wend = min(width-border, i+m_nTileRadius/2);

			hstart = max(border, j-m_nTileRadius/2);
			hend = min(height-border, j+m_nTileRadius/2);

			ret.x = max_x = i;
			ret.y = max_y = j;
			max_val = (Byte)positionMap->imageData[j*positionMap->widthStep + i];

			for(int k = wstart; k < wend; k++)
			{
				for(int l = hstart; l < hend; l++)
				{

					if((Byte)tileMap->imageData[l * tileMap->widthStep + k] == TILE)
						continue;
					if((Byte)overlapMap->imageData[l * overlapMap->widthStep + k] == OVERLAP)
						continue;
					if(isEdge){
						if((Byte)m_pRegion->imageData[l * m_pRegion->widthStep + k] != PREMIUM)
							continue;					
					}else{
						if((Byte)m_pRegion->imageData[l * m_pRegion->widthStep + k] == NON_LAYER || (Byte)m_pRegion->imageData[l * m_pRegion->widthStep + k] == EDGE_PRESERVING)
							continue;					
					}

					if((Byte)m_pBestPosition->imageData[l*m_pBestPosition->widthStep + k] != 0)
						continue;	
					

					if((Byte)positionMap->imageData[l*positionMap->widthStep + k] > max_val)
					{
						max_val = (Byte)positionMap->imageData[l*positionMap->widthStep + k];
						ret.x = max_x = k;
						ret.y = max_y = l;
					}					


				}
			}
			break;
		}
		
		if(ret.x == -1.0 && ret.y == -1.0 )
			continue;
		float val;
		val = CV_IMAGE_ELEM(m_pBestPosition, float, ret.y, ret.x);
		if(val == 0.0)
		{
			CV_IMAGE_ELEM(m_pBestPosition, float, ret.y, ret.x) = 1.0;
		}		
	}
}

// 각 픽셀과 에지와의 거리에 대한 맵 생성
void ColoredPaperMosaic::setEdgeDistanceMap()
{
	int w,h;

	Byte r,g,b;

	int x, y;

	vector<MPoint> vEdge;
	IplImage* Voronoib_Img;
	IplImage* Voronoi_Img;
	
	Voronoib_Img = cvCreateImage(cvGetSize(m_pDlg->gray), IPL_DEPTH_8U, 4);
	Voronoi_Img = cvCreateImage(cvGetSize(m_pDlg->gray), IPL_DEPTH_8U, 4);

	w = m_pDlg->gray->width;
	h = m_pDlg->gray->height;
	
	for(int j = 0 ; j < h ; j++)
	for(int i = 0 ; i < w ; i++)
	{
		if(125 > (Byte)m_pDlg->gray->imageData[j*m_pDlg->gray->widthStep + i])
		{
			MPoint p;
			p.x = i;
			p.y = j;
			vEdge.push_back(p);
		}
	}

	int size = vEdge.size();
	
	// 에지위에 에지의 위치를 색상으로 한 콘을 생성.

	CGLView backup = m_pDlg->OpenGLView; // 안 필요 할 지도.
	CvSize  cgsize = cvGetSize(m_pDlg->gray);
	m_pDlg->OpenGLView.AllocInitWithFrame(CGRectMake(0, 0, cgsize.width, cgsize.height));
	m_pDlg->Clear(0,0,0);
	for(int i = 0; i < size; i++){
		r=Byte(vEdge[i].x % 256);
		g=Byte(vEdge[i].y % 256);
		b=Byte((vEdge[i].x / 256) * 16 + vEdge[i].y / 256);
		m_pDlg->DrawCone( vEdge[i].x, vEdge[i].y, r,g,b);
	}

	m_pDlg->getBackBuffer(Voronoib_Img);
	m_pDlg->OpenGLView = backup;

	for(int i = 0; i < Voronoib_Img->height; i++){
		memcpy( &Voronoi_Img->imageData[i*Voronoi_Img->widthStep],
			&Voronoib_Img->imageData[(Voronoib_Img->height-1-i)*Voronoib_Img->widthStep],
			sizeof(Byte)*Voronoib_Img->widthStep);
	} 
	m_pDlg->sem.out();
		

	m_pEdgeDistanceVal = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1); 
	cvSet(m_pEdgeDistanceVal,cvScalar(-1.0));

	float val;
	float dis;

	// 에지와의 거리에 따라 각 픽셀에 값을 지정
	for(int j = 0 ; j < h ; j++)
	for(int i = 0 ; i < w ; i++)
	{
		// 에지와 같은 색상의 픽셀을 찾아 값을 지정
		r = (Byte)Voronoi_Img->imageData[j*Voronoi_Img->widthStep + i*4 +2];
		g = (Byte)Voronoi_Img->imageData[j*Voronoi_Img->widthStep + i*4 +1];
		b = (Byte)Voronoi_Img->imageData[j*Voronoi_Img->widthStep + i*4];

		if(r==255 && g==255 && b == 255)
		{
			CV_IMAGE_ELEM(m_pEdgeDistanceVal, float, j, i) = 0;
			continue;
		}

		x = (int)(Byte)r + ((int)(Byte)b/16)*256;
		y = (int)(Byte)g + ((int)(Byte)b%16)*256;

		dis = sqrt(float((x-i)*(x-i) + (y-j)*(y-j)));
		int side =  m_pDlg->m_fside/2 + 0.5;
		val = (dis / side);


		if(dis == 0)
		{
			CV_IMAGE_ELEM(m_pEdgeDistanceVal, float, j, i) = 0.0;
		}
		else if(0< dis && dis<=side)
		{
			val = powf(dis/side,0.5);
			if(val >= 1.0)
			{
				val = 1.0;
			}
			CV_IMAGE_ELEM(m_pEdgeDistanceVal, float, j, i) = val;
		}

		else if(side< dis && dis<=side*2)
		{
			val = powf(2,(side-dis)/6);

			if(val >= 1.0)
			{
				val = 1.0;
			}
			CV_IMAGE_ELEM(m_pEdgeDistanceVal, float, j, i) = val;
		}

		else if(side*2< dis )
		{
			val = 0.2;
			CV_IMAGE_ELEM(m_pEdgeDistanceVal, float, j, i) = val;
		}

		
	}

	if(Voronoib_Img)
		cvReleaseImage(&Voronoib_Img);
	
	if(Voronoi_Img)
		cvReleaseImage(&Voronoi_Img);
	
	vEdge.clear();	
}

//색상 평가(자동붙이기): 이전에 붙여진 색종이 타일의 색상과 다음 붙여질 타일 위치의 원본 색상을 비교 평가 
float ColoredPaperMosaic::ColorEval(MPoint current, MPoint previous)
{
	Byte cR,cG,cB; // 현재 컬러
	Byte pR,pG,pB; // 이전 컬러
	float Sacle;
	float ratio_r,ratio_g ,ratio_b;  
	float eval;

	//다음 붙여질 위치의 원본 이미지의 색상
	cR = (Byte)m_pSource->imageData[(int)current.y*m_pSource->widthStep + (int)current.x*4 +2];
	cG = (Byte)m_pSource->imageData[(int)current.y*m_pSource->widthStep + (int)current.x*4 +1];
	cB = (Byte)m_pSource->imageData[(int)current.y*m_pSource->widthStep + (int)current.x*4 ];

	//이전에 붙여진 타일의 색상
	pR = (Byte)m_pSource->imageData[(int)previous.y*m_pSource->widthStep + (int)previous.x*4 +2];
	pG = (Byte)m_pSource->imageData[(int)previous.y*m_pSource->widthStep + (int)previous.x*4 +1];
	pB = (Byte)m_pSource->imageData[(int)previous.y*m_pSource->widthStep + (int)previous.x*4 ];

	if(abs(cR-pR) == 0)
	{
		ratio_r = 0;
	}
	else
	{
		ratio_r = 100 - ((float(abs(cR-pR))/255) * 100);
	}

	if(abs(cG-pG) == 0)

	{
		ratio_g = 0;
	}
	else
	{
		ratio_g = 100 - ((float(abs(cG-pG))/255) * 100);
	}

	if(abs(cB-pB) == 0)
	{
		ratio_b = 0;
	}
	else
	{
		ratio_b = 100 - ((float(abs(cB-pB))/255) * 100);
	}

	if(ratio_r == 0 && ratio_g == 0 && ratio_b == 0 )
	{
		Sacle = 0;
		return  1.0;
	}
	else
	{
		Sacle = (ratio_r + ratio_g + ratio_b) / 300 ;

		eval =  1.0 - Sacle;

		return eval;
	}

}

// 방향성 평가(자동붙이기): 이전에 붙여진 타일들의 방향성과 유사한 방향성을 갖는 다음 위치를 결정하기 위해 사용
float ColoredPaperMosaic::DirectionEval(MPoint current, MPoint previous1, MPoint previous2)
{
	float prev_Angle;	// p2->p1 angle
	float p1Angle;		// p1 angle
	float cur_Angle;	// p1->curr angle
	int x,y;

	float angle;
	float eval;

	// 이전 타일의 방향성 결정
	prev_Angle = atan2(previous2.y - previous1.y, previous2.x - previous1.x) * (PI/180.0f);
	if(prev_Angle < 0)
	{
		prev_Angle+=360;
	}


	x = previous1.x;
	y = previous1.y; 	
	p1Angle = 0;
    //((float*)(m_pDirectionAngle->imageData + (m_pDirectionAngle->widthStep * y)))[x] ;

	
	if(prev_Angle > 90 && prev_Angle < 270)					//2,3 사분면
	{
		if(p1Angle<=90)
		{
			p1Angle = p1Angle + 180;
		}
		else if(p1Angle>=270)
		{
			p1Angle = p1Angle - 180;
		}
	}
	else if(prev_Angle<=90 || prev_Angle>=270)				//1,4 사분면	
	{
		if(p1Angle > 90 && p1Angle <=  180)
		{
			p1Angle = p1Angle + 180;
		}
		else if(p1Angle > 180 && p1Angle < 270)
		{
			p1Angle = p1Angle - 180;
		}

	}

	// 후보 점과의 방향성 유사도 평가
	cur_Angle = atan2(previous1.y - current.y, previous1.x - current.x) * (PI/180.0f);
	if(cur_Angle < 0)
	{
		cur_Angle+=360;
	}

	angle = fabs(p1Angle - cur_Angle);

	if(angle < 180)
	{
		eval =  (180-angle)/180;
	}
	else
	{
		eval = (angle/180) - 1;
	}

	return eval;

}

//타일 거리평가 함수(자동붙이기): 이전에 붙여진 색종이 조각과 다음 붙여질 위치와의 거리 평가함수
float ColoredPaperMosaic::DistanceEval(MPoint current, MPoint previous)
{
	float dis=0;
	float eval=0;
	float side=0;
	
	//거리가 타일의 지름일 때 좋은 위치라고 평가
	side = (int)((m_pDlg->m_fside) + 0.5); 
	dis = sqrt(float((current.x-previous.x)*(current.x-previous.x) + (current.y-previous.y)*(current.y-previous.y)));

	if(side < dis) {
		eval =  powf(2,(side-dis)/10);		
	} else if(0 < dis && dis <=side) {
		eval = powl(dis/side, 3);		
	} else if(0 == dis ){
		eval = 0.0;		
	} else if(dis == side) {
		eval = 1.0;		
	}
	
	return eval;

}

// 에지와의 거리 평가함수(자동 붙이기): EdgeDistance 맵(m_pEdgeDistanceVal)을 사용(전처리 과정에서 생성)
float ColoredPaperMosaic::EdgeEval(MPoint current)
{
	float eval;
	eval = CV_IMAGE_ELEM(m_pEdgeDistanceVal, float, current.y, current.x);
	return eval;
}

//자동붙이기시 BestPosition 맵을 통해 생성된 후보점만을 평가 값을 적용(전처리 과정에서 맵(m_pBestPosition) 생성)
float ColoredPaperMosaic::PositionEval(MPoint current)
{
	float eval;
	eval = CV_IMAGE_ELEM(m_pBestPosition, float, current.y, current.x);
	return eval;
}

//자동 붙이기
bool ColoredPaperMosaic::Odering()
{
	if(m_pDlg->m_bAutoGeneration==false)
		return false;

	int width = tileMap->width;
	int height = tileMap->height;

	float Eval1,Eval2;
	float Ee,Et,Ec,Ed,Edis;
	MPoint current, previous1, previous2;

	int size = m_vpBestPosition.size();

	
	int angle;	
	angle = 0;
	
	tilePosition.x = tilePosition.y = -1;

	bool search = true;


	if(size == 0) { // 붙여진 타일이 없을 때 임의의 점 생성
		for(int j = 0; j < height; j++) {
			for(int i =0; i < width; i++) {
				int n = j*m_pRegion->widthStep+i;

// TODO: start tag
				if((Byte)LayerMask->imageData[n] != this->m_pDlg->getStartSectionCode())
					continue;
				
				if((Byte)tileMap->imageData[n] == TILE) // 타일이 있을 때 제외
					continue;

				if((Byte)overlapMap->imageData[n] == OVERLAP) // 오버랩 되는 영역 제외
					continue;

				current.x = i;
				current.y = j;

				Ee = EdgeEval(current);      // 에지와 픽셀 사이의 거리에 대한 평가 값 리턴
				Et = PositionEval(current);  // 위치가 지정 된 픽셀일 경우 1값 리턴
				
//				if (Et ==1 || Ee > 0.9) {
//					int k;
//				}
				
				if(Et == 1 && Ee > 0.9 ) {
					previous1.x = i;
					previous1.y = j;
					m_vpBestPosition.push_back(previous1);
					tilePosition = previous1;
					search = false;
					break;
				}
			}
			
			if(search == false) {
				search = true;
				break;
			}
		}
	} else { // 타일이 하나 이상 붙었을 경우
		int k,l;
		size  = m_vpBestPosition.size();
		k = m_vpBestPosition[size - 1].x;	
		l = m_vpBestPosition[size - 1].y;

		Eval1 = Eval2 = 0.0;
		for(int j = 0; j < height; j++) {
			for(int i = 0; i < width; i++) {
				Ee = Et = Ec= Ed = Edis= 0.0;

				int n = j*m_pRegion->widthStep+i;

				if((Byte)LayerMask->imageData[n] == 255) // 이미지 바탕제외 (마크스에 255로 지정되어야 함.)
					continue;

				if((Byte)tileMap->imageData[n] == TILE)  // 타일이 위치한 경우 제외
					continue;
				if((Byte)overlapMap->imageData[n] == OVERLAP) // 오버랩 되는 위치일 경우 제외
					continue;

				current.x = i;
				current.y = j;
			
				Et = PositionEval(current);
				if(Et != 1)
					continue;


				// 원본 소스로 부터 색상 받아 옴.
				Byte color10,color11, color12;
				color10 = CV_IMAGE_ELEM(m_pSource, Byte, j, m_pSource->nChannels*i+0);
				color11 = CV_IMAGE_ELEM(m_pSource, Byte, j, m_pSource->nChannels*i+1);
				color12 = CV_IMAGE_ELEM(m_pSource, Byte, j, m_pSource->nChannels*i+2);
				
				if(color10 < 30 && color11 < 30 && color12 < 30){
					continue;
				}
				

			
				size  = m_vpBestPosition.size();
				previous1 = m_vpBestPosition[size -1];			

 				Ee = EdgeEval(current); 	   			 // 에지와의 거리 평가

				Ec = ColorEval(current, previous1 );	 // 색상 평가

				Edis = DistanceEval(current, previous1); // 타일과의 거리 평가

				if(size > 1 ) { // 자동 붙이기를 통해 타일이 하나 이상 붙었을 경우 적용
					previous1 = m_vpBestPosition[size -1];
					previous2 = m_vpBestPosition[size -2];
					Ed = DirectionEval( current,  previous1, previous2); // 방향성 평가												
				}

				//가중치 적용, 가중치 값 변경을 통해 붙여지는 방식을 다르게 함
				//Eval1 = Ee*0.1  + Ec*0.48 + Ed*0.1 + Edis*0.266;        //레이어 별로 붙이기
				  Eval1 = Ee*0.27  + Ec*0.345 + Ed*0.13 + Edis*0.255;     // 기본
				//Eval1 = Ee*0.375 + Ec*0.13 + Ed*0.14 + Edis*0.355;
				//Eval1 = Ee*0.370 + Ec*0.13 + Ed*0.14 + Edis*0.36;

				//Eval1 = Ee*0.1 + Ec*0.1 + Ed*0.4+ Edis*0.30;             방향 기반
				//Eval1 = Ee*0.1 + Ec*0.3 + Ed*0.1+ Edis*0.5; 

				//Eval1 = Ee*0 + Ec*0 + Ed*0+ Edis*1; 
				if(Eval1 > Eval2){
					Eval2 = Eval1;

					tilePosition.x =i;
					tilePosition.y =j;			
				}
			}
		}
		m_vpBestPosition.push_back(tilePosition);  //BestPosition 저장.
	}
	
	if(tilePosition.x == -1) {
		search = false;
	}


	if(search) {
		/*베스트 후보에 타일 생성*/
		m_optile = new ColoredPapperTile();
		
		
		Byte c0 = CV_IMAGE_ELEM(m_pSource, Byte, tilePosition.y, m_pSource->nChannels*tilePosition.x+0);
		Byte c1 = CV_IMAGE_ELEM(m_pSource, Byte, tilePosition.y, m_pSource->nChannels*tilePosition.x+1);
		Byte c2 = CV_IMAGE_ELEM(m_pSource, Byte, tilePosition.y, m_pSource->nChannels*tilePosition.x+2);
		Byte c3 = CV_IMAGE_ELEM(m_pSource, Byte, tilePosition.y, m_pSource->nChannels*tilePosition.x+3);
		CvScalar positionColor = cvScalar(c0,c1,c2,c3);
		
		m_optile->setPosition(tilePosition.x,tilePosition.y,angle,width,height, positionColor );
		m_optile->generation(overlapMap, m_vpEdges[0], NULL, m_nTileRadius, m_fMinRatio,m_nAdditionalEdgeWidth);
		m_optile->setFractalInfo();
		m_optile->ColoredLayer(NULL);
		m_optile->WhiteLayer(NULL);
		
		m_pDlg->AutoTileMove();
		AddTile(m_optile);
		
		
		Byte n0 = CV_IMAGE_ELEM(m_pSource, Byte, tilePosition.y, m_pSource->nChannels*tilePosition.x+0);
		Byte n1 = CV_IMAGE_ELEM(m_pSource, Byte, tilePosition.y, m_pSource->nChannels*tilePosition.x+1);
		Byte n2 = CV_IMAGE_ELEM(m_pSource, Byte, tilePosition.y, m_pSource->nChannels*tilePosition.x+2);
		Byte n3 = CV_IMAGE_ELEM(m_pSource, Byte, tilePosition.y, m_pSource->nChannels*tilePosition.x+3);
		m_pDlg->m_Color = cvScalar(n0,n1,n2,n3);
	}
	return search;
}

//ETF를 구하기 위한 함수들
IplImage* ColoredPaperMosaic::getDirectionAngle(IplImage* src, int win, int iter)
{
	ETF* e;
	IplImage* gray = cvCreateImage( cvGetSize(src), src->depth, 1);
	//cvCvtColor( src, gray, CV_BGR2GRAY );
	// FIXME: memory leek
	gray = src;
	e = GetETF(gray, win, iter);
	IplImage* ret = cvCreateImage( cvGetSize(src), IPL_DEPTH_32F, 1);

	int x = src->width;
	int y = src->height;
	int angle;

	double dx,dy;
	for(int j = 0; j < y; j++){
		for(int i = 0; i < x; i++){
			dx = e->p[i][j].tx;
			dy = e->p[i][j].ty;
			angle = acos( dx ) * 180 / 3.14;
			if( dy < 0){
				angle = 360 - angle;
			}

			((float*)(ret->imageData + ret->widthStep*j))[i] = angle;
		}
	}

	e->delete_all();
	cvReleaseImage(&gray);
	return ret;
}

ETF* ColoredPaperMosaic::GetETF(IplImage * src, int win, int iter)
{
	unsigned int y = src->height, x = src->width;
	imatrix img(x, y);

	// Copy gradients from cv to imatrix
	for(unsigned int j = 0 ; j < y ; j++)
	{
		for (unsigned int i = 0 ; i < x ; i++)
		{
			img[i][j] = int(cvGetReal2D(src, j, i));
			// FIXME: dead code.
		}
	}

	// ETF Interpolation
	ETF* e;
	e = new ETF();
	e->init(x, y);
	e->set(img); // get gradients from gradient map
	e->Smooth(win, iter);

	return e;
}

IplImage* ColoredPaperMosaic::getLICfromImage(IplImage* src, int win, int iter)
{
	ETF* e;
	IplImage* gray = cvCreateImage( cvGetSize(src), src->depth, 1);
	cvCvtColor( src, gray, CV_BGR2GRAY );
	e = GetETF(gray, win, iter);
	IplImage* ret = getLICfromETF(e);
	e->delete_all();
	return ret;
}

IplImage* ColoredPaperMosaic::getLICfromETF(ETF *e)
{
	int x = e->getRow();
	int y = e->getCol();
	dwLIC lic(x, y);
	int index;
	for(int j = 0; j < y; j++){
		for(int i = 0; i < x; i++){
			index = (j * x * 2 + i * 2);
			lic.pVectr[index ] = e->p[i][j].tx;
			lic.pVectr[index+1] = e->p[i][j].ty;
		}
	}
	lic.FlowImagingLIC();
	IplImage* licImage = lic.GetLICimage();
	IplImage* ret = cvCreateImage(cvGetSize(licImage),8,1);
	for(int j = 0; j < ret->height; j++){
		for(int i = 0; i < ret->width; i++){
			ret->imageData[j* ret->widthStep + i] = (Byte)licImage->imageData[(ret->height-j-1)*ret->widthStep + i];
		}
	}
	return ret;

}