#pragma once

#include "stdafx.h"
#include "fucs.h"
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#pragma once

#define BLACK 0
#define WHITE 255
#define GREY 128

using namespace cv;
using namespace std;

struct PIXEL {
	Point p;
	vector<Point> N; // Neighbour
	vector<Point> B; // Boundary
	float sum_v;
};

vector<PIXEL> neighbour_pixels;

int Iterative_opti(cv::Mat& trimap, const cv::Mat& grey, cv::Mat& alpha0) {
	neighbour_pixels.clear();
	//(F-B) init
	Mat difference = Mat::zeros(trimap.rows, trimap.cols, CV_32F);

	setPixels();
	initDiff(trimap, grey, difference);
	//GaussianBlur(difference, difference, Size(3, 3), 0, 0, BORDER_DEFAULT);

	//gradient x, y
	Mat Ix, Iy;
	//Sobel(grey, xGrad, CV_32F, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	//Sobel(grey, yGrad, CV_32F, 0, 1, 3, 1, 0, BORDER_DEFAULT);
	gradient(grey, Ix, Iy);

	//div(gradient I/(F-B))
//	Mat Di; //laplacian alpha values
//	Divergence(Ix / difference, Iy / difference, Di);
//	imshow("Di", difference);
	//waitKey();
	const Point offsets[] = { {0,-1}, {0,1}, {-1,0}, {1,0},{-1,-1}, {1,1},{-1,1},{1,-1} };
	//Neighbour and boundary pixels setting
	for (int y = 0; y < trimap.rows; y++) {
		for (int x = 0; x < trimap.cols; x++) {
			if (trimap.at<uchar>(y, x) == GREY) {
				PIXEL NB_pixel;
				NB_pixel.sum_v = 0;
				NB_pixel.p = Point(x, y);
				for (int i = 0; i < 4; i++) {
					Point q = NB_pixel.p + offsets[i];
					if (q.x < trimap.cols && q.x >= 0 && q.y < trimap.rows&& q.y >= 0) {
						if (trimap.at<uchar>(q) == GREY) {
							NB_pixel.N.push_back(q);
						}
						else {
							NB_pixel.B.push_back(q);
						}
						NB_pixel.sum_v = NB_pixel.sum_v - (offsets[i].x*Ix.at<float>(q) + offsets[i].y*Iy.at<float>(q)) / 255.f / (difference.at<float>(NB_pixel.p) + 0.001);
					}
				}
				neighbour_pixels.push_back(NB_pixel);
			}
		}
	}

	//alpha reconstruction
	Mat alpha1;
	alpha1 = alpha0.clone();

	//alpha reconstruction
	for (int k = 0; k < 1000; k++) {
		float stopPoint = 0;
		Mat& x0 = (k % 2 == 0) ? alpha0 : alpha1;
		Mat& x1 = (k % 2 == 0) ? alpha1 : alpha0;

		for (auto& NB_pixel : neighbour_pixels)
		{
			float sum_fq = 0;
			for (auto& np : NB_pixel.N)
			{
				sum_fq += x0.at<float>(np);
			}
			for (auto& bp : NB_pixel.B) //0 or 1
			{
				sum_fq += trimap.at<uchar>(bp) / 255.f;
			}

			sum_fq += NB_pixel.sum_v;
			x1.at<float>(NB_pixel.p) = sum_fq / (float)(NB_pixel.N.size() + NB_pixel.B.size());

			float a = x1.at<float>(NB_pixel.p) - x0.at<float>(NB_pixel.p); 
			stopPoint = stopPoint + a * a;
		}
		printf("Stop point: %f\n", stopPoint);
		if (stopPoint < 0.01) {
			if (k == 0) {
				return 1;
				break;
			}
			cout << k << endl;
			break; 
		}
	}

	//F, B refinement
	for (int y = 0; y < alpha1.rows; y++) {
		for (int x = 0; x < alpha1.cols; x++) {
			if (alpha1.at<float>(y, x) > 0.95) {
				trimap.at<uchar>(y, x) = WHITE;
			}

			if (alpha1.at<float>(y, x) < 0.05) {
				trimap.at<uchar>(y, x) = BLACK;
			}
		}
	}
	return 0;
}