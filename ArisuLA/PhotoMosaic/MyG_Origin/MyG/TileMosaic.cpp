//
//  TileMosaic.cpp
//  MyG
//
//  Created by SeulBeom on 13. 7. 4..
//  Copyright (c) 2013년 LGR. All rights reserved.
//

#include "TileMosaic.h"

void TileMosaic::goMosaic(IplImage* res){
    /*double time = CACurrentMediaTime();
    printf("Start Mosaic : %f \n", (CACurrentMediaTime() - time));
    if(origin_imm ==NULL)
		return;
    
	int width = origin_imm->width;
	int height = origin_imm->height;
	
	rectsize =  (float)max(origin_imm->width, origin_imm->height)/15;
	int tile_size =  (int)rectsize;
	
    
    
    
	for(int i=0; i<height/tile_size; i++)   //타일 생성
	{
		for(int j=0; j<width/tile_size; j++)
		{
			int jitter1, jitter2;
			jitter1 = 5-rand()%10;
			jitter2 = 5-rand()%10;
			addtile( max(min(((tile_size/2)+tile_size*j)+jitter1,width),0) , max(min(((tile_size/2)+tile_size*i)+jitter2,height),0) );
		}
	}
    
    phead->rectsize = rectsize;
    
	cv::Rect cr;
	cr.width = origin_imm->width;
	cr.height = origin_imm->height;
	cr.x = 0;
	cr.y = 0;
    
    cv::Mat img(cr.size(),CV_8UC3);
    cv::Mat res(cr.size(),CV_8UC3);
    cv::Subdiv2D sub(cr);
    
	Voronoi(img,sub);
    printf("first get voronoi : %f \n", (CACurrentMediaTime() - time));
	//cvWaitKey(5);
    
	for(int i=0; i<2; i++)
	{
		CVD(img);  // cvd
        printf("%d's CVD : %f \n",i, (CACurrentMediaTime() - time));
	}
    
	//cvWaitKey(5);
    
	Line_avoid(img); //라인 피하기
    printf("Line avoiding success : %f \n", (CACurrentMediaTime() - time));
    
	calcDirectionMap(); //각도계산
    printf("Direction Calculation success : %f \n", (CACurrentMediaTime() - time));
    
    drawRect(res);
    printf("DrawRect success : %f \n", (CACurrentMediaTime() - time));
    
    result_imm = new IplImage(res);
	//imshow( "res", res );
	//cvWaitKey(0);*/
}
void TileMosaic::getEdges(IplImage* image){
/*
    lineImage = cvCreateImage(cvGetSize(targetImage),image->depth, image->nChannels);
	cvCanny(image, lineImage, 100, 150);  //라인구하기

    edges = new edgeList;
	edges->beforePtr = nullptr;
    edges->nextPtr = nullptr;
    
	point tp;
	point ttp;
	tp = ehead->firstp;
    
	for(int h=0;h<lineImage->height; h++)  //라인 저장 생성
	{
		for(int w=0; w<lineImage->width; w++)
		{
			if(lineImage->imageData[h*lineImage->widthStep+w]!=0)
			{
				lineImage->imageData[h*lineImage->widthStep+w] = (unsigned char)255;
				
				if(ehead->numofP==0)
				{
					ttp = new point;
					ttp->x = w;
					ttp->y = h;
					ttp->nextp = 0;
					ttp->prevp = 0;
					
					ehead->firstp = ttp;
                    tp = ttp;
					ehead->numofP++;
				}
                
				else
				{
                    for(; tp->nextp != nullptr ; tp = tp->nextp);
					ttp = new point;
					ttp->x = w;
					ttp->y = h;
					ttp->nextp = 0;
					ttp->prevp = 0;
					
					tp->nextp = ttp;
					ttp->prevp = tp;
					ehead->numofP++;
				}
			}
            
			else
			{
				lineImage->imageData[h*lineImage->widthStep+w] = 0;
			}
		}
	}
*/
}

void TileMosaic::setTargetImage(IplImage *img){
    if(targetImage != NULL)
    {
        cvReleaseImage(&targetImage);
    }
    
    targetImage = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
    cvCopy(img, targetImage);
    
    
    IplImage *image = cvCreateImage(cvGetSize(img), img->depth, 1);
    cvCvtColor(targetImage, image, CV_BGR2GRAY);
    getEdges(image);
}


void TileMosaic::setDBFilePath(const char* sqlDB){
    DBPath = sqlDB;
}
void TileMosaic::setDBFileNumbers(int n){
    DBNumbers = n;
}