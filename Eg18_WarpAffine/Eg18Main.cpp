#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	//定义两组点,代表两个三角形
	Point2f srcTriangle[3];
	Point2f dstTriangle[3];
	//定义一些Mat变量
	Mat rotMat(2, 3, CV_32FC1);
	Mat warpMat(2, 3, CV_32FC1);
	Mat srcImage, dstWarpImage, dstWarpRotateImage;

	//加载源图像并作一些初始化
	srcImage = imread("..\\pic\\1.jpg");
	if (!srcImage.data)
		cout << "Read Image Error" << endl;
	imshow("Origin", srcImage);

	dstWarpImage = Mat::zeros(srcImage.size(), srcImage.type());

	//设置源图像和目标图像上的三组点以计算仿射变换
	srcTriangle[0] = Point2f(0, 0);
	srcTriangle[1] = Point2f(static_cast<float>(srcImage.cols - 1), 0);
	srcTriangle[2] = Point2f(0, static_cast<float>(srcImage.rows - 1));
	dstTriangle[0] = Point2f(static_cast<float>(srcImage.cols*0.0), static_cast<float>(srcImage.rows*0.33));
	dstTriangle[1] = Point2f(static_cast<float>(srcImage.cols*0.65), static_cast<float>(srcImage.rows*0.35));
	dstTriangle[2] = Point2f(static_cast<float>(srcImage.cols*0.15), static_cast<float>(srcImage.rows*0.6));

	//求得仿射变换
	warpMat = getAffineTransform(srcTriangle, dstTriangle);

	//对源图像应用刚刚求得的仿射变换
	warpAffine(srcImage, dstWarpImage, warpMat, dstWarpImage.size());
	imshow("Warp", dstWarpImage);

	//对图像进行缩放后再旋转:计算绕图像中点顺时针旋转50度,缩放因子为0.6的旋转矩阵
	Point center = Point(dstWarpImage.cols / 2, dstWarpImage.rows / 2);
	double angle = -30.0;
	double scale = 0.6;
	//通过上面的旋转细节信息求得旋转矩阵
	rotMat = getRotationMatrix2D(center, angle, scale);
	//旋转已缩放后的图像
	warpAffine(dstWarpImage, dstWarpRotateImage, rotMat, dstWarpImage.size());
	imshow("Warp and Rotation", dstWarpRotateImage);

	waitKey();
	return 0;
}