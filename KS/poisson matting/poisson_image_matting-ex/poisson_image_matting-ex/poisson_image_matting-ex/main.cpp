// Poisson.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifdef _DEBUG
#pragma comment (lib, "opencv_world342d")
#else
#pragma comment (lib, "opencv_world342")
#endif
using namespace cv;
using namespace std;

Mat dst;
Mat src;
Mat mask;
Mat srcCopy;

struct PIXEL {
	Point p;
	vector<Point> N; // Neighbor
	vector<Point> B; // Boundary
	Vec3f sum_v;
};

vector<PIXEL> pixels;

float dot(const Vec3f& a, const Vec3f& b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void mouseCallback(int event, int x, int y, int flag, void* param) {
	switch (event) {
	case EVENT_MOUSEMOVE:
		if (flag == EVENT_FLAG_LBUTTON) {
			circle(srcCopy, Point(x, y), 7, Scalar(255, 255, 255), CV_FILLED);
			imshow("source_image", srcCopy);
		}
		break;
	case EVENT_RBUTTONDOWN:
		PIXEL pixel;

		for (int i = 0; i < src.rows; i++) { //row
			for (int j = 0; j < src.cols; j++) { //col
				PIXEL pixel = {};
				if (src.at<Vec3f>(i, j) != srcCopy.at<Vec3f>(i, j)) {
					pixel.p = Point(j, i);
					if (j < src.cols - 1 && src.at<Vec3f>(i, j + 1) != srcCopy.at<Vec3f>(i, j + 1)) {
						pixel.N.push_back(Point(j + 1, i));
						pixel.sum_v = pixel.sum_v + (src.at<Vec3f>(i, j) - src.at<Vec3f>(i, j + 1));
					}
					else {
						pixel.B.push_back(Point(j + 1, i));
					}
					if (i > 0 && src.at<Vec3f>(i - 1, j) != srcCopy.at<Vec3f>(i - 1, j)) {
						pixel.N.push_back(Point(j, i - 1));
						pixel.sum_v = pixel.sum_v + (src.at<Vec3f>(i, j) - src.at<Vec3f>(i - 1, j));
					}
					else {
						pixel.B.push_back(Point(j, i - 1));
					}
					if (i < src.rows - 1 && src.at<Vec3f>(i + 1, j) != srcCopy.at<Vec3f>(i + 1, j)) {
						pixel.N.push_back(Point(j, i + 1));
						pixel.sum_v = pixel.sum_v + (src.at<Vec3f>(i, j) - src.at<Vec3f>(i + 1, j));
					}
					else {
						pixel.B.push_back(Point(j, i + 1));
					}
					if (j > 0 && src.at<Vec3f>(i, j - 1) != srcCopy.at<Vec3f>(i, j - 1)) {
						pixel.N.push_back(Point(j - 1, i));
						pixel.sum_v = pixel.sum_v + (src.at<Vec3f>(i, j) - src.at<Vec3f>(i, j - 1));
					}
					else {
						pixel.B.push_back(Point(j - 1, i));
					}

					pixels.push_back(pixel);
				}
			}
		}



		Mat X0 = dst.clone();
		//Mat X1 = dst.clone();

		for (int k = 0; k < 1000; k++) {

			//Mat& x0 = k % 2 ? X0 : X1;
			//Mat& x1 = k % 2 ? X1 : X0;
			float zz = 0;

			for (auto& pixel : pixels) {
				Vec3f sum_fq(0, 0, 0);

				for (auto& np : pixel.N) {
					sum_fq += X0.at<Vec3f>(np);
				}

				for (auto& bp : pixel.B) {
					sum_fq += dst.at<Vec3f>(bp);
				}

				sum_fq += pixel.sum_v;

				Vec3f xx = sum_fq / (float)(pixel.N.size() + pixel.B.size());
				zz += dot(X0.at<Vec3f>(pixel.p) - xx, X0.at<Vec3f>(pixel.p) - xx);
				X0.at<Vec3f>(pixel.p) = xx;
			}

			imshow("window", X0);
			waitKey(1);

			if (zz < 0.001) {
				cout << k << "\n";
				break;
			}
		}

		break;
	}
}

int main()
{
	imread("C:\\Users\\bbbb_\\Desktop\\dst4.jpg").convertTo(dst, CV_32F, 1 / 255.f);
	imread("C:\\Users\\bbbb_\\Desktop\\source4.jpg").convertTo(src, CV_32F, 1 / 255.f);
	mask = imread("C:\\Users\\bbbb_\\Desktop\\mask.png", 0);

	src.copyTo(srcCopy);

	namedWindow("source_image");
	setMouseCallback("source_image", mouseCallback);

	imshow("source_image", srcCopy);

	PIXEL pixel;

	resize(mask, mask, Size(src.cols, src.rows));
	uchar zero = 0;


	//for (int i = 0; i < src.rows; i++) { //row
	//	for (int j = 0; j < src.cols; j++) { //col
	//		PIXEL pixel = {};
	//		if (mask.at<uchar>(i, j) != zero) {
	//			pixel.p = Point(j, i);
	//			if (j < src.cols - 1 && mask.at<uchar>(i, j + 1) != zero) {
	//				pixel.N.push_back(Point(j + 1, i));
	//				pixel.sum_v = pixel.sum_v + (src.at<Vec3f>(i, j) - src.at<Vec3f>(i, j + 1));
	//			}
	//			else {
	//				pixel.B.push_back(Point(j + 1, i));
	//			}
	//			if (i > 0 && mask.at<uchar>(i - 1, j) != zero) {
	//				pixel.N.push_back(Point(j, i - 1));
	//				pixel.sum_v = pixel.sum_v + (src.at<Vec3f>(i, j) - src.at<Vec3f>(i - 1, j));
	//			}
	//			else {
	//				pixel.B.push_back(Point(j, i - 1));
	//			}
	//			if (i < src.rows - 1 && mask.at<uchar>(i + 1, j) != zero) {
	//				pixel.N.push_back(Point(j, i + 1));
	//				pixel.sum_v = pixel.sum_v + (src.at<Vec3f>(i, j) - src.at<Vec3f>(i + 1, j));
	//			}
	//			else {
	//				pixel.B.push_back(Point(j, i + 1));
	//			}
	//			if (j > 0 && mask.at<uchar>(i, j - 1) != zero) {
	//				pixel.N.push_back(Point(j - 1, i));
	//				pixel.sum_v = pixel.sum_v + (src.at<Vec3f>(i, j) - src.at<Vec3f>(i, j - 1));
	//			}
	//			else {
	//				pixel.B.push_back(Point(j - 1, i));
	//			}

	//			pixels.push_back(pixel);
	//		}
	//	}
	//}

	//Mat X0 = dst.clone();
	//Mat X1 = dst.clone();

	//for (int k = 0; k < 1000; k++) {

	//	Mat& x0 = k % 2 ? X0 : X1;
	//	Mat& x1 = k % 2 ? X1 : X0;

	//	for (auto& pixel : pixels) {
	//		Vec3f sum_fq(0, 0, 0);

	//		for (auto& np : pixel.N) {
	//			sum_fq += x0.at<Vec3f>(np);
	//		}

	//		for (auto& bp : pixel.B) {
	//			sum_fq += dst.at<Vec3f>(bp);
	//		}

	//		sum_fq += pixel.sum_v;

	//		x1.at<Vec3f>(pixel.p) = sum_fq / (float)(pixel.N.size() + pixel.B.size());
	//	}
	//	imshow("window", X0);
	//	waitKey(1);
	//}

	waitKey();
	return 0;
}