#pragma once
#import "ColoredPaper.h"
#import "ColoredPaperMosaic.h"
#import "ColoredPapperTile.h"
class ActiveXDialog;

class InteractiveColoredPaperMosaic
{
public:
	InteractiveColoredPaperMosaic(void);
	~InteractiveColoredPaperMosaic(void);

public:
	ColoredPaper m_oPaper;
	ColoredPaperMosaic* m_oCPM;
	ColoredPapperTile* m_opSelectedTile;
	ActiveXDialog* m_pDlg;
	
	MPoint dif_Posi;

public:
	void SetColoredPaper();
	void CutAPaper(MPoint Position);
	bool HasSelectedTile();
	void AttachPaper(MPoint position);
	void DrawSelectedTileAt(CGContextRef ctx, MPoint position);
	void DrawAutoTileAt(CGContextRef ctx, MPoint position);
	void SetMovingTile(MPoint position);
	void RotateTileAt(MPoint position, int deltaAngle);
	void ResizeTileAt(MPoint position, float lengthRatio);
	void AutoGeneration(ActiveXDialog *flagOwner);
	void setDlg(ActiveXDialog* dlg);
	void MoveTile();
};

