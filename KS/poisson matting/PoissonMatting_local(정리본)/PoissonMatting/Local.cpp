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
*	: alpha matte�� trimap�� �ٽ� �����ϴ� �Լ�
*
* alpha ���� 0�̸� BLACK (definitely background)
* alpha ���� 1�̸� WHITE (definitely foreground)
* alpha ���� 0���� 1 �����̸� GREY (unknown region)
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
*	: channel selection ��ư�� ������ ����Ǵ� �Լ�
* 
* ComputAveBGR �Լ��� ���õ� ����� RGB ����� ���� ��, ComputeWeight�Լ��� weight�� ���Ѵ�.
* ���� weight�� �����Ų �� ���ο� alpha matte ����
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
*	: bg�� RGB ��� ���� ����ϴ� �Լ�
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
*	: ���� ��õ� ������ weight ���� ���ϴ� �Լ�
*
* <�������>
*  ������ ������ ������ �����غ����Ͽ����� ������� �����ʾ� ��հ����� ������.
*  toggle�Ǿ� �ִ� �κ��� ���� ���� �����Ϸ��� �ߴ� �κ�.
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
*	: ���� weight�� original pic�� �����Ű�� �Լ�
*
* local region�� trimap���� black�� �κп� weight �� ����
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
*	: button UI�� mouse event �Լ�
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
*	: local region �����ϴ� �Լ�
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
*	: local background region �����ϴ� �Լ�
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



