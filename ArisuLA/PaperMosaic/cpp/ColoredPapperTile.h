#pragma once
#import <vector>
using namespace std;

#import "MPoint.h"
#import "TornEffect.h"

class ColoredPapperTile
{
public:
	ColoredPapperTile(int saveCount=0);
	~ColoredPapperTile(void);

public:
	// 색종이 조각의 속성들
	int x;
	int y;

	Byte r;
	Byte g;
	Byte b;

	long ar;
	long ag;
	long ab;
	int ccount;
	
	int m_type;
	MPoint minV;
	MPoint maxV;

	int m_nRadius;
	int m_nAdditionalEdge;

	int m_nMinLength;

	float m_fMinRatio;
	MPoint m_oPosition;	
	MPoint m_mImageSize;
	int m_nAngle;
	bool m_bSelected;
	
	bool m_bCutted; //isCutted
	bool isNeiborCutted;
	

	vector< bool > NoEffect;			// 색종이 외각 부분 톤이펙트 효과 결정
	vector< bool > isCuttedVert;		
	vector<int> CuttedVertNum;
	vector<int> CuttedTileNum;	

	vector<MPoint> vertex;
	vector<MPoint> m_vAlignedVert;	

	vector<CvScalar> m_vColor;	
	vector<int> m_vnVertAngle;
	vector<float> m_vfVertLength;
	vector<float> m_vfLength;
	
	vector< MPoint > m_vColorVert;
	vector< MPoint > m_vWhiteVert;

	vector<MPoint> m_vWhiteBase;
	vector< bool > m_vbIsWhiteVertSet;	
	
	vector<MPoint> m_vMinVert;
	
	vector<TornEffect*> m_vpTornOfColored;
	vector<TornEffect*> m_vpTornOfWhite;
	
	//
	vector<double> m_vTileAround;

public:
	void Destroy();

	void Render(IplImage* dst, bool isShadow=true, IplImage* temp=NULL);
	void setTileMap(float x, float y, int angle, IplImage* tileMap, float val);

	void RenderAt(IplImage* dst, MPoint position, int angle);
	
	void RenderToDC(CGContextRef ctx,MPoint position);
	
	void Moveto(MPoint position);
	void Rotate(int deltaAngle);
	void Resize(float lengthRatio);
	void RandomMove(int range, int maxx, int maxy);

	void AddColor(Byte R, Byte G, Byte B);
	void SetAvgColor();
	void AddVertex(int x, int y);
	void AlignVerties();
	void Clear();
	
	void setFractalInfoAsNULL();
	void setFractalInfo();
	void setFractalInfo(int i, bool isWhite, TornEffect* effect);
	
	void updateBaseVert();
	void ColoredLayer(IplImage* dis=NULL, int minDis=2, float ratio=1.0f);
	void WhiteLayer(IplImage* dis=NULL, int whiteRange=7, int minDis=2, float ratio=1.0f, float ratio2=1.0f);
	void WhiteVertices(int n, int whiteRange=7, bool b = true);

	int setChangeVert(int px, int py , int vX, int vY);
	void setCuttedVert(int px, int py,  int vertexnum , int TileNum);

	void setMaps(IplImage* tileMap, IplImage* overlapMap, IplImage* positionMap, float overlapRatio, float regenRatio, IplImage* temp=NULL);
	void generation(IplImage* overmap, IplImage* edge, IplImage* region, int radius, float minRatio, int addtionalEdge = 0);
	void setPosition(float x, float y, int angle, int maxx, int maxy, CvScalar color);

	bool isIntersect(float fx, float fy, IplImage* overmap, IplImage* edge, IplImage* layer);	
	void setAccumulatedTile(IplImage* dst, IplImage* temp=NULL);


	void setAccumulatedMap(IplImage* dst, IplImage* temp=NULL);
	
	void edgeClipping(IplImage* edge, int deltaLength=0, IplImage* tile=NULL, bool isInit=false);
	
	// 타일 이동 시 마스크맵 재 설정
	void reSetMaps(IplImage* tileMap, IplImage* overlapMap, IplImage* positionMap, float overlapRatio, float regenRatio, IplImage* temp=NULL);
	void setMaskMaps(IplImage* tileMap, IplImage* overlapMap, IplImage* positionMap, float overlapRatio, float regenRatio, IplImage* temp=NULL);
	void SelectRender(IplImage* dst, MPoint Max, MPoint Min, bool isShadow=true, IplImage* temp=NULL, IplImage* temp2=NULL);
protected:
	//에지 클리핑 시 사용되는 함수들(명훈 선배 소스)
	bool isEdge(int x, int y, IplImage* edgeMap);
	int getDiagonalEdgeClippedLength(int val, double curLength, bool isInit, int deltaLength=0);
	int getEdgeClippedLength(int val, double curLength, bool isInit, int deltaLength=0);
	int getDiagonalTileClippedLength(int val, double curLength, bool isInit, int deltaLength=0);
	int getTileClippedLength(int val, double curLength, bool isInit, int deltaLength=0);

};

