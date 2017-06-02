#include <opencv2\opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//声明全局函数
void colorReduce(Mat& inputImage, Mat& outputImage, int div, int method)
{
	switch (method)
	{
	case 0:	//用指针访问像素
	{
		//1. 用指针访问像素
		outputImage = inputImage.clone();
		int rowNumber = outputImage.rows;	//行数
		int colNumber = outputImage.cols * outputImage.channels();	//列数*通道数=每一行元素的个数

		//双重循环，遍历所有的像素值
		for (int i = 0; i < rowNumber; ++i)
		{
			uchar* data = outputImage.ptr<uchar>(i);	//获取第i行的首地址
			for (int j = 0; j < colNumber; ++j)
			{
				data[j] = data[j] / div * div + div / 2;
			}
		}
	}
	break;
	case 1:		//用迭代器操作像素
	{
		outputImage = inputImage.clone();		
		for (Mat_<Vec3b>::iterator it = outputImage.begin<Vec3b>(); it != outputImage.end<Vec3b>(); ++it)
		{
			(*it)[0] = (*it)[0] / div*div + div / 2;
			(*it)[1] = (*it)[1] / div*div + div / 2;
			(*it)[2] = (*it)[2] / div*div + div / 2;
		}
	}
	break;
	case 2:		//动态地址计算
	{
		outputImage = inputImage.clone();
		int rowNumber = outputImage.rows;	//行数
		int colNumber = outputImage.cols;	//列数

		//存取彩色图像像素
		for (int i = 0; i < rowNumber; ++i)
		{
			for (int j = 0; j < colNumber; ++j)
			{
				outputImage.at<Vec3b>(i, j)[0] = outputImage.at<Vec3b>(i, j)[0] / div * div + div / 2;		//Blue Channel
				outputImage.at<Vec3b>(i, j)[1] = outputImage.at<Vec3b>(i, j)[1] / div * div + div / 2;		//Green Channel
				outputImage.at<Vec3b>(i, j)[2] = outputImage.at<Vec3b>(i, j)[2] / div * div + div / 2;		//Red Channel
			}
		}
	}
	break;
	}
}


//test for color reduce
void testColorReduce(Mat& srcImage, Mat& dstImage, int method)
{
	//3.记录起始时间
	double time0 = static_cast<double>(getTickCount());

	//4.调用颜色空间缩减函数
	colorReduce(srcImage, dstImage, 32, method);

	//5.计算运行时间并输出
	time0 = (static_cast<double>(getTickCount()) - time0) / getTickFrequency();
	cout << "此方法运行时间为: " << time0 << "s" << endl;
}


int main()
{
	//1. 创建原始图并显示
	Mat srcImage = imread("..\\pic\\1.jpg");
	imshow("原始图像", srcImage);

	//2.按原始图的参数奈何徕创建效果图
	Mat dstImage;
	dstImage.create(srcImage.rows, srcImage.cols, srcImage.type());

	testColorReduce(srcImage, dstImage, 0);		//指针
	testColorReduce(srcImage, dstImage, 1);		//迭代器
	testColorReduce(srcImage, dstImage, 2);		//动态地址


	//6.显示效果图
	imshow("效果图", dstImage);

	waitKey(0);
	return 0;
}