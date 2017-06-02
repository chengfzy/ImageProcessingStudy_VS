#include <iostream>
#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	//read and show source image
	Mat srcImage = imread("..\\pic\\clock.png");
	if (srcImage.empty())
		return -1;
	imshow("Origin Image", srcImage);


	Mat linearPolarImage, logPolarImage, recoverLinearPolarImage, recoverdLogPolarImage;
	//parameters
	Point2f center((float)srcImage.cols / 2, (float)srcImage.rows / 2);
	double radius = srcImage.cols / 2.0;
	double M = srcImage.cols / log(radius);

	//descartes -> polar
	linearPolar(srcImage, linearPolarImage, center, radius, INTER_LINEAR + WARP_FILL_OUTLIERS);
	logPolar(srcImage, logPolarImage, center, M, INTER_LINEAR + WARP_FILL_OUTLIERS);
	//polar->descartes
	linearPolar(linearPolarImage, recoverLinearPolarImage, center, radius, INTER_LINEAR + WARP_INVERSE_MAP + WARP_FILL_OUTLIERS);
	logPolar(logPolarImage, recoverdLogPolarImage, center, M, INTER_LINEAR + WARP_INVERSE_MAP + WARP_FILL_OUTLIERS);

	//show result
	imshow("Linear Polar Image", linearPolarImage);
	imshow("LogPolar Image", logPolarImage);
	imshow("Recoverd Linear Polar Image", recoverLinearPolarImage);
	imshow("Recoverd Log Polar Image", recoverdLogPolarImage);

	waitKey();
	return 0;
}