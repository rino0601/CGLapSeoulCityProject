//
//  Tile_Mosaic_CV.h
//  MyG
//
//  Created by 이 경록 on 12. 11. 29..
//  Copyright (c) 2012년 LGR. All rights reserved.
//

#ifndef __MyG__Tile_Mosaic_CV__
#define __MyG__Tile_Mosaic_CV__

#include <opencv2/opencv.hpp>
#include <string.h>
#include <stdio.h>
#include <iostream>

#endif /* defined(__MyG__Tile_Mosaic_CV__) */


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define PI 3.1415926535897931

struct point
{
    int x;
    int y;
    
    unsigned char R;
    unsigned char G;
    unsigned char B;
    
    double direction;
    int matchim_num;
    
    point* nextp;
    point* prevp;
};

struct pointhead
{
    int numofP;
    int rectsize;
    point* firstp;
};

struct edgehead
{
    int numofP;  //오타수정 nunofP -> numofP by kim
    point* firstp;
};

class Mosaic_CV
{
    // 생성입니다.
public:
    Mosaic_CV(); // 표준 생성자입니다.
    ~Mosaic_CV();
    
protected:
        
    
    // 구현입니다.
protected:
   
    
    // 생성된 메시지 맵 함수
    
    
    IplImage *image;
    IplImage *origin_imm;
    IplImage *line_imm;
    IplImage *temp;
    IplImage *result_imm;
    double** DirectionMap;
    
    const char *sqlDB;
    
    bool isfirstvoro;
    
    int isdone;
    
    int startx;
    int starty;
    
    int centerx;
    int centery;
    
    int viewW;
    int viewH;
    
    int numofdbi;
    
    float rectsize;
    
    int image_val;
    int dismode;
    double biyul;
    
    void addtile(int x ,int y);
    void Voronoi(cv::Mat img, cv::Subdiv2D sub);
    
    void CVD(cv::Mat img);
    void Line_avoid(cv::Mat img);
    pointhead* phead;
    pointhead* basehead;
    edgehead* ehead;
    
public:
    
    void setTargetImage(IplImage *img);
    void setDBFile(const char* sqlDB, int n);
    
    void goMosaic();
    pointhead* getPointList(int option);
    
    void returnResult(IplImage *res);

    void locate_point( cv::Mat& img,  cv::Subdiv2D& subdiv,  cv::Point2f fp,  cv::Scalar active_color );
    void draw_subdiv_point(  cv::Mat& img,  cv::Point2f fp,  cv::Scalar color );
    void draw_subdiv(  cv::Mat& img,  cv::Subdiv2D& subdiv,  cv::Scalar delaunay_color );
    void paint_voronoi( cv::Mat& img, cv::Subdiv2D& subdiv );
    void drawRect(cv::Mat res);
    void calcDirectionMap();
    double findDgree(double dx, double dy);

};