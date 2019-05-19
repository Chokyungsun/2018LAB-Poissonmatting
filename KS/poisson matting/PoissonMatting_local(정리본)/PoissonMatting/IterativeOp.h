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

vector<PIXEL> neighbour_pixels;

float dot(const Vec3f& a, const Vec3f& b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/***********************
*
* int Iterative_opti(cv::Mat& trimap, const cv::Mat& img, cv::Mat& alpha0)
*	: trimap과 img를 가지고 alpha matte를 구하는 global matting하는 함수
* 
* alpha0은 main 함수에서 정의한 이전 alpha matte (초기 alpha0은 trimap과 같음)
*
***********************/

int Iterative_opti(cv::Mat& trimap, const cv::Mat& img, cv::Mat& alpha0) {
	neighbour_pixels.clear();
	//(F-B) init
	//Mat difference = Mat::zeros(trimap.rows, trimap.cols, CV_32FC3);

	setPixels();
	//GaussianBlur(difference, difference, Size(3, 3), 0, 0, BORDER_DEFAULT);

	//gradient x, y
	Mat Ix, Iy;
	gradient(img, Ix, Iy);

	//div(gradient I/(F-B))
	const Point offsets[] = { {0,-1}, {0,1}, {-1,0}, {1,0},{-1,-1}, {1,1},{-1,1},{1,-1} };
	//Neighbour and boundary pixels setting
	for (int y = 0; y < trimap.rows; y++) {
		for (int x = 0; x < trimap.cols; x++) {
			if (trimap.at<uchar>(y, x) == GREY) {
				PIXEL NB_pixel;
				NB_pixel.sum_v = 0;
				NB_pixel.p = Point(x, y);
				for (int i = 0; i < 8; i++) {
					Point q = NB_pixel.p + offsets[i];
					if (q.x < trimap.cols && q.x >= 0 && q.y < trimap.rows&& q.y >= 0) {
						if (trimap.at<uchar>(q) == GREY) {
							NB_pixel.N.push_back(q);
						}
						else {
							NB_pixel.B.push_back(q);
						}
						Vec3f dif = initDiff(trimap, img, x, y, NB_pixel);
						float dif_value = dif[0] * dif[0] + dif[1] * dif[1] + dif[2] * dif[2];
						Vec3f I = offsets[i].x*Ix.at<Vec3f>(q) + offsets[i].y*Iy.at<Vec3f>(q);
						NB_pixel.sum_v = NB_pixel.sum_v - dot(I, dif) / dif_value;
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
		//printf("Stop point: %f\n", stopPoint);
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
			Vec3f second_con;

			if (1> alpha1.at<float>(y, x)&&alpha1.at<float>(y, x) > 0.95) {
				for (auto& NB_pixel : neighbour_pixels) {
					if (NB_pixel.p == Point(x, y)) {
						second_con = img.at<Vec3f>(y, x) - NB_pixel.F_pixel; 
						if (dot(second_con, second_con) < 0.0001) {
							trimap.at<uchar>(y, x) = WHITE;
						}
						break;
					}
				}
			}

			if (0 < alpha1.at<float>(y, x) && alpha1.at<float>(y, x)< 0.05) {
				for (auto& NB_pixel : neighbour_pixels) {
					if (NB_pixel.p == Point(x, y)) {
						second_con = img.at<Vec3f>(y, x) - NB_pixel.B_pixel;
						if (dot(second_con, second_con) < 0.0001) {
							trimap.at<uchar>(y, x) = BLACK;
						}
						break;
					}
				}
			}
		}
	}
	return 0;
}