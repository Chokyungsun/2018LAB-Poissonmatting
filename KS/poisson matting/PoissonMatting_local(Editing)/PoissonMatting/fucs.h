
#pragma once

//basic functions for Poisson matting
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define BLACK 0
#define WHITE 255
#define GREY 128

using namespace cv;
using namespace std;

struct pixel
{
	int x, y;
};

struct PIXEL {
	Point p;
	vector<Point> N; // Neighbour
	vector<Point> B; // Boundary
	Vec3f F_pixel; // the nearest Foreground
	Vec3f B_pixel; // the nearest Foreground
	Point F_point;
	Point B_point;
	float sum_v;
};

extern std::vector<pixel> pixels; //offset pixels
extern Mat alpha, img, trimap;

void Divergence(const Mat& Ix, const Mat& Iy, Mat& output);
void laplacian(const cv::Mat src, cv::OutputArray output);

//Global
void gradient(cv::Mat src, cv::Mat& x, cv::Mat& y);
void setPixels();
Vec3f initDiff(cv::Mat& mask, const cv::Mat& img, const int& x, const int& y, PIXEL& pixel);


//Local
void setTRIMAPagain(Mat& alpha);
void local_brush(int event, int x, int y, int, void* param);
void buttons(int event, int x, int y, int, void* param);

//Channel Selection
void channel_selection(int event, int x, int y, int, void* param);
void bg_brush(int event, int x, int y, int, void* param);
void ComputeAveBGR(cv::Vec3f& output, const cv::Mat& bg);
Vec3f ComputeWeight(Vec3f& ave, const Mat& bg);
void ApplyColourtoBG(Mat& local,const Vec3f& weight);

//Boosting brush(Editing)
void boosting_brush(int event, int x, int y, int, void* param);
void ApplyBoosting(int event, int x, int y, void* param);

//Clone brush(Editing)
void clone_brush(int event, int x, int y, int, void* param);
void cloneA_brush(int event, int x, int y, int, void* param);
void cloneB_brush(int event, int x, int y, int, void* param);
