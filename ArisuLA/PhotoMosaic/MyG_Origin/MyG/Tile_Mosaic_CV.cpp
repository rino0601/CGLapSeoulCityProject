//
//  Tile_Mosaic_CV.cpp
//  MyG
//
//  Created by 이 경록 on 12. 11. 29..
//  Copyright (c) 2012년 LGR. All rights reserved.
//

#import <QuartzCore/CAAnimation.h>
#include "Tile_Mosaic_CV.h"
#import "sqlite3.h"
// Semantic Isuue 발생해서 집어넣습니다. - 고한종
using namespace std;

Mosaic_CV::Mosaic_CV() //초기화
{
    image = NULL;
    origin_imm = NULL;
	line_imm = NULL;
	temp = NULL;
	phead =NULL;
    basehead = NULL;
	DirectionMap = NULL;
	isfirstvoro = true;
	ehead = NULL;
	
	image_val = 0;
	dismode = 0;
    isdone = 0;
}
//이미지 받아오기 
void Mosaic_CV::setTargetImage(IplImage *img)
{
    if(origin_imm != NULL)
    {
        cvReleaseImage(&origin_imm);
    }
    
    if(image != NULL)
    {
        cvReleaseImage(&image);
    }
    
    
    
    origin_imm = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
    image = cvCreateImage(cvGetSize(img), img->depth, 1);
    
    cvCopy(img, origin_imm);
    cvCvtColor(origin_imm, image, CV_BGR2GRAY);
    
    line_imm = cvCreateImage(cvGetSize(origin_imm),image->depth, image->nChannels);
    
	cvCanny(image,line_imm,100,150);  //라인구하기
    
    ehead = new edgehead;
	ehead->firstp = 0;
	ehead->numofP = 0;
    
	point* tp;
	point* ttp;
	tp = ehead->firstp;
    
	for(int h=0;h<line_imm->height; h++)  //라인 저장 생성
	{
		for(int w=0; w<line_imm->width; w++)
		{
			if(line_imm->imageData[h*line_imm->widthStep+w]!=0)
			{
				line_imm->imageData[h*line_imm->widthStep+w] = (unsigned char)255;
				
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
					//tp = ehead->firstp; // 매번 초기화할 필요가 없는 코드 인 것 같습니다. 코드상 tp는 첫 포문을 제외하고 항상 뒤에서 두번째 노드를 참조합니다. by Kim
					/*while(1)
					{
						if(tp->nextp==NULL)
							break;
                        
						else
							tp = tp->nextp;
					}*/
                    
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
				line_imm->imageData[h*line_imm->widthStep+w] = 0;
			}
		}
	}
	
}

void Mosaic_CV::addtile(int x, int y) //시스포인트 리스트 추가
{
	if(phead==NULL)
	{
		phead = new pointhead;
		phead->numofP = 0;
		phead->firstp = NULL;
	}
	
	point* tp;
	point* tt;
    
	tp = new point;
    
	tp->x = x;
	tp->y = y;
    tp->direction = 999.0;
	tp->nextp = NULL;
	tp->prevp = NULL;
    
	bool nosame = false;
	tt = phead->firstp;
    
	tp->R = 0;
	tp->G = 0;
	tp->B = 0;
    
	int R;
	int G;
	int B;
    
	R = rand()%255;
	G = rand()%255;
	B = rand()%255;
    
	while(!nosame && tt!=NULL)
	{
		int nosamecount = 0;
        
		R = rand()%255;
		G = rand()%255;
		B = rand()%255;
        
		for(int i=0; i<phead->numofP; i++)
		{
			if(tt->R == R && tt->G == G && tt->B == B)
			{
				break;
			}
            
			tt = tt->nextp;
			nosamecount++;
		}
        
		if(nosamecount == phead->numofP)
		{
			nosame = true;
			break;
		}
        
		tt = phead->firstp;
	}
	tp->R = R;
	tp->G = G;
	tp->B = B;
    
	phead->numofP++;
    
    
	if(phead->firstp == NULL)
	{
		phead->firstp = tp;
		
	}
    
	else
	{
		point* tp2;
		tp2 = phead->firstp;
		if(tp2!=NULL)
		{
			while(tp2->nextp!=NULL)
			{
				tp2 = tp2->nextp;
			}
			tp2->nextp = tp;
			tp->prevp = tp2;
			
		}
        
		else
		{
			if(phead->firstp == NULL)
			{
				phead->firstp = tp;
				phead->numofP++;
			}
		}
	}
}

void Mosaic_CV::locate_point(  cv::Mat& img,  cv::Subdiv2D& subdiv,  cv::Point2f fp,  cv::Scalar active_color ) //보로노이 포인트 위치시키기
{
	int e0=0, vertex=0;
    
	subdiv.locate(fp, e0, vertex);
    
	if( e0 > 0 )
	{
		int e = e0;
		do
		{
            cv::Point2f org, dst;
			if( subdiv.edgeOrg(e, &org) > 0 && subdiv.edgeDst(e, &dst) > 0 )
				line( img, org, dst, active_color, 3, CV_AA, 0 );
            
			e = subdiv.getEdge(e,  cv::Subdiv2D::NEXT_AROUND_LEFT);
		}
		while( e != e0 );
	}
    
}
void Mosaic_CV::Voronoi(cv::Mat img) //보로노이 만들기
{
	/*cv::Rect cr;
	cr.x = 0;
	cr.y = 0;
	cr.width = origin_imm->width;
	cr.height = origin_imm->height;
    */
    
	cv::Scalar active_facet_color(0,0,255), delaunay_color(255,255,255);
    
	//string win = "voronoi";
    
	point* pp;
	pp = phead->firstp;
	for(int i=0; i<phead->numofP; i++)
	{
		cv::Point2f fp;
		fp.x = (float)pp->x;
		fp.y = (float)pp->y;
		
		locate_point( img, sub, fp, active_facet_color );
        
		sub.insert(fp);
        
		img =  cv::Scalar::all(0);
        
		pp = pp->nextp;
        
	}
    
    img =  cv::Scalar::all(0);
    
    
    
}

void Mosaic_CV::CVD(cv::Mat img)
{// maybe Lloyd's algorithm  note.kim
	IplImage *imm = new IplImage(img);
	
	
	cv::Rect cr;
	cr.width = origin_imm->width;
	cr.height = origin_imm->height;
	cr.x = 0;
	cr.y = 0;
    
    unsigned long* sumOfX = new unsigned long[phead->numofP];
    unsigned long* sumOfY = new unsigned long[phead->numofP];
    unsigned long* numbers = new unsigned long[phead->numofP];
    
    for (int i = 0; i < phead->numofP ; i++) {
        sumOfX[i] = 0;
        sumOfY[i] = 0;
        numbers[i] = 0;
    }
    
    for(int h = 0; h < origin_imm->height; h++){
        for(int w = 0; w < origin_imm->width; w++){
            //if not black do sum
            cv::Point2f pt;
            
            pt.x = (float)w;
            pt.y = (float)h;
            
            unsigned char curR= (unsigned char)imm->imageData[h*imm->widthStep+w*3+0];
            unsigned char curG= (unsigned char)imm->imageData[h*imm->widthStep+w*3+1];
            unsigned char curB= (unsigned char)imm->imageData[h*imm->widthStep+w*3+2];
             
            if(curR == 0 && curG == 0 && curB == 0) {
                int k = sub.findNearest(pt) - 4; // initial outer vertex
                
                if(k < 0) {
                    NSLog(@"??");
                
                } else if( k < phead->numofP ) {
                
                    sumOfX[k] += pt.x;
                    sumOfY[k] += pt.y;
                    numbers[k]++;
                } else {
                    NSLog(@"fuck... what is problem? %d / %d",k,phead->numofP);
                }
            }
        }
    }
    
    
    point *tp;
	tp = phead->firstp;
    
    
    for(int i = 0; i < phead->numofP; i++) {
        
        if(numbers[i] != (unsigned) 0){

            tp->x = ((float) sumOfX[i] / numbers[i]);
            tp->y = ((float) sumOfY[i] / numbers[i]);
    
            if (tp->x < 0 || tp->y < 0 ||
                tp->x >= origin_imm->width ||
                tp->y >= origin_imm->height ) { // error check
            
                NSLog(@"%ld, %ld, %ld", sumOfX[i], sumOfY[i], numbers[i]);
                printf("%d: tp->x %d, tp->y %d, origin_imm->width %d, origin_imm->height %d\n",
                       i,tp->x,tp->y,origin_imm->width,origin_imm->height);
        
            }
        }
        tp = tp->nextp;
    }
    
    
    
    
/*	for(int i=0; i<phead->numofP; i++)
	{
		unsigned char R = tp->R;
		unsigned char G = tp->G;
		unsigned char B = tp->B;
        
		unsigned long sumx =0;
		unsigned long sumy =0;
		unsigned int sumnum =0;
		for(int h = 0; h< origin_imm->height; h++)
		{
			for(int w = 0; w< origin_imm->width; w++)
			{
				unsigned char curR= (unsigned char)imm->imageData[h*imm->widthStep+w*3+0];
				unsigned char curG= (unsigned char)imm->imageData[h*imm->widthStep+w*3+1];
				unsigned char curB= (unsigned char)imm->imageData[h*imm->widthStep+w*3+2];
                
                
				if(curR ==R && curG==G && curB==B)
				{
					sumx += w;
					sumy += h;
					sumnum += 1;
				}
			}
		}
        
		if(sumnum!=0)
		{
			tp->x = ((double)sumx / (double)sumnum)+0.7;
			tp->y = ((double)sumy / (double)sumnum)+0.7;
		}
		
        
		tp = tp->nextp;
	}*/
    
    
    sub.initDelaunay(cr);
    
	Voronoi(img);
}

void Mosaic_CV::goMosaic() //모자이크 만들기.
{
    double time = CACurrentMediaTime();
    NSLog(@"Start Mosaic");
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
    sub.initDelaunay(cr);

	Voronoi(img);
    NSLog(@"first get voronoi complete");
	//cvWaitKey(5);
    
	for(int i=0; i<2; i++)
	{
		CVD(img);  // cvd
        NSLog(@"%d's CVD complete",i);
	}
    
	//cvWaitKey(5);
    
	Line_avoid(img); //라인 피하기
    printf("Line avoiding success : %f \n", (CACurrentMediaTime() - time));
    
    setBasePoints();
    
	calcDirectionMap(); //각도계산
    printf("Direction Calculation success : %f \n", (CACurrentMediaTime() - time));
    
    drawRect(res);
    printf("DrawRect success : %f \n", (CACurrentMediaTime() - time));
    
    result_imm = new IplImage(res);
    
}

void Mosaic_CV::setDBFile(const char *sqlDB, int n) //db 파일 이름 받아오기
{
    this->sqlDB = sqlDB;
    
    numofdbi = n;
}

void mininsert(double* distances, int* candidates, double distance, int candidate, int num)
{
    for(int i=0; i<num ; i++)
    {
        if(distances[i]>distance)
        {
            int dis = distances[i];
            int can = candidates[i];
            
            distances[i] = distance;
            candidates[i] = candidate;
            
            mininsert(distances, candidates, dis, can, num);
            
            break;
        }
    }
}

bool isinhere(int* candidates, int candidate, int num)
{
    for(int i=0; i<num; i++)
    {
        if(candidates[i]==candidate)
            return true;
    }
    
    return false;
}

void Mosaic_CV::Line_avoid(cv::Mat img)
{
	IplImage *imm = new IplImage(img);
	//imm = &IplImage(img);
    
	int edge_avoid_iternum =63;
    
	IplImage *store_imm = cvCreateImage(cvGetSize(imm),8,3);
	cvCopy(imm,store_imm,0);
    
	IplImage *store_line = cvCreateImage(cvGetSize(imm),8,1);
	cvCopy(line_imm,store_line,0);
    
	IplImage *dilated = cvCreateImage(cvGetSize(imm),8,1);
    
	
	//edge너비를 늘려가면서 cvd를 반복 수행
    
	for(int ei=0; ei<edge_avoid_iternum; ei++) {
		
		for(int h=0;h<line_imm->height; h++) {
			for(int w=0; w<line_imm->width; w++) {
				if(dilated->imageData[h*line_imm->widthStep+w]!=0) {
                    
					imm->imageData[h*imm->widthStep+w*3+0] = (unsigned char)dilated->imageData[h*line_imm->widthStep+w];
                    imm->imageData[h*imm->widthStep+w*3+1] = (unsigned char)dilated->imageData[h*line_imm->widthStep+w];
					imm->imageData[h*imm->widthStep+w*3+2] = (unsigned char)dilated->imageData[h*line_imm->widthStep+w];
				
                }
			}
		}
        
        
        if(ei%6==0 && ei<40) {
            
			cvDilate(store_line,dilated,0,1);
			cvCopy(dilated,store_line);
            
        } else if (ei%6==0 && ei>40) {
            
            cvErode(store_line,dilated,0,1);
			cvCopy(dilated,store_line);
            
        }
        NSLog(@"%d: CVD",ei);
        cv::Mat mm(imm);
		CVD(mm);
		cvCopy(imm,store_imm);
        
	}
    
    
	//cvReleaseImage(&imm);
	cvReleaseImage(&store_imm);
	cvReleaseImage(&store_line);
}

void Mosaic_CV::calcDirectionMap()
{
	//가장 가까운 엣지와 거리를 구하기 위해 일정 범위 까지만 엣지 검색
    
    
    point* tp;
    tp = phead->firstp;
    for(int pp=0; pp<phead->numofP; pp++)
    {
        double dis = 30000;
        int selx =tp->x;
        int sely =tp->y;
        
        int startx = 0;
        int starty = 0;
        
        int endx = origin_imm->width;
        int endy = origin_imm->height;
        
        point* ttp = ehead->firstp;
        for(int ee=0; ee<ehead->numofP; ee++)
        {
            if( (startx <= ttp->x)&&(endx >= ttp->x)&&(starty <=ttp->y)&&(endy>=ttp->y))
            {
                double tdis = sqrt(pow((double)ttp->x-(double)tp->x,2.0)+pow((double)ttp->y-(double)tp->y,2.0));
                if(dis > tdis)
                {
                    selx = ttp->x;
                    sely = ttp->y;
                    dis = tdis;
                }
            }
            
            ttp = ttp->nextp;
        }
        
        if(selx != tp->x || sely !=tp->y)
        {
            double vec1[2];
            double vec2[2];
            
            vec1[0] = (selx-tp->x)*100;;
            vec1[1] = 0;
            
            vec2[0] = (selx-tp->x)*100;
            vec2[1] = (sely-tp->y)*100;
            
            double cc = ( sqrt( pow(vec1[0],2) + pow(vec1[1],2) ) * sqrt( pow(vec2[0],2) + pow(vec2[1],2) ) );
            
            double coco = (vec1[0] * vec1[1] + vec2[0] * vec2[1]) / cc;
            
            if(cc==0)
                tp->direction = 0;
            else
                tp->direction = ( (acos(coco)* 180) / PI);
            
        }
        
        else
        {
            tp->direction = 0;
        }
        
        tp = tp->nextp;
    }
    
    
    tp = basehead->firstp;
    for(int pp=0; pp<basehead->numofP; pp++)
    {
        //검사 범위
        int startx =  max((double)tp->x-(1*(double)rectsize),0);
        int starty =  max((double)tp->y-(1*(double)rectsize),0);
        
        int endx =  min((double)tp->x+(1*(double)rectsize),origin_imm->width);
        int endy =  min((double)tp->y+(1*(double)rectsize),origin_imm->height);
        
        startx = 0;
        starty = 0;
        
        endx = origin_imm->width;
        endy = origin_imm->height;
        
        
        double dis = 30000;
        int selx =tp->x;
        int sely =tp->y;
        
        point* ttp = ehead->firstp;
        for(int ee=0; ee<ehead->numofP; ee++)
        {
            if( (startx <= ttp->x)&&(endx >= ttp->x)&&(starty <=ttp->y)&&(endy>=ttp->y))
            {
                double tdis = sqrt(pow((double)ttp->x-tp->x,2)+pow((double)ttp->y-tp->y,2));
                if(dis > tdis)
                {
                    selx = ttp->x;
                    sely = ttp->y;
                    dis = tdis;
                }
            }
            
            ttp = ttp->nextp;
        }
        
        if(selx != tp->x || sely !=tp->y)
        {
            double vec1[2];
            double vec2[2];
            
            vec1[0] = selx-tp->x;
            vec1[1] = 0;
            
            vec2[0] = selx-tp->x;
            vec2[1] = sely-tp->y;
            
            double cc = ( sqrt( pow(vec1[0],2) + pow(vec1[1],2) ) * sqrt( pow(vec2[0],2) + pow(vec2[1],2) ) );

            
            double coco = (vec1[0] * vec1[1] + vec2[0] * vec2[1]) / cc;
            
            
            if(cc==0)
                tp->direction = 0;
            else
                tp->direction = ( (acos(coco)* 180) / PI);
            
            //tp->direction = (acos(coco)* 180 / PI);
            
        }
        
        else
        {
            tp->direction = 0;
        }
        
        tp = tp->nextp;
    }
}
void Mosaic_CV::setBasePoints() {
    cv::vector< cv::Vec6f> triangleList;
	sub.getTriangleList(triangleList);
    cv::vector< cv::Point> pt(3);
    
    if(basehead!=NULL)
    {
        point *tp = basehead->firstp;
        
        while(tp!=NULL)
        {
            point* ttp = tp->nextp;
            free(tp);
            tp = ttp;
        }
        
        free(basehead);
        basehead = NULL;
    }
    
    if(basehead==NULL)
    {
        basehead = new pointhead;
        basehead->rectsize = rectsize;
        basehead->numofP = 0;
        basehead->firstp = 0;
        
        point* currentPoint = (basehead->firstp);
        
        for( size_t i = 0; i < triangleList.size(); i++ )
        {
            cv::Vec6f t = triangleList[i];
            pt[0] =  cv::Point(cvRound(t[0]), cvRound(t[1]));
            pt[1] =  cv::Point(cvRound(t[2]), cvRound(t[3]));
            pt[2] =  cv::Point(cvRound(t[4]), cvRound(t[5]));
            
            double x = (pt[0].x + pt[1].x+ pt[2].x)/3.0;
            double y = (pt[0].y + pt[1].y+ pt[2].y)/3.0;
            
            if(x<0 || x>origin_imm->width)
                continue;
            
            if(y<0 || y>origin_imm->height)
                continue;
            
            point *tp = new point;
            tp->x = x;
            tp->y = y;
            tp->nextp = NULL;
            
            if( currentPoint == NULL ){
                basehead->firstp = tp;
                currentPoint = tp;
            } else {
                tp->prevp = currentPoint;
                currentPoint->nextp = tp;
                currentPoint = tp;
            }
            
            basehead->numofP++;
        }
        
    }
}
void Mosaic_CV::drawRect(cv::Mat res) //사각형 그리기 겸... 이미지 찾기 used opj-c in this code
{   
    
    
    sqlite3 *contactDB;
    
    sqlite3_open(sqlDB, &contactDB);
    
    NSMutableArray *tByTImages = [[NSMutableArray alloc] init];
    
    const char* querySQL = "SELECT ID, TBYTIMAGE FROM IMAGEDATA";
    
    sqlite3_stmt *statement ;
    
    sqlite3_prepare_v2(contactDB, querySQL, -1, &statement, NULL);
    
    while( sqlite3_step (statement) == SQLITE_ROW ) {
        NSUInteger blobLength = sqlite3_column_bytes(statement, 1);
        NSData* ipl = [[NSData alloc] initWithBytes:sqlite3_column_blob(statement, 1) length:blobLength];
        [tByTImages addObject:ipl];
    }
    
    sqlite3_finalize(statement);
    
	point* pt;
    res =  cv::Scalar::all(0);
    
	pt = phead->firstp;
    
    cv::Mat orimat = origin_imm;
    
    point* pt2;
    pt2 = basehead->firstp;
    
    
    
    while(pt2!=NULL)
	{
		cv::Point2f ctt(pt2->x,pt2->y);
		cv::Size2f cs(rectsize-(rectsize/5.0),rectsize-(rectsize/5.0));
        
		int R = (unsigned char)origin_imm->imageData[pt2->y*origin_imm->widthStep + pt2->x*3 + 0];
		int G = (unsigned char)origin_imm->imageData[pt2->y*origin_imm->widthStep + pt2->x*3 + 1];
		int B = (unsigned char)origin_imm->imageData[pt2->y*origin_imm->widthStep + pt2->x*3 + 2];
        ///////////////////////
        
        
		double angle = pt->direction;
        
        if(isnan(angle))
            angle = 0.0;
        
        if(angle>180)
            angle -= 180;
        
        pt->direction = angle;
        
		//angle = angle;
		//float rad = angle*(PI/180.0f);
		cv::RotatedRect cvR(ctt,cs,angle);
        
        //Rotated rect 추출.
        cv::Size rect_size = cvR.size;
        
        cv::Mat M, rotated, cropped;
        
        // get the rotation matrix
        M = cv::getRotationMatrix2D(cvR.center, angle, 1.0);
        // perform the affine transformation
        cv::warpAffine(orimat, rotated, M, orimat.size(), cv::INTER_CUBIC, cv::BORDER_TRANSPARENT);
        // crop the resulting image
        cv::getRectSubPix(rotated, rect_size, cvR.center, cropped);
        
        IplImage *sample = new IplImage(cropped);
        IplImage *twosample = cvCreateImage(cvSize(2,2), sample->depth, sample->nChannels);
        IplImage *foursample = cvCreateImage(cvSize(4,4), sample->depth, sample->nChannels);
        IplImage *sixteensample = cvCreateImage(cvSize(16,16), sample->depth, sample->nChannels);
        
        cvResize(sample, twosample);
        cvResize(sample, foursample);
        cvResize(sample, sixteensample);
		
        //매치를 찾는다.
        int candidates1[10];
        double distance[10] = {30000,30000,30000,30000,30000,30000,30000,30000,30000,30000};
        
        int candidates2[5];
        double distance2[5] = {30000,30000,30000,30000,30000};
        
        int selectedone;
        double seldis;
        
        //2개짜리 후보군 5개///////////////////////////////////////////////////////////////////////////////////////////////
        for (NSData *a in tByTImages) {
            
            double disT = 0;
            
            for(int h=0; h<twosample->height; h++)
            {
                for(int w=0; w<twosample->width; w++)
                {
                    int orib = (unsigned char)twosample->imageData[h*twosample->widthStep+w*3+0];
                    int orig = (unsigned char)twosample->imageData[h*twosample->widthStep+w*3+1];
                    int orir = (unsigned char)twosample->imageData[h*twosample->widthStep+w*3+2];
                    
                    int dbb = ((const unsigned char*)[a bytes])[h*6 + w*3 + 0];
                    int dbg = ((const unsigned char*)[a bytes])[h*6 + w*3 + 1];
                    int dbr = ((const unsigned char*)[a bytes])[h*6 + w*3 + 2];
                    
                    int disB = abs(orib-dbb);
                    int disG = abs(orig-dbg);
                    int disR = abs(orir-dbr);
                    
                    disT += (double)(disB+disG+disR)/3.0;
                }
            }
            
            disT = disT/(double)(twosample->height * twosample->width);
            if( disT < distance[9] )
                mininsert(distance, candidates1, disT, [tByTImages indexOfObject:a], 10);
            
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //4개짜리 후보군 3개
        
        for (int i = 0; i < 5; i++) {
            double disT = 0;
            
            const char* asdf = "SELECT ID, FBYFIMAGE FROM IMAGEDATA WHERE ID = @A";
            
            sqlite3_prepare_v2(contactDB, asdf, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, candidates1[i]);
            
            while( sqlite3_step (statement) == SQLITE_ROW ) {
                NSUInteger blobLength = sqlite3_column_bytes(statement, 1);
                NSData* ipl = [[NSData alloc] initWithBytes:sqlite3_column_blob(statement, 1) length:blobLength];
                
                for(int h=0; h<foursample->height; h++)
                {
                    
                    for(int w=0; w<foursample->width; w++)
                    {
                        int orib = (unsigned char)foursample->imageData[h*foursample->widthStep+w*3+0];
                        int orig = (unsigned char)foursample->imageData[h*foursample->widthStep+w*3+1];
                        int orir = (unsigned char)foursample->imageData[h*foursample->widthStep+w*3+2];
                        
                        
                        int dbb = ((const unsigned char*)[ipl bytes])[h*4*3 + w*3 + 0];
                        int dbg = ((const unsigned char*)[ipl bytes])[h*4*3 + w*3 + 1];
                        int dbr = ((const unsigned char*)[ipl bytes])[h*4*3 + w*3 + 2];
                        
                        int disB = abs(orib-dbb);
                        int disG = abs(orig-dbg);
                        int disR = abs(orir-dbr);
                        disT += (double)(disB+disG+disR)/3.0;
                    }
                    
                }
                
                disT = disT/(double)(foursample->height*foursample->width);
                
                mininsert(distance2, candidates2, disT, candidates1[i], 5);
                
            }
            
        }
        sqlite3_finalize(statement);
        
        //////////////////////////////////////////////////////////////////////////////////////////////////
        //4개짜리 후보군 중에 16개짜리 최종 후보군 뽑기
        
        selectedone = candidates2[0];
        seldis = distance2[0];
        
        
        for(int i = 0 ; i < 5 ; i ++ ){
            
            const char* asdf = "SELECT ID, SBYSIMAGE FROM IMAGEDATA WHERE ID = @A";
            
            sqlite3_prepare_v2(contactDB, asdf, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, candidates2[0]);
            
            
            while( sqlite3_step (statement) == SQLITE_ROW ) {
                double disT = 0;
                NSUInteger blobLength = sqlite3_column_bytes(statement, 1);
                NSData* ipl = [[NSData alloc] initWithBytes:sqlite3_column_blob(statement, 1) length:blobLength];
                
                for(int h=0; h<sixteensample->height; h++)
                {
                    for(int w=0; w<sixteensample->width; w++)
                    {
                        int orib = (unsigned char)sixteensample->imageData[h*sixteensample->widthStep+w*3+0];
                        int orig = (unsigned char)sixteensample->imageData[h*sixteensample->widthStep+w*3+1];
                        int orir = (unsigned char)sixteensample->imageData[h*sixteensample->widthStep+w*3+2];
                        
                        int dbb = ((const unsigned char*)[ipl bytes])[h*16*3 + w*3 + 0];
                        int dbg = ((const unsigned char*)[ipl bytes])[h*16*3 + w*3 + 1];
                        int dbr = ((const unsigned char*)[ipl bytes])[h*16*3 + w*3 + 2];
                        
                        int disB = abs(orib-dbb);
                        int disG = abs(orig-dbg);
                        int disR = abs(orir-dbr);
                        disT += (double)(disB+disG+disR)/3.0;
                    }
                }
                
                disT = disT/(double)(sixteensample->height*sixteensample->width);
                
                if(disT < seldis)
                {
                    seldis = disT;
                    selectedone = sqlite3_column_int(statement, 0);
                }
            }
            
            sqlite3_finalize(statement);
        }
        
        
        
        //////////////////////////////////////////////////////////////////////////////////////
        
		cv::Point2f cp[4];
		cvR.points(cp);
		
		cv::Point cpp[4];
		for(int h=0;h<4;h++)
		{
			cpp[h].x = cp[h].x;
			cpp[h].y = cp[h].y;
            
            cpp[h].x = MIN(MAX(cpp[h].x,0.f),origin_imm->width);
            cpp[h].y = MIN(MAX(cpp[h].y,0.f),origin_imm->height);
		}
		
        cv::fillConvexPoly(res,cpp,4,cv::Scalar(B,G,R));
        isdone = 0;
        
        pt2->matchim_num = selectedone;
        
		pt2 = pt2->nextp;
        
        cvReleaseImage(&twosample);
        cvReleaseImage(&foursample);
        cvReleaseImage(&sixteensample);
	}
    
    while(pt!=NULL)
	{
        
		cv::Point2f ctt(pt->x,pt->y);
		cv::Size2f cs(rectsize-(rectsize/5.0),rectsize-(rectsize/5.0));
        
		int R = (unsigned char)origin_imm->imageData[pt->y*origin_imm->widthStep+pt->x*3+0];
		int G = (unsigned char)origin_imm->imageData[pt->y*origin_imm->widthStep+pt->x*3+1];
		int B = (unsigned char)origin_imm->imageData[pt->y*origin_imm->widthStep+pt->x*3+2];
        ///////////////////////
        
        
		double angle = pt->direction;
        
        if(isnan(angle))
            angle = 0.0;
        
        if(angle>180)
            angle -= 180;
        
        pt->direction = angle;
        
		//angle = angle;
		//float rad = angle*(PI/180.0f);
		cv::RotatedRect cvR(ctt,cs,angle);
        
        //Rotated rect 추출.
        cv::Size rect_size = cvR.size;
        
        cv::Mat M, rotated, cropped;
        
        // get the rotation matrix
        M = cv::getRotationMatrix2D(cvR.center, angle, 1.0);
        // perform the affine transformation
        cv::warpAffine(orimat, rotated, M, orimat.size(), cv::INTER_CUBIC, cv::BORDER_TRANSPARENT);
        // crop the resulting image
        cv::getRectSubPix(rotated, rect_size, cvR.center, cropped);
        
        IplImage *sample = new IplImage(cropped);
        IplImage *twosample = cvCreateImage(cvSize(2,2), sample->depth, sample->nChannels);
        IplImage *foursample = cvCreateImage(cvSize(4,4), sample->depth, sample->nChannels);
        IplImage *sixteensample = cvCreateImage(cvSize(16,16), sample->depth, sample->nChannels);
        
        cvResize(sample, twosample);
        cvResize(sample, foursample);
        cvResize(sample, sixteensample);
		
        //매치를 찾는다.
        
        int candidates1[5];
        double distance[5] = {30000,30000,30000,30000,30000};
        
        int candidates2[3];
        double distance2[3] = {30000,30000,30000};
        
        int selectedone;
        double seldis;
        
        //2개짜리 후보군 5개///////////////////////////////////////////////////////////////////////////////////////////////
        for (NSData *a in tByTImages) {
            
            double disT = 0;
            
            for(int h=0; h<twosample->height; h++)
            {
                for(int w=0; w<twosample->width; w++)
                {
                    int orib = (unsigned char)twosample->imageData[h*twosample->widthStep+w*3+0];
                    int orig = (unsigned char)twosample->imageData[h*twosample->widthStep+w*3+1];
                    int orir = (unsigned char)twosample->imageData[h*twosample->widthStep+w*3+2];
                    
                    int dbb = ((const unsigned char*)[a bytes])[h*6 + w*3 + 0];
                    int dbg = ((const unsigned char*)[a bytes])[h*6 + w*3 + 1];
                    int dbr = ((const unsigned char*)[a bytes])[h*6 + w*3 + 2];
                    
                    int disB = abs(orib-dbb);
                    int disG = abs(orig-dbg);
                    int disR = abs(orir-dbr);
                    
                    disT += (double)(disB+disG+disR)/3.0;
                }
            }
            
            disT = disT/(double)(twosample->height * twosample->width);
            if( disT < distance[4] )
                mininsert(distance, candidates1, disT, [tByTImages indexOfObject:a], 5);
            
        }
        
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //4개짜리 후보군 3개
        
        
        for (int i = 0; i < 5; i++) {
            double disT = 0;
            
            const char* asdf = "SELECT ID, FBYFIMAGE FROM IMAGEDATA WHERE ID = @A";
            
            sqlite3_prepare_v2(contactDB, asdf, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, candidates1[i]);
            
            while( sqlite3_step (statement) == SQLITE_ROW ) {
                NSUInteger blobLength = sqlite3_column_bytes(statement, 1);
                NSData* ipl = [[NSData alloc] initWithBytes:sqlite3_column_blob(statement, 1) length:blobLength];
                
                for(int h=0; h<foursample->height; h++)
                {
                    
                    for(int w=0; w<foursample->width; w++)
                    {
                        int orib = (unsigned char)foursample->imageData[h*foursample->widthStep+w*3+0];
                        int orig = (unsigned char)foursample->imageData[h*foursample->widthStep+w*3+1];
                        int orir = (unsigned char)foursample->imageData[h*foursample->widthStep+w*3+2];
                        
                        
                        int dbb = ((const unsigned char*)[ipl bytes])[h*4*3 + w*3 + 0];
                        int dbg = ((const unsigned char*)[ipl bytes])[h*4*3 + w*3 + 1];
                        int dbr = ((const unsigned char*)[ipl bytes])[h*4*3 + w*3 + 2];
                        
                        int disB = abs(orib-dbb);
                        int disG = abs(orig-dbg);
                        int disR = abs(orir-dbr);
                        disT += (double)(disB+disG+disR)/3.0;
                    }
                    
                }
                
                disT = disT/(double)(foursample->height*foursample->width);
                
                mininsert(distance2, candidates2, disT, candidates1[i], 3);
                
            }
            
        }
        sqlite3_finalize(statement);
        
        //////////////////////////////////////////////////////////////////////////////////////////////////
        //16개짜리 후보군 하나
        
        selectedone = candidates2[0];
        seldis = distance2[0];
        
        
        for(int i = 0 ; i < 3 ; i ++ ){
            
            const char* asdf = "SELECT ID, SBYSIMAGE FROM IMAGEDATA WHERE ID = @A";
            
            sqlite3_prepare_v2(contactDB, asdf, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, candidates2[0]);
            
            
            while( sqlite3_step (statement) == SQLITE_ROW ) {
                double disT = 0;
                NSUInteger blobLength = sqlite3_column_bytes(statement, 1);
                NSData* ipl = [[NSData alloc] initWithBytes:sqlite3_column_blob(statement, 1) length:blobLength];
                
                for(int h=0; h<sixteensample->height; h++)
                {
                    for(int w=0; w<sixteensample->width; w++)
                    {
                        int orib = (unsigned char)sixteensample->imageData[h*sixteensample->widthStep+w*3+0];
                        int orig = (unsigned char)sixteensample->imageData[h*sixteensample->widthStep+w*3+1];
                        int orir = (unsigned char)sixteensample->imageData[h*sixteensample->widthStep+w*3+2];
                        
                        int dbb = ((const unsigned char*)[ipl bytes])[h*16*3 + w*3 + 0];
                        int dbg = ((const unsigned char*)[ipl bytes])[h*16*3 + w*3 + 1];
                        int dbr = ((const unsigned char*)[ipl bytes])[h*16*3 + w*3 + 2];
                        
                        int disB = abs(orib-dbb);
                        int disG = abs(orig-dbg);
                        int disR = abs(orir-dbr);
                        disT += (double)(disB+disG+disR)/3.0;
                    }
                }
                
                disT = disT/(double)(sixteensample->height*sixteensample->width);
                
                if(disT < seldis)
                {
                    seldis = disT;
                    selectedone = sqlite3_column_int(statement, 0);
                }
            }
            
            sqlite3_finalize(statement);
        }
        
        
        //////////////////////////////////////////////////////////////////////////////////////
        
		//cv::vector<cv::Point2f> ifacet;
		cv::Point2f cp[4];
		cvR.points(cp);
		
		cv::Point cpp[4];
		for(int h=0;h<4;h++)
		{
			cpp[h].x = cp[h].x;
			cpp[h].y = cp[h].y;
            
            
            
            cpp[h].x = MIN(MAX(cpp[h].x,0),origin_imm->width);
            cpp[h].y = MIN(MAX(cpp[h].y,0),origin_imm->height);
		}
		
		cv::fillConvexPoly(res,cpp,4,cv::Scalar(B,G,R));
        isdone = 0;
        
        pt->matchim_num = selectedone;
        
		pt = pt->nextp;
        
        //if(pt==NULL)
        //    break;
        //cvReleaseImage(&sample);
        cvReleaseImage(&twosample);
        cvReleaseImage(&foursample);
        cvReleaseImage(&sixteensample);
	}
    
    
    
    
    
    
}


void Mosaic_CV::returnResult(IplImage *result)
{
    
    //point *tp = phead->firstp;
    
    
    cvCopy(result_imm, result);
}

Mosaic_CV::~Mosaic_CV()
{
    if(phead)
	{
		point* tp;
		point* ttp;
		tp = phead->firstp;
        
		for(int i=0; i<phead->numofP; i++)
		{
			ttp = tp;
			tp = tp->nextp;
            
			free(ttp);
            
            if(tp==NULL)
                break;
		}
        
		free(phead);
	}
	
    
	if(ehead)
	{
		point* tp;
		point* ttp;
		tp = ehead->firstp;
        
		for(int i=0; i<phead->numofP; i++)
		{
			ttp = tp;
			tp = tp->nextp;
            
            
			free(ttp);
            
            if(tp==NULL)
                break;
		}
        
		free(ehead);
	}
    
    if(basehead)
    {
        point* tp;
		point* ttp;
		tp = basehead->firstp;
        
		for(int i=0; i<basehead->numofP; i++)
		{
			ttp = tp;
			tp = tp->nextp;
            
            
			free(ttp);
            
            if(tp==NULL)
                break;
		}
        
		free(basehead);
    }
	
	if(origin_imm)
		cvReleaseImage(&origin_imm);
	if(line_imm)
		cvReleaseImage(&line_imm);
	if(image)
		cvReleaseImage(&image);
    
	if(DirectionMap!=NULL)
		free(DirectionMap);
}

pointhead* Mosaic_CV::getPointList(int option)
{
	//control may reach end of non-void function
	// if sentence 에서 switch로 변환 - 고한종.
	switch(option) {
		case 1:
			return phead;
		case 2:
			return basehead;
		default:
			NSLog(@"wrong usage at pointhead* Mosaic_CV::getPointList(int option)");
			return NULL;
	}
}

