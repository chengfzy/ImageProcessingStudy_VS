#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//多通道混合
bool MultiChannelBlending()
{
	//1.读入图像
	Mat srcImage = imread("..\\pic\\dota.jpg");
	Mat logoImage = imread("..\\pic\\dota_logo.jpg", 0);
	if (!srcImage.data)
	{
		cout << "读取srcImage错误!" << endl;
		return false;
	}
	if (!logoImage.data)
	{
		cout << "读取logoImage错误!" << endl;
		return false;
	}

	//2.把一个3通道图像转换成3个单通道图像
	vector<Mat> channels;
	split(srcImage, channels);

	//3.将原图的蓝色通道引用返回给imageBlueChannel
	Mat imageBlueChannel = channels.at(0);

	//4.将蓝色通道右下方区域与logo图进行加权操作，得到混合结果
	addWeighted(imageBlueChannel(Rect(500, 250, logoImage.cols, logoImage.rows)), 1.0, logoImage,
		0.5, 0, imageBlueChannel(Rect(500, 250, logoImage.cols, logoImage.rows)));

	//5.将三个通道重新合成一个三通道
	merge(channels, srcImage);

	//6.显示效果图
	imshow("游戏原画+logo蓝色通道", srcImage);

	return true;
}


int main()
{
	MultiChannelBlending();

	waitKey(0);
	return 0;
}