#include <opencv2/opencv.hpp>
using namespace cv;

const int gWindowWidth = 600;			//窗口大小
const String gAtomWindowName = "原子图";	//窗口标题
const String gRookWindowName = "组合图";

//绘制椭圆
void DrawEllipse(Mat img, double angle)
{
	int thickness = 2;
	int lineType = 8;
	ellipse(img, Point(gWindowWidth / 2, gWindowWidth / 2), Size(gWindowWidth / 4, gWindowWidth / 6),
		angle, 0, 360, Scalar(255, 129, 0), thickness, lineType);
}

//绘制实心圆
void DrawFilledCircle(Mat img, Point center)
{
	int thickness = -1;
	int lineType = 8;
	circle(img, center, gWindowWidth / 32, Scalar(0, 255, 255), thickness, lineType);
}

//绘制凹多边形
void DrawPolygon(Mat img)
{
	int lineType = 8;

	//创建一些点
	Point rookPoints[1][20];
	rookPoints[0][0] = Point(gWindowWidth / 4, 7 * gWindowWidth / 8);
	rookPoints[0][1] = Point(3 * gWindowWidth / 4, 7 * gWindowWidth / 8);
	rookPoints[0][2] = Point(3 * gWindowWidth / 4, 13 * gWindowWidth / 16);
	rookPoints[0][3] = Point(11 * gWindowWidth / 16, 13 * gWindowWidth / 16);
	rookPoints[0][4] = Point(19 * gWindowWidth / 32, 3 * gWindowWidth / 8);
	rookPoints[0][5] = Point(3 * gWindowWidth / 4, 3 * gWindowWidth / 8);
	rookPoints[0][6] = Point(3 * gWindowWidth / 4, gWindowWidth / 8);
	rookPoints[0][7] = Point(26 * gWindowWidth / 40, gWindowWidth / 8);
	rookPoints[0][8] = Point(26 * gWindowWidth / 40, gWindowWidth / 4);
	rookPoints[0][9] = Point(22 * gWindowWidth / 40, gWindowWidth / 4);
	rookPoints[0][10] = Point(22 * gWindowWidth / 40, gWindowWidth / 8);
	rookPoints[0][11] = Point(18 * gWindowWidth / 40, gWindowWidth / 8);
	rookPoints[0][12] = Point(18 * gWindowWidth / 40, gWindowWidth / 4);
	rookPoints[0][13] = Point(14 * gWindowWidth / 40, gWindowWidth / 4);
	rookPoints[0][14] = Point(14 * gWindowWidth / 40, gWindowWidth / 8);
	rookPoints[0][15] = Point(gWindowWidth / 4, gWindowWidth / 8);
	rookPoints[0][16] = Point(gWindowWidth / 4, 3 * gWindowWidth / 8);
	rookPoints[0][17] = Point(13 * gWindowWidth / 32, 3 * gWindowWidth / 8);
	rookPoints[0][18] = Point(5 * gWindowWidth / 16, 13 * gWindowWidth / 16);
	rookPoints[0][19] = Point(gWindowWidth / 4, 13 * gWindowWidth / 16);

	const Point* ppt[1] = { rookPoints[0] };
	int npt[] = { 20 };
	fillPoly(img, ppt, npt, 1, Scalar(255, 255, 255), lineType);
}

//绘制线
void DrawLine(Mat img, Point start, Point end)
{
	int thickness = 2;
	int lineType = 8;
	line(img, start, end, Scalar(0, 0, 0), thickness, lineType);
}

int main()
{
	//创建空白的Mat图像
	Mat atomImage = Mat::zeros(gWindowWidth, gWindowWidth, CV_8UC3);
	Mat rookImage = Mat::zeros(gWindowWidth, gWindowWidth, CV_8UC3);

	//1. 绘制化学中的原子示例图
	//1.1 先绘制出椭圆
	DrawEllipse(atomImage, 90);
	DrawEllipse(atomImage, 0);
	DrawEllipse(atomImage, 45);
	DrawEllipse(atomImage, -45);
	//1.2 绘制圆心
	DrawFilledCircle(atomImage, Point(gWindowWidth / 2, gWindowWidth / 2));
	
	//2.绘制组合图
	//2.1. 先绘制出椭圆
	DrawPolygon(rookImage);
	//2.2. 绘制矩形
	rectangle(rookImage, Point(0, 7 * gWindowWidth / 8), Point(gWindowWidth, gWindowWidth), Scalar(0, 255, 255), -1, 8);
	//2.3. 绘制一些线段
	DrawLine(rookImage, Point(0, 15 * gWindowWidth / 16), Point(gWindowWidth, 15 * gWindowWidth / 16));
	DrawLine(rookImage, Point(gWindowWidth / 4, 7 * gWindowWidth / 8), Point(gWindowWidth / 4, gWindowWidth));
	DrawLine(rookImage, Point(gWindowWidth / 2, 7 * gWindowWidth / 8), Point(gWindowWidth / 2, gWindowWidth));
	DrawLine(rookImage, Point(3 * gWindowWidth / 4, 7 * gWindowWidth / 8), Point(3 * gWindowWidth / 4, gWindowWidth));

	//3. 显示绘制出的图像
	imshow(gAtomWindowName, atomImage);
	moveWindow(gAtomWindowName, 0, 200);
	imshow(gRookWindowName, rookImage);
	moveWindow(gRookWindowName, gWindowWidth, 200);

	waitKey(0);
	return 0;
}