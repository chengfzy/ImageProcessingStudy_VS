#include <opencv2\opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//原图,原图的灰度图,目标图
Mat g_srcImage, g_srcGrayImage, g_dstImage;

//Canny边缘检测相关变量
Mat g_cannyDetectedEdges;
int g_cannyLowThreshold = 1;		//trackbar位置参数

//Sobel边缘检测相关变量
Mat g_sobelGradientX, g_sobelGradientY;
Mat g_sobelAbsGradientX, g_sobelAbsGradientY;
int g_sobelKernelSize = 1;			//trackbar位置参数


//基本的Canny边缘检测
void CannyBasic()
{
	Mat srcImage = imread("..\\pic\\Orange.jpg");
	Mat srcCopy = srcImage.clone();

	imshow("Origin", srcImage);
	//高阶canny用法:转化成灰度图,降噪,用canny,最后将得到的边缘作为掩码,拷贝原图到效果图上,得到彩色的边缘图
	Mat dstImage, edge, gray;
	//1.创建与src同类型和大小的矩阵
	dstImage.create(srcImage.size(), srcImage.type());
	//将原图转换成灰度图
	cvtColor(srcImage, gray, COLOR_RGB2GRAY);
	//先使用3X3内核降噪
	blur(gray, edge, Size(3, 3));
	//进行Canny算子
	Canny(edge, edge, 3, 9, 3);
	//将dstImage内所有元素设置为0
	dstImage = Scalar::all(0);
	//使用Canny算子输出的边缘图edge作为掩码,来将原图scrImage拷到目标图dstImage中
	srcCopy.copyTo(dstImage, edge);
	//显示效果图
	imshow("Canny Detect Result", dstImage);
}


//Canny边缘检测
static void OnCanny(int, void*)
{
	//先使用3X3内核降噪
	blur(g_srcGrayImage, g_cannyDetectedEdges, Size(3, 3));
	//进行Canny算子
	Canny(g_cannyDetectedEdges, g_cannyDetectedEdges, g_cannyLowThreshold, 3 * g_cannyLowThreshold, 3);
	//将dstImage内所有元素设置为0
	g_dstImage = Scalar::all(0);
	//使用Canny算子输出的边缘图edge作为掩码,来将原图scrImage拷到目标图dstImage中
	g_srcImage.copyTo(g_dstImage, g_cannyDetectedEdges);
	//显示效果图
	imshow("Canny Edge Detect", g_dstImage);
}

//Sobel边缘检测
static void OnSobel(int, void*)
{
	//求X方向梯度
	Sobel(g_srcImage, g_sobelGradientX, CV_16S, 1, 0, 2 * g_sobelKernelSize + 1, 1, 0, BORDER_DEFAULT);	
	convertScaleAbs(g_sobelGradientX, g_sobelAbsGradientX);	//计算绝对值,并将结果转换成8位

	//求Y方向梯度
	Sobel(g_srcImage, g_sobelGradientY, CV_16S, 0, 1, 2 * g_sobelKernelSize + 1, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(g_sobelGradientY, g_sobelAbsGradientY);	//计算绝对值,并将结果转换成8位

	//合并梯度
	addWeighted(g_sobelAbsGradientX, 0.5, g_sobelAbsGradientY, 0.5, 0, g_dstImage);

	//显示效果图
	imshow("Sobel Edge Detect", g_dstImage);
}

//Scharr边缘检测
void ScharrEdgeDetect()
{
	//Scharr滤波相关变量
	Mat gradientX, gradientY;
	Mat absGradientX, absGradientY;

	//求X方向梯度
	Scharr(g_srcImage, gradientX, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(gradientX, absGradientX);	//计算绝对值,并将结果转换成8位

	//求Y方向梯度
	Scharr(g_srcImage, gradientY, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(gradientY, absGradientY);	//计算绝对值,并将结果转换成8位

																//合并梯度
	addWeighted(absGradientX, 0.5, absGradientY, 0.5, 0, g_dstImage);

	//显示效果图
	imshow("Scharr Edge Detect", g_dstImage);
}

//Laplace变换
void LaplaceDetect()
{
	Mat src, srcGray, dst, absDst;

	//使用高斯滤波消除噪声
	GaussianBlur(g_srcImage, src, Size(3, 3), 0);
	//转换为灰度图
	cvtColor(src, srcGray, COLOR_RGB2GRAY);
	//使用Laplace函数
	Laplacian(srcGray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);
	//计算绝对值,并将结果转换成8位
	convertScaleAbs(dst, absDst);
	imshow("Laplace Detect", absDst);
}

int main()
{	
	//CannyBasic();

	g_srcImage = imread("..\\pic\\Orange.jpg");
	if (!g_srcImage.data)
		cout << "Read Image Error" << endl;
	
	imshow("Origin", g_srcImage);

	//创建与src同类型和大小的矩阵dst
	g_dstImage.create(g_srcImage.size(), g_srcImage.type());
	//将原图转换成灰度图
	cvtColor(g_srcImage, g_srcGrayImage, COLOR_RGB2GRAY);

	//Canny
	namedWindow("Canny Edge Detect");
	createTrackbar("Parameters: ", "Canny Edge Detect", &g_cannyLowThreshold, 120, OnCanny);
	OnCanny(0, 0);

	//Sobel
	namedWindow("Sobel Edge Detect");
	createTrackbar("Parameters: ", "Sobel Edge Detect", &g_sobelKernelSize, 3, OnSobel);
	OnSobel(0, 0);

	//Scharr
	ScharrEdgeDetect();
	
	//Laplace
	LaplaceDetect();


	waitKey();
	return 0;
}