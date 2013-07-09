//
//  TileMosaic.h
//  MyG
//
//  Created by SeulBeom on 13. 7. 4..
//  Copyright (c) 2013년 LGR. All rights reserved.
//

#ifndef MyG_TileMosaic_h
#define MyG_TileMosaic_h

#include <opencv2/opencv.hpp>
#include <string.h>
#include <stdio.h>
#include <iostream>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define PI 3.1415926535897931

class point
{
    int key;

    int x;
    int y;
    
    unsigned char R;
    unsigned char G;
    unsigned char B;
    
    double direction;
    
public:
    
};

struct voronoiPoints {
    point* p;
    int length;
};

struct edgeList {
    point p;
    edgeList* beforePtr;
    edgeList* nextPtr;
};

class TileMosaic
{
public:
    TileMosaic(); // 표준 생성자입니다.
    ~TileMosaic();
    
protected:
    int DBNumbers;
    const char* DBPath;
    edgeList* edges;
    
    IplImage* lineImage;
    IplImage* targetImage;
public:
    
    void goMosaic(IplImage* res);
    
    void setTargetImage(IplImage *img);
    void setDBFilePath(const char* sqlDB);
    void setDBFileNumbers(int n);
    
    /*void locate_point( cv::Mat& img,  cv::Subdiv2D& subdiv,  cv::Point2f fp,  cv::Scalar active_color );
    void draw_subdiv_point(  cv::Mat& img,  cv::Point2f fp,  cv::Scalar color );
    void draw_subdiv(  cv::Mat& img,  cv::Subdiv2D& subdiv,  cv::Scalar delaunay_color );
    void paint_voronoi( cv::Mat& img, cv::Subdiv2D& subdiv );
    void drawRect(cv::Mat res);
    void calcDirectionMap();
    double findDgree(double dx, double dy);
    */
    
protected:

    void getEdges(IplImage* image);
};

#endif
