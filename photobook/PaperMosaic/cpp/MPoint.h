#pragma once
class MPoint
{
public:
	int x;
	int y;
	int a;
	MPoint(){x = y = a = 0;}
	MPoint(int x, int y){this->x = x; this->y = y; a = 0;}
	MPoint(double x, double y){this->x = x; this->y = y; a = 0;}
	MPoint(double x, double y, double a){this->x = x; this->y = y; this->a = a;}
	~MPoint(){}
	const MPoint operator-(const MPoint &point){return MPoint(x-point.x,y-point.y);}
};

