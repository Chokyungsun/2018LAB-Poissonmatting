#include "stdafx.h"
#include "fucs.h"

std::vector<pixel> pixels = {};

void Divergence(const Mat& Ix, const Mat& Iy, Mat& output) {
	output.create(Ix.size(), CV_32FC1);
	for (int i = 0; i < Ix.rows; i++)
	{
		for (int j = 0; j < Ix.cols; j++)
		{
			float x, y;
			if (i == 0) y = Iy.at<float>(i + 1, j) - Iy.at<float>(i, j);
			else if (i == Iy.rows - 1) y = Iy.at<float>(i, j) - Iy.at<float>(i - 1, j);
			else y = (float)(Iy.at<float>(i + 1, j) - Iy.at<float>(i - 1, j)) / 2;

			if (j == 0) x = Ix.at<float>(i, j + 1) - Ix.at<float>(i, j);
			else if (j == Ix.cols - 1) x = Ix.at<float>(i, j) - Ix.at<float>(i, j - 1);
			else x = (float)(Ix.at<float>(i, j + 1) - Ix.at<float>(i, j - 1)) / 2;

			output.at<float>(i, j) = x + y;
		}
	}
}

void gradient(cv::Mat src, cv::Mat& x, cv::Mat& y) {
	x.create(src.size(), CV_32FC3);
	y.create(src.size(), CV_32FC3);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (i == 0) y.at < Vec3f >(i, j) = src.at<Vec3f>(i + 1, j) - src.at<Vec3f>(i, j);
			else if (i == src.rows - 1) y.at<Vec3f>(i, j) = src.at<Vec3f>(i, j) - src.at<Vec3f>(i - 1, j);
			else y.at<Vec3f>(i, j) = (Vec3f)(src.at<Vec3f>(i + 1, j) - src.at<Vec3f>(i - 1, j)) / 2;

			if (j == 0) x.at<Vec3f>(i, j) = src.at<Vec3f>(i, j + 1) - src.at<Vec3f>(i, j);
			else if (j == src.cols - 1) x.at<Vec3f>(i, j) = src.at<Vec3f>(i, j) - src.at<Vec3f>(i, j - 1);
			else x.at<Vec3f>(i, j) = (Vec3f)(src.at<Vec3f>(i, j + 1) - src.at<Vec3f>(i, j - 1)) / 2;
		}
	}
}

void laplacian(const cv::Mat src, cv::OutputArray output) {
	cv::Mat gaussian;
	GaussianBlur(src, gaussian, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
	//imshow("gaussian", gaussian);
	cv::Mat dst;
	int delta = 0, ddepth = CV_16S, scale = 1, kernel_size = 3;
	Laplacian(gaussian, dst, ddepth, kernel_size, scale, delta, cv::BORDER_DEFAULT);
	convertScaleAbs(dst, output);
}

void setPixels()
{
	string listFile = "pixelDistance.txt";
	ifstream openFile(listFile.data());

	if (openFile.is_open())
	{
		string info;
		char* x;
		char* y;
		char* context;
		while (getline(openFile, info))
		{
			char * c_info = new char[info.length() + 1];
			strcpy_s(c_info, info.length() + 1, info.c_str());

			x = strtok_s(c_info, ",", &context);
			y = strtok_s(context, "\n", &context);
			pixel p;
			p.x = atoi(x);
			p.y = atoi(y);

			pixels.push_back(p);
		}
		openFile.close();
	}

	//for (std::vector<pixel>::iterator it = pixels.begin(); it != pixels.end(); ++it)
	//{
	//	cout << "pixels x: " << (*it).x << endl;
	//	cout << "pixels y: " << (*it).y << endl;

	//}
	printf("Loaded\n");
}

Vec3f initDiff(cv::Mat& mask, const cv::Mat& img,const int& x, const int& y, PIXEL& pixel) {

	Vec3f f_pixel = 0;
	Vec3f b_pixel = 0;


	for (auto& it : pixels)
	{
		int absoluteY = min(mask.rows - 1, max(0, y + it.y));
		int absoluteX = min(mask.cols - 1, max(0, x + it.x));
		uchar msk = mask.at<uchar>(absoluteY, absoluteX);
		if (msk == WHITE)
		{
			//cout << "white." << endl;
			//pic.at<uchar>(absoluteY, absoluteX) = 200;
			f_pixel = img.at<Vec3f>(absoluteY, absoluteX);
			pixel.F_pixel = f_pixel;
			break;
		}
	}
	for (auto& it : pixels)
	{
		int absoluteY = min(mask.rows - 1, max(0, y + it.y));
		int absoluteX = min(mask.cols - 1, max(0, x + it.x));
		uchar msk = mask.at<uchar>(absoluteY, absoluteX);
		if (msk == BLACK)
		{
			//cout << "black." << endl;
			//pic.at<uchar>(absoluteY, absoluteX) = 200;
			b_pixel = img.at<Vec3f>(absoluteY, absoluteX);
			pixel.B_pixel = b_pixel;
			break;
		}
	}
	//cout << f_pixel << "," << b_pixel << endl;
	return f_pixel - b_pixel;

	//	cv::imshow("points of the nearest ", pic);
}