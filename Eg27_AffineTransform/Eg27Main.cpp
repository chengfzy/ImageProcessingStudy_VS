#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("..\\pic\\building.jpg");
	if (src.empty())
	{
		cout << "Cannot read image" << endl;
		return -1;
	}

	Point2f srcTri[] = {
		Point2f(0,0),				//top left
		Point2f(src.cols - 1, 0),	//top right
		Point2f(0,src.rows - 1)		//bottom left
	};
	Point2f dstTri[] = {
		Point2f(src.cols * 0.f, src.rows * 0.33f),		//top left
		Point2f(src.cols * 0.85f, src.rows * 0.25f),	//top right
		Point2f(src.cols * 0.15f, src.rows * 0.7f)		//dst bottom left
	};

	//compute affine matrix
	Mat warpMat = getAffineTransform(srcTri, dstTri);
	Mat dst;
	warpAffine(src, dst, warpMat, src.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar());

	//mark the triangle
	for (int i = 0; i < 3; ++i)
	{
		circle(src, srcTri[i], 5, Scalar(255, 0, 255), -1, LINE_AA);
		circle(dst, dstTri[i], 5, Scalar(255, 0, 255), -1, LINE_AA);
	}

	//show result
	imshow("Origin Image", src);
	imshow("Affine Transform Result", dst);
	waitKey();


	//compute rotation matrix
	Mat dst2;
	for (int frame = 0; ; ++frame)
	{
		Point2f center(src.cols * 0.5f, src.rows * 0.5f);
		double angle = frame * 3 % 360;
		double scale = (cos((angle - 60) * CV_PI / 180) + 1.05) * 0.8;
		Mat rotationMat = getRotationMatrix2D(center, angle, scale);

		warpAffine(src, dst2, rotationMat, src.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar());
		imshow("Rotated Image", dst2);
		if (waitKey(30) >= 0)
			break;
	}
	
	return 0;
}