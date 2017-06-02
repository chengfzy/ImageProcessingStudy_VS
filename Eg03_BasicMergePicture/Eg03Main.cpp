#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
	//1.载入和显示图像
	Mat girl = imread("..\\pic\\girl.jpg");
	namedWindow("动漫图");
	imshow("动漫图", girl);

	//2.初级图像混合
	Mat image = imread("..\\pic\\dota.jpg");
	Mat logo = imread("..\\pic\\dota_logo.jpg");
	namedWindow("原画图");
	imshow("原画图", image);
	namedWindow("Logo");
	imshow("Logo", logo);
	//定义一个Mat用于存放图像的ROI
	Mat imageROI;
	imageROI = image(Rect(800, 350, logo.cols, logo.rows));
	//将logo加到原图上
	addWeighted(imageROI, 0.5, logo, 0.3, 0, imageROI);
	//显示效果
	namedWindow("原画+logo图");
	imshow("原画+logo图", image);

	//3.输出到文件
	imwrite("..\\pic\\Eg03Out.jpg", image);
	
	waitKey(0);
	return 0;
}