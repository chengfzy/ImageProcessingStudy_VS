#include <iostream>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

using namespace std;
using namespace cv;


int main()
{
	//read and show source image
	Mat srcImage = imread("..\\pic\\cat.jpg");
	if (srcImage.empty())
		return -1;
	imshow("Origin Image", srcImage);

	//map
	Mat mapX(srcImage.size(), CV_32FC1), mapY(srcImage.size(), CV_32FC1);
	for (int r = 0; r < srcImage.rows; ++r)
	{
		for (int c = 0; c < srcImage.cols; ++c)
		{
			mapX.at<float>(r, c) = static_cast<float>(c);
			mapY.at<float>(r, c) = static_cast<float>(srcImage.rows - r);
		}
	}

	//remap
	Mat dstImage;
	remap(srcImage, dstImage, mapX, mapY, INTER_LINEAR);

	//show result
	imshow("Result", dstImage);



	waitKey();
	return 0;
}