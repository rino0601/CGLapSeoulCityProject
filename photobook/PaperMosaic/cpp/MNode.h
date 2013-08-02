#pragma once
#import <vector>

struct MFRACTAL
{
	double mid;
	double distance;
};

using namespace std;
class MNode
{
public:
	MNode(void);
	~MNode(void);
	vector< MNode* > child;
	MFRACTAL*	data;
	void Destroy();
};
