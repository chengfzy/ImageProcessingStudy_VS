#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
	const int r{ 100 };
	Mat src(4 * r, 4 * r, CV_8UC1, Scalar(0));

	// draw polygon
	vector<Point2f> vert{ Point(3 * r / 2, static_cast<int>(1.34 * r)),
		Point(1 * r, 2 * r),
		Point(3 * r / 2, static_cast<int>(2.866 * r)),
		Point(5 * r / 2, static_cast<int>(2.866 * r)),
		Point(3 * r, 2 * r),
		Point(5 * r / 2, static_cast<int>(1.347 * r))
	};
	for (int i = 0; i < vert.size(); ++i) {
		line(src, vert[i], vert[(i + 1) % vert.size()], Scalar(255), 3, 8);
	}
	imshow("Source", src);

	// find contours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Mat srcCopy = src.clone();
	findContours(srcCopy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	// polygon test
	Mat rawDist(src.size(), CV_32FC1);
	for (int r = 0; r < src.rows; ++r) {
		for (int c = 0; c < src.cols; ++c) {
			rawDist.at<float>(r, c) = static_cast<float>(pointPolygonTest(contours[0], Point2f(c, r), true));
		}
	}

	// find min and max
	double minVal{ 0 }, maxVal{ 0 };
	minMaxLoc(rawDist, &minVal, &maxVal);
	minVal = abs(minVal);
	maxVal = abs(maxVal);

	// draw test result
	Mat drawing(src.size(), CV_8UC3, Scalar(0));
	for (int r = 0; r < src.rows; ++r) {
		for (int c = 0; c < src.cols; ++c) {
			if (rawDist.at<float>(r, c) < 0)
				drawing.at<Vec3b>(r, c)[0] = saturate_cast<uchar>(255 - abs(rawDist.at<float>(r, c) * 255 / minVal));
			else if (rawDist.at<float>(r, c) > 0)
				drawing.at<Vec3b>(r, c)[2] = saturate_cast<uchar>(255 - rawDist.at<float>(r, c) * 255 / maxVal);
			else
				drawing.at<Vec3b>(r, c) = Vec3b(255, 255, 255);
		}
	}
	imshow("Distance", drawing);

	waitKey();
	return 0;
}