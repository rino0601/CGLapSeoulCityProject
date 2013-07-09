//
//  LGRViewController.m
//  MyG
//
//  Created by 이 경록 on 12. 11. 29..
//  Copyright (c) 2012년 LGR. All rights reserved.
//

// 쓸모없거나 중복되는 코드 삭제 - 고한종.

#import "LGRViewController.h"
#import "LGRAppDelegate.h"

@implementation LGRViewController

- (void) saveDataKey:(int)key width:(int)width height:(int)height
            tByTData:(NSData*)tbt fByFData:(NSData*)fbf sBySData:(NSData*)sbs
{
    LGRAppDelegate *appDelegate = [[UIApplication sharedApplication] delegate];
    NSManagedObjectContext *context = [appDelegate managedObjectContext];
    
    NSManagedObject *newContact;
    newContact = [NSEntityDescription
                  insertNewObjectForEntityForName:@"ImageDiscript"
                  inManagedObjectContext:context];
    
    [newContact setValue:[NSString stringWithFormat:@"%d",key] forKey:@"key"];
    [newContact setValue:[NSString stringWithFormat:@"%d",height] forKey:@"oriHeight"];
    [newContact setValue:[NSString stringWithFormat:@"%d",width] forKey:@"oriWidth"];
    [newContact setValue:tbt forKey:@"imageDataTByT"];
    [newContact setValue:fbf forKey:@"imageDataSByS"];
    [newContact setValue:sbs forKey:@"imageDataFByF"];
    
    NSError *error;
    [context save:&error];
}

@synthesize fetchedResultsController, managedObjectContext;

- (void)viewDidLoad
{

    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    NSArray *dirPaths;
    NSString *docsDir;
    dirPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES);
    
    docsDir = [dirPaths objectAtIndex:0];
    
    NSFileManager *filemgr;
    filemgr = [NSFileManager defaultManager];
    DBFilep = [NSString stringWithFormat:@"%@/DB",docsDir];
    sqlDBPath = [NSString stringWithFormat:@"%@/SqlDB",docsDir];

    if ([filemgr fileExistsAtPath: DBFilep ] == YES)
        NSLog (@"File exists");
    else
    {
        NSLog (@"File not found");
        NSData *databuffer;
        
        databuffer = [filemgr contentsAtPath: DBFilep ];
        
        
        [filemgr createFileAtPath: DBFilep contents: databuffer attributes: nil];
        //[filemgr release];
    }
    
    
    [self getphotos];
    
    
    
}

- (ALAssetsLibrary *)defaultAssetsLibrary
{
    static dispatch_once_t pred = 0;
    static ALAssetsLibrary *library = nil;
    dispatch_once(&pred, ^{
        library = [[ALAssetsLibrary alloc] init];
    });
    return library;
}

- (void) getphotos
{
    //올어셋 준비
    NSLog(@"asset ready");
    assetArray =[[ NSMutableArray alloc ] init];
    
    __block NSUInteger bb = 0;
    
    //ALAssetsLibrary *library = [[ALAssetsLibrary alloc ]init];
    library = [[ALAssetsLibrary alloc ]init];
   
    [library enumerateGroupsWithTypes:ALAssetsGroupAll usingBlock:^(ALAssetsGroup *group, BOOL *stop) {
        //NSLog(@"asdasfadf");
        if (group != nil)
        {
            [group enumerateAssetsUsingBlock:^(ALAsset *asset, NSUInteger index, BOOL *stop) {
                if (asset != nil)
                {
                    //NSLog(@"asset %@", asset);
                    [assetArray addObject:asset];
                    bb++;
                }
            }
             ];
        }
    }
                         failureBlock:^(NSError *error){
                             NSLog(@"fail --- %@", [error description]);
                         }
     ];

    
    //[assetArray addObject:@"aa"];
    
    //int a = assetArray.count;
    
   
}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    UIImagePickerController *imagepickerController = [[UIImagePickerController alloc] init];
    [imagepickerController setDelegate:self];
    //[imagepickerController setAllowsEditing:YES];
    if(buttonIndex == 0){
        
    }
    else if(buttonIndex == 1){
        
        UIImagePickerController *imagePicker = [[UIImagePickerController alloc] init];
        imagePicker.delegate = self;
        imagePicker.editing = YES;
        imagePicker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
        
        UIPopoverController *popover = [[UIPopoverController alloc] initWithContentViewController:imagePicker];
        
        m_pop = popover;
        
        //[popover presentPopoverFromRect:self->imageView.bounds inView:self->imageView permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
        
        [popover presentPopoverFromRect:CGRectMake(0.0, 0.0, 400.0, 400.0)
                                 inView:self.view
               permittedArrowDirections:UIPopoverArrowDirectionAny
                               animated:YES];
        
    }

    
}

- (IplImage *)CreateIplImageFromUIImage:(UIImage *)image {
    // Getting CGImage from UIImage
    CGImageRef imageRef = image.CGImage;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    // Creating temporal IplImage for drawing
    IplImage *iplimage = cvCreateImage(
                                       cvSize(image.size.width,image.size.height), IPL_DEPTH_8U, 4
                                       );
    // Creating CGContext for temporal IplImage
    CGContextRef contextRef = CGBitmapContextCreate(
                                                    iplimage->imageData, iplimage->width, iplimage->height,
                                                    iplimage->depth, iplimage->widthStep,
                                                    colorSpace, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrderDefault
                                                    );
    // Drawing CGImage to CGContext
    CGContextDrawImage(
                       contextRef,
                       CGRectMake(0, 0, image.size.width, image.size.height),
                       imageRef
                       );
    CGContextRelease(contextRef);
    CGColorSpaceRelease(colorSpace);
    
    // Creating result IplImage
    IplImage *ret = cvCreateImage(cvGetSize(iplimage), IPL_DEPTH_8U, 3);
    cvCvtColor(iplimage, ret, CV_RGBA2BGR);
    cvReleaseImage(&iplimage);
    
    return ret;
}

- (UIImage *)UIImageFromIplImage:(IplImage *)image {
    cv::Mat cvMat;
    cvMat = image;
    NSData *data = [NSData dataWithBytes:cvMat.data length:cvMat.elemSize()*cvMat.total()];
    CGColorSpaceRef colorSpace;
    
    if (cvMat.elemSize() == 1) {
        colorSpace = CGColorSpaceCreateDeviceGray();
    } else {
        colorSpace = CGColorSpaceCreateDeviceRGB();
    }
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
    
    // Creating CGImage from cv::Mat
    CGImageRef imageRef = CGImageCreate(cvMat.cols,                                 //width
                                        cvMat.rows,                                 //height
                                        8,                                          //bits per component
                                        8 * cvMat.elemSize(),                       //bits per pixel
                                        cvMat.step[0],                            //bytesPerRow
                                        colorSpace,                                 //colorspace
                                        kCGImageAlphaNone|kCGBitmapByteOrderDefault,// bitmap info
                                        provider,                                   //CGDataProviderRef
                                        NULL,                                       //decode
                                        false,                                      //should interpolate
                                        kCGRenderingIntentDefault                   //intent
                                        );
    
    
    // Getting UIImage from CGImage
    UIImage *finalImage = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    
    return finalImage;
}

#pragma mark UIImagePickerContoller Delegate
- (void)imagePickerController:(UIImagePickerController *)picker
        didFinishPickingImage:(UIImage *)image editingInfo:(NSDictionary *)editingInfo
{
    //imageView = [[UIImageView alloc] initWithImage:image];
    imageView.image = image;
    //tim = image;
    
    // selim.
    
    
    [self dismissModalViewControllerAnimated:YES];
}


- (IBAction)DBbuttonPressed:(id)sender
{
    
    
    const char *szfilenum =  [DBFilep UTF8String];

    //assetArray = [NSMutableArray arrayWithCapacity:200];
    
    ALAssetsLibrary* library = [[ALAssetsLibrary alloc] init];
    
    __block int assetcount = 0;
    numodDBIM = 0;
    
    sqlite3 *sampleImageDB;
    
    if (sqlite3_open([sqlDBPath UTF8String], &sampleImageDB) == SQLITE_OK) {
        printf("asdf\n");
    }
    
    const char* sql_stmt = "CREATE TABLE IF NOT EXISTS IMAGEDATA (ID INTEGER PRIMARY KEY, TBYTIMAGE BLOB, FBYFIMAGE BLOB, SBYSIMAGE BLOB)";
    
    char* errMsg;
    
    if (sqlite3_exec(sampleImageDB, sql_stmt, NULL, NULL, &errMsg) == SQLITE_OK) {
        printf("create success\n");
    }
    
    
    [library enumerateGroupsWithTypes:ALAssetsGroupAll usingBlock:^(ALAssetsGroup *group, BOOL *stop)
    {
        if (group != nil) {
            [group enumerateAssetsUsingBlock:^(ALAsset *asset, NSUInteger index, BOOL *stop) {
                if (asset != nil) {
                    //여기에서 하면 된다.
                    NSLog(@"asset %@", asset);
                    
                    sqlite3_stmt *statement ;
                    
                    UIImage *tim = [UIImage imageWithCGImage:[[asset defaultRepresentation] fullResolutionImage]];
                    IplImage *ttim = [self CreateIplImageFromUIImage:tim];
                    
                    IplImage *twobytwo = cvCreateImage(cvSize(2,2),8,ttim->nChannels);
                    IplImage *fourbyfour = cvCreateImage(cvSize(4,4),8,ttim->nChannels);
                    IplImage *sixteenbysixteen = cvCreateImage(cvSize(16,16),8,ttim->nChannels);
                    
                    cvResize(ttim, twobytwo);
                    cvResize(ttim, fourbyfour);
                    cvResize(ttim, sixteenbysixteen);
                    
                    //--DB에2x2넣기
                    
                    NSData *tbttemp = [NSData alloc];
                    tbttemp = [tbttemp initWithBytes:twobytwo->imageData length:twobytwo->widthStep*twobytwo->height];
                    
                    //--DB에4x4넣기
                    
                    NSData *fbftemp = [NSData alloc];
                    fbftemp = [fbftemp initWithBytes:fourbyfour->imageData length:fourbyfour->widthStep*fourbyfour->height];
                    
                    //--DB에16x16넣기
                    
                    NSData *sbstemp = [NSData alloc];
                    sbstemp = [sbstemp initWithBytes:sixteenbysixteen->imageData length:sixteenbysixteen->widthStep*sixteenbysixteen->height];

                    NSString *querySQL = [NSString stringWithFormat:@"INSERT INTO IMAGEDATA (ID,TBYTIMAGE,FBYFIMAGE,SBYSIMAGE) VALUES (%d,@A,@B,@C)",assetcount];
                    
                    if (sqlite3_prepare_v2(sampleImageDB, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK )
                    {
                        sqlite3_bind_blob(statement, 1, tbttemp.bytes, tbttemp.length, SQLITE_TRANSIENT);
                        sqlite3_bind_blob(statement, 2, fbftemp.bytes, fbftemp.length, SQLITE_TRANSIENT);
                        sqlite3_bind_blob(statement, 3, sbstemp.bytes, sbstemp.length, SQLITE_TRANSIENT);
                        
                        if(sqlite3_step(statement) == SQLITE_DONE){
                            
                        } else {
                            printf("FAIL to step!");
                        }
                    } else {
                        printf("FAIL to prepare!");
                    }
                    
                    
                    sqlite3_finalize(statement);
                    
                    imageView.image = tim;
                    assetcount++;
                    
                    numodDBIM = assetcount;
                    
                    cvReleaseImage(&twobytwo);
                    cvReleaseImage(&fourbyfour);
                    cvReleaseImage(&sixteenbysixteen);
                    cvReleaseImage(&ttim);
                    
                }
            } ];
        }
    }
                         failureBlock:^(NSError *error){
                             NSLog(@"fail --- %@", [error description]);
                         }
     ];
    
}

- (IBAction)PhotoButtonPressed:(id)sender
{
    UIActionSheet *actionsheet = [[UIActionSheet alloc]
                                  initWithTitle:nil
                                  delegate:self
                                  cancelButtonTitle:@"취소"
                                  destructiveButtonTitle:nil
                                  otherButtonTitles:@"사진 촬영", @"앨범에서 가져오기",nil];
    [actionsheet showInView:self.view];
    
    
}



- (IBAction)GoButtonPressed:(id)sender
{
    
    Mosaic_CV tmcreator;
    
    IplImage *input, *t_input;
    IplImage *output;
    
    FILE* pfileD = fopen([DBFilep UTF8String], "rb");
    
    fread(&(numodDBIM), sizeof(int), 1, pfileD);
    
    fclose(pfileD);
    
    t_input = [self CreateIplImageFromUIImage:imageView.image];
    
    
    int width;
    int height;
    
    if(t_input->width >500)
    {
        double aa = 500.0/t_input->width;
        
        width  = t_input->width * aa;
        height = t_input->height * aa;
    }
    
    else if(t_input->height > 500)
    {
        
        double aa = 500.0/t_input->height;
        
        width  = t_input->width * aa;
        height = t_input->height * aa;
    }
    
    else{
        width = t_input->width;
        height = t_input->height;
    }

    input = cvCreateImage(cvSize(width,height), t_input->depth, t_input->nChannels);
    cvResize(t_input, input);
    
    
    tmcreator.setTargetImage(input);
    
    tmcreator.setDBFile([sqlDBPath UTF8String], numodDBIM);
    
    tmcreator.goMosaic();
    
    NSLog(@"C_Mosaic");
    
    output = cvCreateImage(cvGetSize(input), input->depth, input->nChannels);
    
    tmcreator.returnResult(output);
    
    pointhead *phead = tmcreator.getPointList(1); //포인트 리스트 얻어와서.. 붙이기
    pointhead *phead2 = tmcreator.getPointList(2);

    point *tp = phead->firstp;
    point *tp1 = phead2->firstp;
    
    while(tp!=NULL)
    {
        cvCircle(output, cvPoint(tp->x, tp->y), 5, cvScalar(0,255,0));
        tp = tp->nextp;
    }
    
    while(tp1!=NULL)
    {
        cvCircle(output, cvPoint(tp1->x, tp1->y), 3, cvScalar(0,0,255));
        tp1 = tp1->nextp;
    }
    
    
    UIImage *re = [self UIImageFromIplImage:output];
    
    imageView2.image = re;
    
    
  
        
    //[self getphotos];
    
    IplImage *re_cv2 = [self attachpictures:phead2 input:input option:1];
    
    
    
	IplImage *re_cv3 = [self attachpictures:phead input:re_cv2 option:0];
    
    //cvCvtColor(re_cv3, re_cv3, CV_RGB2BGR);
    
    UIImage *re2 = [self UIImageFromIplImage:re_cv3];
    
    imageView3.image = re2;
   
   
    
}

- (IplImage *) attachpictures:(pointhead *)phead input:(IplImage *)input option:(int)option
{
	point* tp;
    
    if(phead->numofP == 0)
    {
        NSLog(@"no seed points");
        return NULL;
    }
    
    tp = phead->firstp;
    
    IplImage* resim_t = cvCreateImage(cvGetSize(input), input->depth, input->nChannels);
    IplImage* resim = cvCreateImage(cvGetSize(input), input->depth, input->nChannels);
    
    
    if(option==1)
    {
        cvCopy(input, resim_t);
        cvCvtColor(resim_t, resim, CV_BGR2RGB);
    }
    
    else
    {
        cvCopy(input, resim);
    }
    
    //cvSet(resim, cvScalar(255,255,255));
    //UIImage* matchim = [UIImage imageWithCGImage:[[assetArray objectAtIndex:0] fullResolutionImage]];
    
    //imageView3.image = matchim;
   
    
    while(tp!=NULL) //포인트 수많큼 그리자
    {
        int photoindex = tp->matchim_num;
        
         //matchim;
        
        //ALAssetRepresentation *rep = [[assetArray objectAtIndex:photoindex] fullResolutionImage];
        
        //CGImageRef iref = [rep fullResolutionImage];
        
   
        
        
        
        ALAssetRepresentation *asset = [[assetArray objectAtIndex:photoindex] defaultRepresentation];
        
        CGImageRef CGIM = [asset fullResolutionImage];
        
        UIImage* matchim = [UIImage imageWithCGImage:CGIM];
        

        
        IplImage *matchim_cv_ori = [self CreateIplImageFromUIImage:matchim];


        
                
        cvCvtColor(matchim_cv_ori, matchim_cv_ori, CV_BGR2RGB);
        
        int selsize = MAX(matchim_cv_ori->width,matchim_cv_ori->height);
        
        IplImage* resizedrot = cvCreateImage(cvSize(selsize,selsize), matchim_cv_ori->depth, matchim_cv_ori->nChannels);
        
        cvResize(matchim_cv_ori, resizedrot);
        
        IplImage*  matchim_cv = cvCreateImage(cvSize(phead->rectsize, phead->rectsize), matchim_cv_ori->depth, matchim_cv_ori->nChannels);
        
        cvResize(resizedrot, matchim_cv);
        
        IplImage* rot_matim = cvCreateImage(cvSize(phead->rectsize, phead->rectsize), matchim_cv->depth, matchim_cv->nChannels);
        
       
        const double scale = 0.8;
        CvPoint2D32f center = cvPoint2D32f(matchim_cv->width/2.0,matchim_cv->height/2.0);
        
        CvMat *rot_mat = cvCreateMat(2,3,CV_32FC1);
        
        //cvGetAffineTransform(srcTri, dstTri, warp_mat);
        
        cv2DRotationMatrix( center, tp->direction , scale, rot_mat);
        
        
        
        /* rot_mat의 값이 정확히 어떤 유니온을 택해야 하는지 의문사항입니다. 우선 float으로 연산했지만 추후 변경이 필요할 수 있습니다. by Kim */
        for (int h = 0; h < matchim_cv->height; h ++ ) {
            for (int w = 0; w < matchim_cv->width; w ++ ) {
                //printf("%f %f %f %f %f %f", rot_mat->data.fl[0], rot_mat->data.fl[1], rot_mat->data.fl[2], rot_mat->data.fl[3], rot_mat->data.fl[4], rot_mat->data.fl[5]);
                int rotatedW = rot_mat->data.fl[0]*w + rot_mat->data.fl[1]*h + rot_mat->data.fl[2];
                int rotatedH = rot_mat->data.fl[3]*w + rot_mat->data.fl[4]*h + rot_mat->data.fl[5];
                
                int i = tp->x + (rotatedW - matchim_cv->width/2); // 회전한 좌표의 중심점 부터의 좌표를 붙이고자 하는 곳의 좌표에서 뺌.
                int j = tp->y + (rotatedH - matchim_cv->height/2);
                
                if(i<resim->width && i>=0 && j<resim->height && j>= 0){
                    resim->imageData[j*resim->widthStep + i*3 + 0] = (unsigned char) matchim_cv->imageData[h*matchim_cv->widthStep + w*3 + 0];
                    resim->imageData[j*resim->widthStep + i*3 + 1] = (unsigned char) matchim_cv->imageData[h*matchim_cv->widthStep + w*3 + 1];
                    resim->imageData[j*resim->widthStep + i*3 + 2] = (unsigned char) matchim_cv->imageData[h*matchim_cv->widthStep + w*3 + 2];
                }
            }
        }
        
        tp = tp->nextp;
        
        cvReleaseImage(&matchim_cv);
        cvReleaseImage(&matchim_cv_ori);
        cvReleaseImage(&rot_matim);
        cvReleaseImage(&resizedrot);
        cvReleaseMat(&rot_mat);
        
    }
    
    return resim;
}

- (void)matrot:(float [2][2])rot source:(CvPoint2D32f *)src destination:(CvPoint2D32f *)dst
{
    //rot 2x2 
    //src 2x1
    
    dst[0].x = rot[0][0]*src[0].x + rot[0][1]*src[0].y;
    dst[0].y = rot[1][0]*src[0].x + rot[1][1]*src[0].y;
    
    dst[1].x = rot[0][0]*src[1].x + rot[0][1]*src[1].y;
    dst[1].y = rot[1][0]*src[1].x + rot[1][1]*src[1].y;
    
    dst[2].x = rot[0][0]*src[2].x + rot[0][1]*src[2].y;
    dst[2].y = rot[1][0]*src[2].x + rot[1][1]*src[2].y;
}






@end
