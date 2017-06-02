// 重映射
#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat srcImage, dstImage;
	Mat mapX, mapY;

	srcImage = imread("..\\pic\\1.jpg");
	if (!srcImage.data)
		cout << "Read Image Error" << endl;

	//创建与原始图一样的效果图,x重映射图,y重映射图
	dstImage.create(srcImage.size(), srcImage.type());
	mapX.create(srcImage.size(), CV_32FC1);
	mapY.create(srcImage.size(), CV_32FC1);

	//遍历每一个像素点,改变mapX和mapY的值
	for (int j = 0; j < srcImage.rows; ++j)
		for (int i = 0; i < srcImage.cols; ++i)
		{
			mapX.at<float>(j, i) = static_cast<float>(i);
			mapY.at<float>(j, i) = static_cast<float>(srcImage.rows - j);
		}

	//进行重映射操作
	remap(srcImage, dstImage, mapX, mapY, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));

	//显示效果图
	imshow("Result", dstImage);

	waitKey();
	return 0;
}