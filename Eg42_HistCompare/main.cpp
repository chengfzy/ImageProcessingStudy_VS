#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void calcHSHist(const Mat& src, Mat& hist, bool show = true) {
	// convert to hsv space
	Mat hsv;
	cvtColor(src, hsv, COLOR_BGR2HSV);
	
	// calc histogram and normalize
	calcHist(vector<Mat>{ src }, vector<int>{ 0, 1 }, noArray(), hist, vector<int>{ 30, 32 }, vector<float>{ 0, 180, 0, 256 }, false);
	//normalize(hist, hist, 0, 1, NORM_MINMAX);	
	normalize(hist, hist);

	if (show) {
		double maxVal{ 0 };
		minMaxLoc(hist, nullptr, &maxVal, nullptr, nullptr);
		const int scale{ 10 };
		Mat histImage(hist.rows* scale, hist.cols * scale, CV_8UC3);
		for (int h = 0; h < hist.rows; ++h) {
			for (int s = 0; s < hist.cols; ++s) {
				float binVal = hist.at<float>(h, s);
				int intensity = cvRound(binVal * 255 / maxVal);
				rectangle(histImage, Point(s * scale, h * scale), Point((s + 1) * scale - 1, (h + 1) * scale - 1), Scalar::all(intensity), CV_FILLED);
			}
		}
		static int index{ 1 };
		imshow("2D HS Histogram - " + to_string(index), histImage);
		++index;
	}
}

int main() {
	Mat src1 = imread("../../data/lena.png", IMREAD_UNCHANGED);
	if (src1.empty()) {
		cout << "couldn't read image." << endl;
		return 0;
	}

	// modify the brightness to obtain another image
	Mat src2;
	src1.convertTo(src2, src1.type(), 1.2, 0);
	imshow("Image1", src1);
	imshow("Image2", src2);
	
	// calulate 2D HS histogram
	Mat hist1, hist2;
	calcHSHist(src1, hist1, true);
	calcHSHist(src2, hist2, true);
	
	// histogram compare
	double result[4] = { 0 };
	result[0] = compareHist(hist1, hist2, HistCompMethods::HISTCMP_CORREL);
	result[1] = compareHist(hist1, hist2, HistCompMethods::HISTCMP_CHISQR_ALT);
	result[2] = compareHist(hist1, hist2, HistCompMethods::HISTCMP_INTERSECT);
	result[3] = compareHist(hist1, hist2, HistCompMethods::HISTCMP_BHATTACHARYYA);

	cout << "Correlation [-1, 1] = " << result[0] << endl;
	cout << "Alternative Chi - Square [+inf, 0] = " << result[1] << endl;
	cout << "Intersection [0, 1] = " << result[2] << endl;
	cout << "Bhattacharyya Distance [1, 0]= " << result[3] << endl;

	waitKey();
	return 0;
}