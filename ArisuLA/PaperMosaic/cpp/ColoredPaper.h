#pragma once
#import <vector>
using namespace std;

#import "MPoint.h"	
#import "ColoredPapperTile.h"
class ActiveXDialog;

class ColoredPaper
{
public:
	ColoredPaper(void);
	~ColoredPaper(void);

public:
	IplImage* m_pIndex;
	IplImage* m_pVoronoi;
	IplImage* m_pVoronoib;
	IplImage* m_pColoredPaperImg;

	
	ColoredPapperTile* m_pTiles; // 색종이 조각
	ColoredPapperTile* m_pCPT;   // 색종이 영역       
	ColoredPapperTile* m_pTemporary; 
	ActiveXDialog* m_pDlg;

	int m_nTile;
	int m_nIndex;

	vector<MPoint> CutTile; //  p.x = TileNum, p.y = VertexNum

public:
	void InitPaper(int ntile = 81);
	void getVoronoi();
	void ReadVoronoiPixels();
	
	//버벡트 통합
	void CombineVert(int TileNum);
	void SearchNeibVert(int x, int y, Byte r, Byte g, Byte b, int vX, int vY, int tileNum);

	void Render(IplImage* dst);	
	ColoredPapperTile* GetACut(MPoint position);
	void Reset();
	void DeleteSelected();

	void setDlg(ActiveXDialog* dlg);

	// 찢어진 효과를 버텍스 Search
	void SearchCuttdEdge(int TileNum);
	void searchNeibVert(int x, int y, Byte r, Byte g, Byte b, int num, int TileNum);
};

