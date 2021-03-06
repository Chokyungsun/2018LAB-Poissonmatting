#include "stdafx.h"
#include "fucs.h"
#include "IterativeOp.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define BLACK 0
#define WHITE 255
#define GREY 128

using namespace cv;
using namespace std;

int main() {

	Mat img = imread("C:/Users/Cho/Desktop/2018LAB/KS/poisson matting/PoissonMatting_colour/input/GT14.png");
	img.convertTo(img, CV_32FC3, 1.0/255.0);
	cv::imshow("Original", img);

	Mat trimap = imread("C:/Users/Cho/Desktop/2018LAB/KS/poisson matting/PoissonMatting_colour/Trimap1/GT14.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow("Trimap", trimap);

	//Mat grey = Mat::zeros(img.rows, img.cols, CV_8U);
	//cvtColor(img, grey, CV_BGR2GRAY);
	//cv::imshow("grey", grey);
	// 
	//cv::imshow("foreFlag", foreFlag);

	//Iterative optimization
	Mat alpha;
	trimap.convertTo(alpha, CV_32F, 1 / 255.f); //from 0 to 1
	int result;
	for (int i = 0; i < 50; i++) {
		result = Iterative_opti(trimap, img, alpha);
		if (result == 1) break;

		imshow("Alpha", alpha);
		waitKey(10);
	}


	alpha.convertTo(alpha, CV_32F, 255.f);
	 
	imshow("result", alpha);
	imwrite("alphamatte_GT14.bmp", alpha);

	imshow("trimap_result", trimap);
	imwrite("Finaltrimap_GT14.bmp", trimap);

	cv::waitKey(0);

	return 0;
}

