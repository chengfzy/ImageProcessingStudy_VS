#include <opencv2\opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//利用感兴趣区域ROI实现图像叠加
bool ROI_AddImage()
{
	//1.读入图像
	Mat srcImage = imread("..\\pic\\dota.jpg");
	Mat logoImage = imread("..\\pic\\dota_logo.jpg");
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


	//2.定义一个Mat类型并设定ROI
	Mat imageROI = srcImage(Rect(200, 250, logoImage.cols, logoImage.rows));

	//3.加载掩摸，必须是灰度图
	Mat mask = imread("..\\pic\\dota_logo.jpg", 0);

	//4.将掩膜复制到ROI
	logoImage.copyTo(imageROI, mask);

	//5.显示结果
	imshow("利用ROI实现图像叠加", srcImage);

	return true;
}

//图像线性混合addWeighted()
void LinearBlending()
{
	//1.读取图像
	Mat srcImage1 = imread("..\\pic\\mogu.jpg");
	Mat srcImage2 = imread("..\\pic\\rain.jpg");
	Mat dstImage;

	//2.图像混合加权
	double alphaValue = 0.5;
	double betaValue = 1.0 - alphaValue;
	addWeighted(srcImage1, alphaValue, srcImage2, betaValue, 0, dstImage);

	//3.显示
	imshow("原图", srcImage1);
	imshow("线性混合", dstImage);
}

//感兴趣区域ROI，实现自定义区域的线性混合
bool ROI_LinearBlending()
{
	//1.读入图像
	Mat srcImage = imread("..\\pic\\dota.jpg");
	Mat logoImage = imread("..\\pic\\dota_logo.jpg");
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

	//2.定义一个Mat类型并给其设定ROI区域
	Mat imageROI = srcImage(Range(250, 250 + logoImage.rows), Range(200, 200 + logoImage.cols));

	//3.将logo加到原图上
	addWeighted(imageROI, 0.5, logoImage, 0.3, 0, imageROI);
	

	//4.显示结果
	imshow("ROI线性图像混合", srcImage);
	return true;
}

int main()
{
	ROI_AddImage();
	LinearBlending();

	ROI_LinearBlending();

	waitKey(0);
	return 0;
}