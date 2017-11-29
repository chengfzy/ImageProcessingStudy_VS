#include <iostream>
#include <math.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

const int kWidth{ 500 };
int levels{ 3 };
vector<vector<Point>> contours;
vector<Vec4i> hierarchy;

void onTrackbar(int, void*) {
	Mat contoursImg(kWidth, kWidth, CV_8UC3, Scalar(0));
	int level_ = levels - 3;
	drawContours(contoursImg, contours, level_ <= 0 ? 3 : -1, Scalar(128, 255, 255), 3, LINE_AA, hierarchy, std::abs(level_));
	imshow("Contours", contoursImg);
}

int main() {
	// draw 6 faces 
	const Scalar kWhite(255);
	const Scalar kBlack(0);
	Mat img(kWidth, kWidth, CV_8UC1, Scalar(0));
	for (int i = 0; i < 6; ++i) {
		int dx = (i % 2) * 250 - 30;
		int dy = (i / 2) * 150;
		if (0 == i) {
			for (int j = 0; j <= 10; ++j) {
				double angle = (j + 5) * CV_PI / 21;
				line(img, Point(cvRound(dx + 100 + j * 10 - 80 * cos(angle)), cvRound(dy + 100 - 90 * sin(angle))),
					Point(cvRound(dx + 100 + j * 10 - 30 * cos(angle)), cvRound(dy + 100 - 30 * sin(angle))), kWhite, 1, LINE_8, 0);
			}
		}

		ellipse(img, Point(dx + 150, dy + 100), Size(100, 70), 0, 0, 360, kWhite, -1, LINE_8, 0);
		ellipse(img, Point(dx + 115, dy + 70), Size(30, 20), 0, 0, 360, kBlack, -1, LINE_8, 0);
		ellipse(img, Point(dx + 185, dy + 70), Size(30, 20), 0, 0, 360, kBlack, -1, LINE_8, 0);
		ellipse(img, Point(dx + 115, dy + 70), Size(15, 15), 0, 0, 360, kWhite, -1, LINE_8, 0);
		ellipse(img, Point(dx + 185, dy + 70), Size(15, 15), 0, 0, 360, kWhite, -1, LINE_8, 0);
		ellipse(img, Point(dx + 115, dy + 70), Size(5, 5), 0, 0, 360, kBlack, -1, LINE_8, 0);
		ellipse(img, Point(dx + 185, dy + 70), Size(5, 5), 0, 0, 360, kBlack, -1, LINE_8, 0);
		ellipse(img, Point(dx + 150, dy + 100), Size(10, 5), 0, 0, 360, kBlack, -1, LINE_8, 0);
		ellipse(img, Point(dx + 150, dy + 150), Size(40, 10), 0, 0, 360, kBlack, -1, LINE_8, 0);
		ellipse(img, Point(dx + 27, dy + 100), Size(20, 35), 0, 0, 360, kWhite, -1, LINE_8, 0);
		ellipse(img, Point(dx + 273, dy + 100), Size(20, 35), 0, 0, 360, kWhite, -1, LINE_8, 0);
	}
	imshow("Image", img);

	// extract the contours
	vector<vector<Point>> contours0;
	findContours(img, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	contours.resize(contours0.size());
	for (size_t n = 0; n < contours0.size(); ++n) {
		approxPolyDP(Mat(contours0[n]), contours[n], 3, true);
	}
	namedWindow("Contours", WINDOW_AUTOSIZE);
	createTrackbar("Level + 3", "Contours", &levels, 7, onTrackbar);
	onTrackbar(0, nullptr);

	waitKey();
	return 0;
}