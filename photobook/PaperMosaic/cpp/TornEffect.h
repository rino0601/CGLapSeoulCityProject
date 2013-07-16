#pragma once
#import <vector>
using namespace std;
#import "MPoint.h"
#import "MNode.h"


struct MFRACTAL 
{
	double mid;
	double distance;
};

class TornEffect
{
public:
	TornEffect(void);
	~TornEffect(void);
	void Destroy();	
	MNode * m_pNodedata;

public:	
	 
	TornEffect* GetInvers();
	void InitEffect(float ratio=0.15, int depth=5);
	vector< MPoint > GetTornEffectPoint(MPoint a, MPoint b, int minDis, float ratio);
	bool hasEffect();

private:
	MNode* getInversNode(MNode* node);
	MNode* getFractal(float ratio, int depth, float length);
	vector< MPoint > getFractalPoint(MPoint a, MPoint b, MNode* info, int minDis, float ratio);
};

