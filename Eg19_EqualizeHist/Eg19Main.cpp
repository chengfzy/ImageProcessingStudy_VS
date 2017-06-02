#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat srcImage = imread("..\\pic\\LowContrast.jpg");
	if (!srcImage.data)
		cout << "Read Image Error" << endl;

	cvtColor(srcImage, srcImage, COLOR_BGR2GRAY);
	imshow("Origin", srcImage);

	Mat dstImage;
	equalizeHist(srcImage, dstImage);
	imshow("Result", dstImage);

	waitKey();
	return 0;
}