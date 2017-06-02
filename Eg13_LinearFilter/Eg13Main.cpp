#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//全局变量声明
Mat g_srcImage, g_dstBoxFilter, g_dstMeanBlur, g_dstGaussianBlur, g_dstMedianBlur, g_dstBilateralFilter;
int g_nBoxFilter = 3;
int g_nMeanBlur = 3;
int g_nGaussianBlur = 3;
int g_nMedianBlur = 10;
int g_nBilateralFilter = 10;

//方框滤波
static void OnBoxFilter(int, void*)
{
	boxFilter(g_srcImage, g_dstBoxFilter, -1, Size(g_nBoxFilter + 1, g_nBoxFilter + 1));
	imshow("Box Filter", g_dstBoxFilter);
}
//均值滤波
static void OnMeanBlur(int, void*)
{
	blur(g_srcImage, g_dstMeanBlur, Size(g_nMeanBlur + 1, g_nMeanBlur + 1));
	imshow("Mean Filter", g_dstMeanBlur);
}
//高斯滤波
static void OnGaussianBlur(int, void*)
{
	GaussianBlur(g_srcImage, g_dstGaussianBlur, Size(g_nGaussianBlur * 2 + 1, g_nGaussianBlur * 2 + 1), 0, 0);
	imshow("Gaussian Filter", g_dstGaussianBlur);
}
//中值滤波
static void OnMedianBlur(int, void*)
{
	medianBlur(g_srcImage, g_dstMeanBlur, g_nMedianBlur * 2 + 1);
	imshow("Median Blur", g_dstMedianBlur);
}
//双边滤波
static void OnBilateralFilter(int, void*)
{
	bilateralFilter(g_srcImage, g_dstBilateralFilter, g_nBilateralFilter, g_nBilateralFilter * 2, g_nBilateralFilter / 2);
	imshow("Bilateral Filter", g_dstBilateralFilter);
}

int main()
{
	g_srcImage = imread("..\\pic\\car.jpg");
	if (!g_srcImage.data)
	{
		cout << "Read Image Error" << endl;
		return -1;
	}

	//复制原图到三个Mat类型中
	g_dstBoxFilter = g_srcImage.clone();
	g_dstMeanBlur = g_srcImage.clone();
	g_dstGaussianBlur = g_srcImage.clone();
	g_dstMedianBlur = g_srcImage.clone();
	g_dstBilateralFilter = g_srcImage.clone();

	//方框滤波
	namedWindow("Box Filter");
	createTrackbar("kSize", "Box Filter", &g_nBoxFilter, 40, OnBoxFilter);
	OnBoxFilter(g_nBoxFilter, 0);
		
	//均值滤波
 	namedWindow("Mean Filter");
	createTrackbar("kSize", "Mean Filter", &g_nMeanBlur, 40, OnMeanBlur);
	OnMeanBlur(g_nMeanBlur, 0);

	//高斯滤波
	namedWindow("Gaussian Filter");
	createTrackbar("kSize", "Gaussian Filter", &g_nGaussianBlur, 40, OnGaussianBlur);
	OnGaussianBlur(g_nGaussianBlur, 0);
	
	//中值滤波
	namedWindow("Median Blur");
	createTrackbar("Parameter", "Median Blur", &g_nMedianBlur, 50, OnMedianBlur);
	OnMedianBlur(g_nMedianBlur, 0);

	//双边滤波
	namedWindow("Bilateral Filter");
	createTrackbar("Parameter", "Bilateral Filter", &g_nBilateralFilter, 50, OnBilateralFilter);
	OnBilateralFilter(g_nBilateralFilter, 0);

	waitKey();
	return 0;
}