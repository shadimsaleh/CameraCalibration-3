///////////////////////////////////////////////////////////////////////////////
//Camera Calibration Program using OpenCV
//Written by 
//Kyohei Iwane, Osaka University
//Modified by
//Kenichi Ohara, Meijo University
//
//**Dependencies**
//OpenCV 2.4.8
//****************
//
//*****************************************************************************
//Version
//*****************************************************************************
//1.0 (2014/03/25): Original implementation based on OpenCV sample program
///////////////////////////////////////////////////////////////////////////////
#include<cstring>
#include<opencv/cv.h>
#include<opencv/highgui.h>

#define INPUT_FILE_NAME "calib_cfg.yml"

int main(void)
{
	int pattern_row, pattern_col, image_num, chessboard_size;
	std::string filename, file_type, ofilename;
	//Open the yaml file to load the calibration setting
	CvFileStorage *cfg;
	if( (cfg = cvOpenFileStorage (INPUT_FILE_NAME, 0, CV_STORAGE_READ)) == NULL)
	{
		printf("Please check calibration setting file!\n");
		getchar();
		return -1;
	}
	else
	{
		image_num = cvReadIntByName(cfg, NULL, "image_num", 0);
		pattern_row = cvReadIntByName(cfg, NULL, "pattern_row", 0);
		pattern_col = cvReadIntByName(cfg, NULL, "pattern_col", 0);
		chessboard_size = cvReadIntByName(cfg, NULL, "chessboard_size", 0);
		filename = cvReadStringByName(cfg, NULL, "image_stored_directory", 0);
		filename += "\\";
		filename += cvReadStringByName(cfg, NULL, "image_file_label", 0);
		file_type = cvReadStringByName(cfg, NULL, "image_file_type", 0);
		ofilename = cvReadStringByName(cfg, NULL, "output_file_name", 0);
		printf("image_num: %d\n", image_num);
		printf("image_file_info: %sXXX.%s\n", filename.c_str(), file_type.c_str());
		printf("output_file_name: %s.yml\n", ofilename.c_str(), file_type.c_str());
		printf("Chess pattern: %d x %d  Size: %d[mm]\n", pattern_col, pattern_row, chessboard_size);
	}

	int pat_size,all_points;
	pat_size   = pattern_row*pattern_col;
	all_points = image_num*pat_size;
	
	IplImage **src_img = 0;
	IplImage *src0;
	char buf[50];

	//Load 1st image obtaining image information
	sprintf(buf, "%s000.%s", filename.c_str(), file_type.c_str());
	if (( src0 = cvLoadImage(buf, CV_LOAD_IMAGE_COLOR) ) == NULL)
	{
		fprintf (stderr, "Cannot load image file : %s¥n", buf );
	}
	if( NULL == (src_img = (IplImage**)malloc(image_num*sizeof(src0))) )
	{
		printf("Cannot allocate image memory¥n");
		return -1;
	}
	
	CvSize pattern_size = cvSize (pattern_col, pattern_row);
	CvPoint3D32f *objects = (CvPoint3D32f *) cvAlloc ( sizeof( CvPoint3D32f) * all_points);
	CvPoint2D32f *corners = (CvPoint2D32f *) cvAlloc (sizeof (CvPoint2D32f) * all_points);
	CvMat object_points;
	
	//Loading camera image to allocated image buffer
	for (int i(0); i < image_num; i++)
	{
		sprintf( buf,  "%s%03d.%s", filename.c_str(), i, file_type.c_str());
		if ((src_img[i] = cvLoadImage (buf, CV_LOAD_IMAGE_COLOR)) == NULL)
		{
			fprintf (stderr, "cannot load image file : %s¥n", buf);
		}
	}
	
	//Setting 3D space coordinate
	for (int i(0); i < image_num; i++)
	{
		for (int j(0); j < pattern_row; j++)
		{
			for (int k(0); k < pattern_col; k++)
			{
				objects[i * pat_size + j * pattern_col + k].x = j * chessboard_size;
				objects[i * pat_size + j * pattern_col + k].y = k * chessboard_size;
				objects[i * pat_size + j * pattern_col + k].z = 0.0;
			}
		}
	}
	cvInitMatHeader (&object_points, all_points, 3, CV_32FC1, objects);
	
	//Corner detection for chess board
	int found_num = 0;
	int false_num = 0;
	int found;
	int corner_count;
	
	cvNamedWindow ("Calibration", CV_WINDOW_AUTOSIZE);
	//Sometimes, chess pattarns cannot be detected.
	//To avoid this problem, false cases are removed from detection results.
	int *p_count = new int[image_num];
	for (int i(0); i < image_num; i++) 
	{
		found = cvFindChessboardCorners (src_img[i], pattern_size, &corners[(i-false_num) * pat_size], &corner_count);
		fprintf (stderr, "%02d...", i);
		if (found)
		{
			fprintf (stderr, "ok\n");
			found_num++;
		}
		else
		{
			fprintf (stderr, "fail¥n");
			false_num++;
		}
		
		//Modifying the corner position to subpixel precision
		IplImage *src_gray = cvCreateImage (cvGetSize (src_img[i]), IPL_DEPTH_8U, 1);
		cvCvtColor (src_img[i], src_gray, CV_BGR2GRAY);
		cvFindCornerSubPix (src_gray, &corners[ (i-false_num) * pat_size], corner_count,
                    cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
		cvDrawChessboardCorners (src_img[i], pattern_size, &corners[ (i-false_num) * pat_size], corner_count, found);
		p_count[i-false_num] = corner_count;
		cvShowImage ("Calibration", src_img[i]);
		cvWaitKey (10);
	}
	cvDestroyWindow ("Calibration");

	CvMat image_points;
	CvMat point_counts;	
	cvInitMatHeader (&image_points, found_num*pat_size, 1, CV_32FC2, corners);
	cvInitMatHeader (&point_counts, found_num, 1, CV_32SC1, p_count);
	
	//
	cvInitMatHeader (&object_points, found_num*pat_size, 3, CV_32FC1, objects);
	
	//Intrinsic parameter estimation
	CvMat *intrinsic = cvCreateMat (3, 3, CV_64FC1);
	CvMat *rotation = cvCreateMat (1, 3, CV_32FC1);
	CvMat *translation = cvCreateMat (1, 3, CV_32FC1);
	CvMat *distortion = cvCreateMat (5, 1, CV_64FC1);
	printf("Estimate intrinsic parameter\n" ); 
	cvCalibrateCamera2 (&object_points, &image_points, &point_counts, cvSize (640, 480), intrinsic, distortion);
		
	//Extrinsic parameter estimation
	printf("Estimate extrinsic parameter\n" ); 
	CvMat sub_image_points, sub_object_points;
	int base = 0;
	cvGetRows (&image_points, &sub_image_points, base * pat_size, (base + 1) * pat_size);
	cvGetRows (&object_points, &sub_object_points, base * pat_size, (base + 1) * pat_size);
	cvFindExtrinsicCameraParams2 (&sub_object_points, &sub_image_points, intrinsic, distortion, rotation, translation);

	//Output calibration result to YAML type file.
	printf("Output calibration result\n" );
	CvFileStorage *fs;
	char obuf[32];
	sprintf(obuf, "%s.yml", ofilename.c_str(), file_type.c_str());
	fs = cvOpenFileStorage (obuf, 0, CV_STORAGE_WRITE);
	cvWriteInt (fs, "image_width", src0->width);
	cvWriteInt (fs, "image_height", src0->height);
	cvWrite (fs, "camera_matrix", intrinsic);
	cvWrite (fs, "rotation", rotation);
	cvWrite (fs, "translation", translation);
	cvWrite (fs, "distortion_coefficients", distortion);
	
	cvReleaseFileStorage (&fs);
	cvReleaseImage(&src0);
	for (int i(0); i < image_num; i++)
	{
		cvReleaseImage (&src_img[i]);
	}
	printf("Please hit any key!\n");
	getchar();
	return 0;
}