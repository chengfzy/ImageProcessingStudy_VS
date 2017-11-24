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
	vector<string> files{ "../../data/HandIndoorColor.jpg", "../../data/HandOutdoorColor.jpg",
		"../../data/HandOutdoorSunColor.jpg", "../../data/orange01.jpg" };
	const int N = files.size() + 1;
	vector<Mat> src(N);

	// load first images
	Mat temp = imread(files[0], IMREAD_UNCHANGED);
	if (temp.empty()) {
		cerr << "couldn't read image 0." << endl;
		return -1;
	}

	// parse the first image into two image halves divided halfway on Y
	const int width = temp.cols;
	const int height = temp.rows;
	src[0].create(height / 2, width, CV_8UC3);
	src[1].create(height / 2, width, CV_8UC3);
	temp.rowRange(0, height / 2).copyTo(src[0]);
	temp.rowRange(height / 2, height).copyTo(src[1]);

	// load another 3 images
	for (int i = 2; i < N; ++i) {
		src[i] = imread(files[i - 1], IMREAD_UNCHANGED);
		if (src[i].empty()) {
			cerr << "could not read image " << i << endl;
			return -1;
		}
	}

	// compute HSV histogram
	vector<Mat> hsv(N), hist(N), histImg(N);
	const int hBins{ 8 }, sBins{ 8 };
	const int scale{ 10 };
	for (int i = 0; i < N; ++i) {
		cvtColor(src[i], hsv[i], COLOR_BGR2HSV);
		calcHist(vector<Mat>{ src[i] }, vector<int>{ 0, 1 }, noArray(), hist[i], vector<int>{ hBins, sBins }, vector<float>{ 0, 180, 0, 255 }, true);
		normalize(hist[i], hist[i], 0, 255, NORM_MINMAX);

		// draw histogram image
		histImg[i].create(hBins * scale, sBins * scale, CV_8UC3);
		histImg[i].setTo(0);
		for (int h = 0; h < hBins; ++h) {
			for (int s = 0; s < sBins; ++s) {
				float val = hist[i].at<float>(h, s);
				rectangle(histImg[i], Rect(h * scale, s * scale, scale, scale), Scalar::all(val), CV_FILLED);
			}
		}

		imshow("Image " + to_string(i), src[i]);
		imshow("HS Histogram " + to_string(i), histImg[i]);
	}
	
	// compare the histogram src0 vs 1, 2, 3, 4
	cout << "Histogram Comparison vs [0]" << endl;
	cout << "ID \tCORREL \t\tCHISQR \t\tINTERSECT \tBHATTACHARYYA \tCHISQR_ALT" << endl;
	for (int i = 1; i < N; ++i) {
		cout << i << "\t";
		cout << compareHist(hist[0], hist[1], HistCompMethods::HISTCMP_CORREL) << " \t";
		cout << compareHist(hist[0], hist[1], HistCompMethods::HISTCMP_CHISQR) << " \t";
		cout << compareHist(hist[0], hist[1], HistCompMethods::HISTCMP_INTERSECT) << " \t";
		cout << compareHist(hist[0], hist[1], HistCompMethods::HISTCMP_BHATTACHARYYA) << " \t";
		cout << compareHist(hist[0], hist[1], HistCompMethods::HISTCMP_CHISQR_ALT) << " \t";
		cout << endl;
	}

	// EMD comparison
	cout << "\nEMD Comparison" << endl;
	vector<Mat> sig(N);
	for (int i = 0; i < N; ++i) {
		vector<Vec3f> sigv;
		normalize(hist[i], hist[i], 1, 0, NORM_L1);
		for (int h = 0; h < hBins; ++h) {
			for (int s = 0; s < sBins; ++s) {
				float val = hist[i].at<float>(h, s);
				if (val != 0) {
					sigv.emplace_back(Vec3f(val, (float)h, (float)s));
				}
			}
		}

		// make N*3 32FC1 matrix, where N is the number of nonzero histogram bins
		sig[i] = Mat(sigv).clone().reshape(1);
		if (i > 0) {
			cout << "[0] vs [" << i << "] = " << EMD(sig[0], sig[i], DIST_L2) << endl;
		}
	}

	waitKey();
	return 0;
}