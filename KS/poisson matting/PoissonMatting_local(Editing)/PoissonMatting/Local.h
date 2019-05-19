#pragma once

#include <opencv2/core.hpp>

//Local
//void local_brush(int event, int x, int y, int, void* param);
//void bg_brush(int event, int x, int y, int, void* param);
//void buttons(int event, int x, int y, int, void* param);
void ComputeAveRGB(cv::Vec3f& output, const cv::Mat& bg);