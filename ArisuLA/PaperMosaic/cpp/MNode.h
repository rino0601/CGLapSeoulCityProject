#pragma once
#import <vector>
using namespace std;
class MNode
{
public:
	MNode(void);
	~MNode(void);
	vector< MNode* > child;
	void *	data;
	void Destroy();
};
