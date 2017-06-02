#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//图像腐蚀
Mat Erosion(const Mat& srcImage)
{
	//进行腐蚀操作
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat dstImage;
	erode(srcImage, dstImage, element);
	return dstImage;
}


// 图像模糊
Mat Blur(const Mat& srcImage)
{
	Mat dstImage;
	blur(srcImage, dstImage, Size(7, 7));
	return dstImage;
}


//Canny 边缘检测
Mat EdgeDetect(const Mat& srcImage)
{
	Mat grayImage, blurImage, edge;
	//1. 转换成灰度图像
	cvtColor(srcImage, grayImage, ColorConversionCodes::COLOR_RGB2GRAY);
	//2. 使用3*3内核来降噪
	blur(grayImage, blurImage, Size(3, 3));
	//3. 运用Canny算子
	Canny(srcImage, edge, 3, 9.3);
	return edge;
}


int main()
{
	cout << "version: " << CV_VERSION << endl;

	Mat srcImage = imread("..\\pic\\cat.jpg");		//read image
	imshow("原图", srcImage);						//show in window

	//1.腐蚀
	imshow("效果图-腐蚀", Erosion(srcImage));
	//2.图像模糊
	imshow("均值滤波", Blur(srcImage));
	//3.Canny图像边缘检测
	imshow("Canny边缘检测", EdgeDetect(srcImage));
	

	waitKey(0);							//wait and close
}