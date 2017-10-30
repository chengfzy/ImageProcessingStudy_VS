#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

// SHT: standard hough transform
// MHT: multiscale hough transform
Mat sht(Mat& src)
{
	// canny edge dectect
	Mat edges;
	Canny(src, edges, 150, 200, 3, false);

	// SHT: standard hough transform
	vector<Vec2f> lines;
	HoughLines(edges, lines, 1, CV_PI / 180, 150, 0, 0);
	// draw result
	Mat dstImage = src.clone();
	for (size_t i = 0; i < lines.size(); ++i)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a* rho, y0 = b * rho;
		Point pt1, pt2;
		pt1.x = cvRound(x0 - 1000 * b);
		pt1.y = cvRound(y0 + 1000 * a);
		pt2.x = cvRound(x0 + 1000 * b);
		pt2.y = cvRound(y0 - 1000 * a);
		line(dstImage, pt1, pt2, Scalar(55, 100, 195), 1, LINE_AA);
	}

	return dstImage;
}


// PPHT: progressive probabilistic hough transform
Mat ppht(Mat& src)
{
	// canny edge dectect
	Mat edges;
	Canny(src, edges, 150, 200, 3, false);

	// PPHT
	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180, 80, 50, 2);
	// draw result
	Mat dstImage = src.clone();
	//Mat dstImage(edges.size(), CV_8UC3, Scalar(0));
	for (size_t i = 0; i < lines.size(); ++i)
	{
		Vec4i& l = lines[i];
		line(dstImage, Point(l[0], l[1]), Point(l[2],l[3]), Scalar(0, 0, 255), 1, LINE_AA);
	}

	return dstImage;
}


// Hough Circle
Mat houghCircle(Mat& src)
{
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, gray, Size(9, 9), 2, 2);

	vector<Vec3f> circles;
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1.5, 10, 200, 100, 0, 0);

	// draw result
	Mat dstImage = src.clone();
	for (size_t i = 0; i < circles.size(); ++i)
	{
		circle(dstImage, Point(cvRound(circles[i][0]), cvRound(circles[i][1])), cvRound(circles[i][2]), Scalar(0, 0, 255), 2, LINE_AA);
	}

	return dstImage;
}

int main()
{
	Mat src = imread("../../../data/catoon.jpg", IMREAD_UNCHANGED);
	if (src.empty())
	{
		cout << "Cannot load images." << endl;
		return -1;
	}

	// SHT
	Mat shtDst = sht(src);
	imshow("Standard Hough Line Transform", shtDst);

	// PPHT
	Mat pphtDst = ppht(src);
	imshow("Progressive Probabilistic Hough Transform", pphtDst);

	// hough circle
	src = imread("../../../data/Circle.jpg", IMREAD_UNCHANGED);
	Mat circleDst = houghCircle(src);
	imshow("Hough Circle Transform", circleDst);
	

	waitKey();
	return 0;
}