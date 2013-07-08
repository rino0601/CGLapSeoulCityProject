#pragma once
#import <vector>
using namespace std;


#import "MPoint.h"
#import "ColoredPapperTile.h"
#import "ETF.h"
class ActiveXDialog;

class ColoredPaperMosaic
{
public:
	ColoredPaperMosaic(void);
	~ColoredPaperMosaic(void);

public:

	ActiveXDialog* m_pDlg;
	ColoredPapperTile* m_optile;

	IplImage* m_pSource;
	IplImage* m_pEdge;
	IplImage* m_pTileImageMask;


	IplImage* tempMap;
	IplImage* overlapMap;
	IplImage* tileMap;
	IplImage* positionMap;
	IplImage* LayerMask;
	IplImage* m_pRegion;

	IplImage* m_pEdgeDistanceVal;
	IplImage* m_pBestPosition;
	
	IplImage* m_pDirectionAngle;
	IplImage* m_pLICfromImage;


	bool m_bHasMap;
	float m_fMinRatio;
	int m_nTileRadius;
	int m_nAdditionalEdgeWidth;

	int m_nLayerIndex;

	float m_fOverlapRate;
	float m_fRegenRatio;
	
	double m_dIndex; // 새로 만듬 4/10

	vector< IplImage* >	 m_vpEdges;	


	vector<ColoredPapperTile*> m_vpTileList;

	
	vector<MPoint> m_vpBestPosition;

	
	MPoint m_oGuidedPosition;

	MPoint tilePosition;

	

public:
	void AddTile(ColoredPapperTile* tile = NULL);
	void Render(IplImage* dst);
	void Reset();
	void setDlg(ActiveXDialog* m_pDlg);
	void preSetting(float edgeProtection=0.7);
	
	//타일이동
	void setMosaic(double index, MPoint p);

	//자동 붙이기
	bool Odering();
	void setOdering();
	void setBestPosition(IplImage* tileMap, IplImage* overlapMap, IplImage* positionMap, bool isEdge, int x, int y, int border= 4);
	void setEdgeDistanceMap();
	// 평가 함수(자동붙이기 사용)
	float ColorEval(MPoint current, MPoint previous);
	float DirectionEval(MPoint current, MPoint previous1, MPoint previous2);
	float DistanceEval(MPoint current, MPoint previous);
	float EdgeEval(MPoint current);
	float PositionEval(MPoint current);

	//ETF
	IplImage* getDirectionAngle(IplImage* src, int win=8, int iter=3);
	ETF* GetETF(IplImage* src, int win=8, int iter=3);
	IplImage* getLICfromImage(IplImage* src, int win=8, int iter=3);
	IplImage* getLICfromETF(ETF *e);
};

