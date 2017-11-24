#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int main() {
	Mat src = imread("../../data/lena.png", IMREAD_UNCHANGED);
	if (src.empty()) {
		cout << "couldn't read image." << endl;
		return 0;
	}
	imshow("Image", src);
	
	// calulate rgb histogram
	Mat rHist, gHist, bHist;
	calcHist(vector<Mat>{ src }, vector<int>{ 2 }, noArray(), rHist, vector<int>{ 256 }, vector<float>{ 0, 255 }, false);
	calcHist(vector<Mat>{ src }, vector<int>{ 1 }, noArray(), gHist, vector<int>{ 256 }, vector<float>{ 0, 255 }, false);
	calcHist(vector<Mat>{ src }, vector<int>{ 0 }, noArray(), bHist, vector<int>{ 256 }, vector<float>{ 0, 255 }, false);
	// draw histogram image 
	Mat rgbHistImage(300, 600, CV_8UC3, Scalar(0, 0, 0));
	// normalized
	normalize(rHist, rHist, 0, rgbHistImage.rows, NORM_MINMAX);
	normalize(gHist, gHist, 0, rgbHistImage.rows, NORM_MINMAX);
	normalize(bHist, bHist, 0, rgbHistImage.rows, NORM_MINMAX);
	// draw lines
	for (int r = 1; r < rHist.rows; ++r) {
		line(rgbHistImage, Point(rgbHistImage.cols / 256.0 * (r - 1), rgbHistImage.rows - cvRound(rHist.at<float>(r - 1))),
			Point(rgbHistImage.cols / 256.0 * (r), rgbHistImage.rows - cvRound(rHist.at<float>(r))), Scalar(0, 0, 255), 1, LINE_8);
		line(rgbHistImage, Point(rgbHistImage.cols / 256.0 * (r - 1), rgbHistImage.rows - cvRound(gHist.at<float>(r - 1))),
			Point(rgbHistImage.cols / 256.0 * (r), rgbHistImage.rows - cvRound(gHist.at<float>(r))), Scalar(0, 255, 0), 1, LINE_8);
		line(rgbHistImage, Point(rgbHistImage.cols / 256.0 * (r - 1), rgbHistImage.rows - cvRound(bHist.at<float>(r - 1))),
			Point(rgbHistImage.cols / 256.0 * (r), rgbHistImage.rows - cvRound(bHist.at<float>(r))), Scalar(255, 0, 0), 1, LINE_8);
	}
	imshow("RGB Histogram", rgbHistImage);

	// convert to HSV image
	Mat hsv;
	cvtColor(src, hsv, COLOR_BGR2HSV);
	imshow("HSV Space", hsv);
	// draw 2D HS historgram
	Mat hsvHist;
	calcHist(vector<Mat>{ hsv }, vector<int>{ 0, 1 }, noArray(), hsvHist, vector<int>{ 60, 64 }, vector<float>{ 0, 180, 0, 255 }, false);
	// show hvHist image
	double maxVal{ 0 };
	minMaxLoc(hsvHist, nullptr, &maxVal, nullptr, nullptr);
	const int scale{ 8 };
	Mat hsvHistImg(hsvHist.cols * scale, hsvHist.rows * scale, CV_8UC3);
	for (int h = 0; h < hsvHist.rows; ++h) {
		for (int s = 0; s < hsvHist.cols; ++s) {
			float binVal = hsvHist.at<float>(h, s);
			int intensity = cvRound(binVal * 255 / maxVal);
			rectangle(hsvHistImg, Point(h * scale, s * scale), Point((h + 1) * scale - 1, (s + 1) * scale - 1), Scalar::all(intensity), CV_FILLED);
		}
	}
	imshow("2D HS Histogram", hsvHistImg);

	waitKey();
	return 0;
}