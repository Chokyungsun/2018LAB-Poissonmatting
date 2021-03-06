#include "stdafx.h"
#include "fucs.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

Mat alpha, img, trimap;

/****** Local matting *****
*
*	Global matting을 완성한 img 와 alpha matte가 있다는 가정하에 맘에 안드는 local 부분을 수정하는 코드
*
*	[구성]
*	 1. Channel Selection (Completed)
*	 2. Boosting brush
*	 3. Highpass filtering
*	 4. Diffusion filtering
*	 5. Clone brush 
*
*	[파일]
*	 - main.cpp 
*	 - Local.cpp : local matting의 핵심 함수들이 들어있는 파일
*	 - PoissonMatting.cpp : matting을 위해 필요한 부가적인 함수들이 들어있는 파일
*	 - IterativeOp.h : alpha matte를 만들기 위한 구현 함수가 들어있는 헤더 파일
*	 - func.h : matting 함수들이 정의되어 있는 헤더 파일
*
**************************/

int main() {
	/***********************
	*
	* 이미지 원본 파일과 알파 매트 열기 
	* 알파 매트는 Global matting을 하고 난 후 matte
	* 
	***********************/

	img = imread("C:/Users/whrud/Desktop/2018LAB/KS/poisson matting/PoissonMatting_local/input/GT14.png");
	img.convertTo(img, CV_32FC3, 1.0 / 255.0);
	cv::imshow("Original", img);

	alpha = imread("C:/Users/whrud/Desktop/2018LAB/KS/poisson matting/PoissonMatting_colour/PoissonMatting/alphamatte_GT14.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	namedWindow("Original ALPHA");
	imshow("Original ALPHA", alpha);

	alpha.convertTo(alpha, CV_32FC1, 1/255.f);

	/***********************
	*
	* Create new trimap
	* 불러온 alpha matte에 대한 새로운 trimap 생성
	*
	***********************/

	trimap = Mat::zeros(alpha.rows, alpha.cols, CV_LOAD_IMAGE_GRAYSCALE);
	setTRIMAPagain(alpha);
	cv::imshow("Trimap", trimap);

	/***********************
	*
	* Button UI 생성
	* 구성: Local brush, Background brush, Channel Selection
	*
	***********************/
	string Channel_selStr("Channel Selection");
	string localButtonStr("Local brush");
	string bgButtonStr("Background brush");
	
	Rect localButton = Rect(10, 10, 230, 280);
	Rect Channel_sel = Rect(250, 10, 240, 80);
	Rect bgButton = Rect(250, 100, 240, 190);

	Mat3b canvas = Mat3b(300, 1000, Vec3b(0, 0, 0));

	canvas(localButton) = Vec3b(100, 200, 200);
	canvas(Channel_sel) = Vec3b(200, 200, 200);
	canvas(bgButton) = Vec3b(100, 200, 200);
	
	putText(canvas(Channel_sel), Channel_selStr, Point(Channel_sel.width*0.25, Channel_sel.height*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
	putText(canvas(localButton), localButtonStr, Point(localButton.width*0.25, localButton.height*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
	putText(canvas(bgButton), bgButtonStr, Point(bgButton.width*0.25, bgButton.height*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));

	namedWindow("Buttons");
	imshow("Buttons", canvas);

	setMouseCallback("Buttons", buttons);

	cv::waitKey(0);

	return 0;
}