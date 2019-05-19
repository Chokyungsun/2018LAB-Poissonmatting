//basic functions for Poisson matting
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#pragma once

#define BLACK 0
#define WHITE 255
#define GREY 128

using namespace cv;
using namespace std;

struct pixel
{
	int x, y;
};

extern std::vector<pixel> pixels;

void Divergence(const Mat& Ix, const Mat& Iy, Mat& output);

void gradient(cv::Mat src, cv::Mat& x, cv::Mat& y);
void laplacian(const cv::Mat src, cv::OutputArray output);

void setPixels();
void initDiff(cv::Mat& mask, const cv::Mat& grey, cv::Mat& output);