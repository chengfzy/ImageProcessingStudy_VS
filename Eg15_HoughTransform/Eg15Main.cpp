#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace std;
using namespace cv;

//全局声明变量
Mat g_srcImage, g_dstImage, g_midImage;	//原始图,目标图和中间图
vector<Vec4i> g_lines;					//存放得到的线段矢量集合
int g_nThreshould = 100;				//变量接收trackbar位置参数

//霍夫线变换
void HoughLineDetect()
{
	Mat srcImage = imread("..\\pic\\building.jpg");
	Mat midImage, standardDstImage, progressDstImage;		//临时变量和目标图
	
	//显示原始图
	imshow("Origin", srcImage);

	//进行边缘检测,转化成灰度图
	Canny(srcImage, midImage, 50, 200, 3);
	cvtColor(midImage, standardDstImage, CV_GRAY2BGR);
	//显示边缘检测后的图
	imshow("Canny Edge Detect", midImage);

	//进行标准霍夫线变换
	vector<Vec2f> standardLines;		//定义一个矢量结构lines用于存放得到的线段矢量集合
	HoughLines(midImage, standardLines, 1, CV_PI / 180, 150, 0, 0);
	//依次在图中绘制出每条线段
	for (size_t i = 0; i < standardLines.size(); ++i)
	{
		float rho = standardLines[i][0], theta = standardLines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * a);
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * a);
		line(standardDstImage, pt1, pt2, Scalar(55, 100, 195), 1, LINE_AA);
	}
	//显示效果图
	imshow("Standard Hough Lines Detect", standardDstImage);

	//进行霍夫概率变换
	cvtColor(midImage, progressDstImage, CV_GRAY2BGR);
	vector<Vec4i> progresLines;		//定义一个矢量结构lines用于存放得到的线段矢量集合
	HoughLinesP(midImage, progresLines, 1, CV_PI / 180, 80, 50, 10);
	//依次在图中绘制出每条线段
	for (size_t i = 0; i < progresLines.size(); ++i)
	{
		Vec4i l = progresLines[i];
		line(progressDstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186, 88, 255), 1, LINE_AA);
	}
	//显示效果图
	imshow("Progressive Hough Lines Detect", progressDstImage);
}

//累计概率霍夫变换
void HoughCircleDetect()
{
	Mat srcImage = imread("..\\pic\\Circle.jpg");
	Mat midImage, dstImage;		//临时变量和目标图
	imshow("Circle Origin", srcImage);

	//转化为灰度图并进行图像平滑
	cvtColor(srcImage, midImage, COLOR_RGB2GRAY);
	GaussianBlur(midImage, midImage, Size(9, 9), 2, 2);

	//进行霍夫圆变换
	vector<Vec3f> circles;
	HoughCircles(midImage, circles, HOUGH_GRADIENT, 2, 10, 200, 100, 0, 0);

	//依次在图中绘制出圆
	for (size_t i = 0; i < circles.size(); ++i)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		//绘制圆心
		circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//绘制圆轮廓
		circle(srcImage, center, radius, Scalar(155, 50, 265), 3, 8, 0);
	}

	//显示效果图
	imshow("Hough Circle Detect", srcImage);
}

static void OnHoughLines(int, void*)
{
	Mat midImage = g_midImage.clone();
	Mat dstImage = g_dstImage.clone();

	//调用HoughLinesP函数
}


int main()
{
	//HoughLineDetect();
	HoughCircleDetect();

	waitKey();
	return 0;
}