#pragma once

#define INF 10000000000

class xDistanceTransform
{
public:
	xDistanceTransform(void);
	~xDistanceTransform(void);

	int m_iWidth;
	int m_iHeight;

public:
	float *dt(float *f, int n);

	int	  IDX(int x, int y);
	
	void  dt(float *im, int width, int height);
	IplImage * GetDT(IplImage *im, Byte on = 1);
};
