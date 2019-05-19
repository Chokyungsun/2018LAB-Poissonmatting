#include "stdafx.h"
#include "fucs.h"
#include "IterativeOp.h"

Mat bgROI, imgROI;
Point corner1, corner2;
Rect box, box_bg;
Vec3f weight;
bool ldown = false, lup = false;
bool localbutton = false, bgbutton = false, channelbutton = false;
bool local_exist = false, bg_exist = false;

/***********************
*
* void setTRIMAPagain(Mat& alpha)
*	: alpha matte의 trimap을 다시 설정하는 함수
*
* alpha 값이 0이면 BLACK (definitely background)
* alpha 값이 1이면 WHITE (definitely foreground)
* alpha 값이 0에서 1 사이이면 GREY (unknown region)
*
***********************/

void setTRIMAPagain(Mat& alpha) {
	for (int y = 0; y < alpha.rows; y++) {
		for (int x = 0; x < alpha.cols; x++) {
			if (alpha.at<float>(y, x) == 0) {
				trimap.at<uchar>(y, x) = BLACK;
			}
			else if (alpha.at<float>(y, x) == 1) {
				trimap.at<uchar>(y, x) = WHITE;
			}
			else {
				trimap.at<uchar>(y, x) = GREY;
			}
		}
	}
}

/***********************
*
* void channel_selection(int event, int x, int y, int, void* param) 
*	: channel selection 버튼을 누르면 실행되는 함수
* 
* ComputAveBGR 함수로 선택된 배경의 RGB 평균을 구한 후, ComputeWeight함수로 weight를 구한다.
* 계산된 weight를 적용시킨 후 새로운 alpha matte 생성
*
***********************/

void channel_selection(int event, int x, int y, int, void* param) {
	if (channelbutton == true && local_exist == true && bg_exist == true) {

		Vec3f AveBGR;

		//Compute the average of RGB in the selected background 
		ComputeAveBGR(AveBGR, bgROI);

		weight = ComputeWeight(AveBGR, bgROI);

		ApplyColourtoBG(imgROI, weight);

		img.copyTo(imgROI);
		imshow("new img", img);

		//Iterative optimization
		Mat new_alpha;
		trimap.convertTo(new_alpha, CV_32F, 1 / 255.f); //from 0 to 1
		int result;
		for (int i = 0; i < 50; i++) {
			result = Iterative_opti(trimap, img, new_alpha);
			if (result == 1) break;

			imshow("Alpha", new_alpha);
		}

		channelbutton = false;
		local_exist = false;
		bg_exist = false;
	}
}

/***********************
*
* void ComputeAveBGR(Vec3f& output, const Mat& bg)
*	: bg의 RGB 평균 값을 계산하는 함수
*
***********************/

void ComputeAveBGR(Vec3f& output, const Mat& bg) {

	for (int y = 0; y < bg.rows; y++) {
		for (int x = 0; x < bg.cols; x++) {
			output += bg.at<Vec3f>(y, x);
		}
	}
	output = output / (float)(bg.size().height * bg.size().width);
	return;
}

/***********************
*
* Vec3f ComputeWeight(Vec3f& ave, const Mat& bg)
*	: 논문에 명시된 식으로 weight 값을 구하는 함수
*
* <참고사항>
*  논문에서 보여진 식으로 구현해볼라하였으나 결과물이 좋지않아 평균값으로 진행함.
*  toggle되어 있는 부분이 논문의 식을 대입하려고 했던 부분.
*
***********************/

Vec3f ComputeWeight(Vec3f& ave, const Mat& bg) {

	Vec3f weight;/*
	float av_B = ave[0];
	float av_G = ave[1];
	float av_R = ave[2];*/

	weight[0] = ave[0];
	weight[1] = ave[1];
	weight[2] = ave[2];

	/*
		for (int y = 0; y < bg.rows; y++) {
			for (int x = 0; x < bg.cols; x++) {
				float B = bg.at<Vec3f>(y, x)[0];
				float G = bg.at<Vec3f>(y, x)[1];
				float R = bg.at<Vec3f>(y, x)[2];

				weight[0] += 2 * R*R - 4 * B*R - 4 * av_R*R + 4 * av_B*R + 2 * B*B + 4 * av_R*B - 4 * av_B*B + 2 * av_R*av_R - 4 * av_B*av_R + 2 * av_B*av_B;
				weight[1] += 2 * G*G - 4 * B*G + 4 * av_B*G - 4 * av_G*G + 2 * B*B - 4 * av_B*B + 4 * av_G*B + 2 * av_B*av_B - 4 * av_G*av_B + 2 * av_G*av_G;
			}
		}
		weight[2] = 1 - weight[0] - weight[1];*/
	return weight;
}

/***********************
*
* void ApplyColourtoBG(Mat& local, const Vec3f& weight)
*	: 구한 weight를 original pic에 적용시키는 함수
*
* local region의 trimap에서 black인 부분에 weight 값 적용
*
***********************/

void ApplyColourtoBG(Mat& local, const Vec3f& weight) {
	Mat local_trimap = trimap(box).clone();
	imshow("local trimap", local_trimap);
	//cout << "local alpha size in the function: " << local_alpha.size() << endl;

	for (int y = 0; y < local_trimap.rows; y++) {
		for (int x = 0; x < local_trimap.cols; x++) {
			if (local_trimap.at<uchar>(y, x) == BLACK) {
				local.at<Vec3f>(y, x) = weight;
			}
		}
	}
	//cout << "local size in the function: " << local.size() << endl;
	imshow("changed local", local);
}

/***********************
*
* void buttons(int event, int x, int y, int, void* param)
*	: button UI의 mouse event 함수
*
***********************/

void buttons(int event, int x, int y, int, void* param) {
	if (event == EVENT_LBUTTONDOWN) {
		if (10 < x&&x < 240 && 10 < y&&y < 290) {
			localbutton = true;
			setMouseCallback("Original", local_brush);
		}
		if (250 < x&&x < 490 && 100 < y&&y < 290) {
			bgbutton = true;
			setMouseCallback("Original", bg_brush);
		}
		if (250 < x&&x < 490 && 10 < y&&y < 90) {
			channelbutton = true;
			setMouseCallback("Buttons", channel_selection);
		}
	}
}

/***********************
*
* void local_brush(int event, int x, int y, int, void* param)
*	: local region 선택하는 함수
*
***********************/

void local_brush(int event, int x, int y, int, void* param) {
	if (localbutton == true) {
		if (event == EVENT_LBUTTONDOWN) {
			ldown = true;
			corner1.x = x;
			corner1.y = y;
		}
		if (event == EVENT_LBUTTONUP) {
			if (0 < x&&x < img.cols && 0 < y&&y << img.rows) {
				if (abs(x - corner1.x) > 1 && abs(y - corner1.y) > 1) {
					lup = true;
					corner2.x = x;
					corner2.y = y;
				}
				else {
					ldown = false;
				}
			}
		}
		if (ldown == true && lup == false) {
			Point pt;
			pt.x = x;
			pt.y = y;
			Mat locale_img = img.clone();

			rectangle(locale_img, corner1, pt, Scalar(128, 0, 0));
			imshow("Original", locale_img);
		}
		if (ldown == true && lup == true) {
			box.width = abs(corner1.x - corner2.x);
			box.height = abs(corner1.y - corner2.y);

			box.x = min(corner1.x, corner2.x);
			box.y = min(corner1.y, corner2.y);

			imgROI = cv::Mat(img(box));
			imshow("Local_img", imgROI);
			ldown = false; lup = false;
			localbutton = false;
			local_exist = true;
		}
	}
}

/***********************
*
* void bg_brush(int event, int x, int y, int, void* param)
*	: local background region 선택하는 함수
*
***********************/

void bg_brush(int event, int x, int y, int, void* param) {
	if (bgbutton == true) {
		if (event == EVENT_LBUTTONDOWN) {
			ldown = true;
			corner1.x = x;
			corner1.y = y;
		}
		if (event == EVENT_LBUTTONUP) {

			if (0 < x&&x < img.cols && 0 < y&&y << img.rows) {
				if (abs(x - corner1.x) > 1 && abs(y - corner1.y) > 1) {
					lup = true;
					corner2.x = x;
					corner2.y = y;
				}
				else {
					ldown = false;
				}
			}
		}
		if (ldown == true && lup == false) {
			Point pt;
			pt.x = x;
			pt.y = y;
			Mat locale_img = img.clone();

			rectangle(locale_img, corner1, pt, Scalar(128, 0, 0));
			imshow("Original", locale_img);
		}
		if (ldown == true && lup == true) {
			box_bg.width = abs(corner1.x - corner2.x);
			box_bg.height = abs(corner1.y - corner2.y);

			box_bg.x = min(corner1.x, corner2.x);
			box_bg.y = min(corner1.y, corner2.y);

			bgROI = cv::Mat(img(box_bg));
			imshow("BG_img", bgROI);
			ldown = false; lup = false;
			bgbutton = false;
			bg_exist = true;
		}
	}
}



