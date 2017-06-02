#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

//创建Mat对象
void CreateMat()
{
	//1. 使用Mat()构造函数
	Mat m1(2, 2, CV_8UC3, Scalar(0, 0, 255));
	cout << "m1 = " << endl << m1 << endl << endl;

	//2. 在C++中通过构造函数进行初始化
	int sz[3] = { 2, 2, 2 };
	Mat m2(3, sz, CV_8UC3, Scalar::all(0));

	//3. 为已存在的IplImage指针创建信息头
	IplImage* img = cvLoadImage("..\\pic\\spring01.jpg", 1);
	// ERROR!!!
	//Mat m3(img);		//转换IplImage* ==> Mat

	//4. 利用Create()函数
	Mat m4;
	m4.create(4, 4, CV_8UC(2));
	cout << "m4 = " << endl << m4 << endl << endl;

	//5. 采用Matlab式的初始化方式
	Mat m5a = Mat::eye(4, 4, CV_64F);
	cout << "m5a = " << endl << m5a << endl << endl;
	Mat m5b = Mat::ones(2, 2, CV_32F);
	cout << "m5b = " << endl << m5b << endl << endl;
	Mat m5c = Mat::zeros(3, 3, CV_8UC1);
	cout << "m5c = " << endl << m5c << endl << endl;

	//6. 对小矩阵使用逗号分隔式初始化
	Mat m6 = (Mat_<double>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
	cout << "m6 = " << endl << m6 << endl << endl;

	//7.为已存在的对象创建新信息头
	Mat m7 = m6.row(1).clone();
	cout << "m7 = " << endl << m7 << endl << endl;

}

//格式化输出
void OutputMat()
{
	Mat r = Mat(10, 3, CV_8UC3);
	randu(r, Scalar::all(0), Scalar::all(255));

	//1. OpenCV默认风格
	cout << "r (默认风格) = " << endl << r << ";" << endl << endl;

	//2. Python风格
	cout << "r (Python风格) = " << endl << format(r, Formatter::FMT_PYTHON) << ";" << endl << endl;

	//3.逗号分隔CSV
	cout << "r (CVS风格) = " << endl << format(r, Formatter::FMT_CSV) << ";" << endl << endl;

	//4.Numpy风格
	cout << "r (Numpy风格) = " << format(r, Formatter::FMT_NUMPY) << ";" << endl << endl;

	//5. C风格
	cout << "r (C风格) = " << format(r, Formatter::FMT_C) << ";" << endl << endl;

	//6. Matlab风格
	cout << "r (Matlab风格) = " << format(r, Formatter::FMT_MATLAB) << ";" << endl << endl;

}

//定义输出其他常用数据结构
void OutputOtherStruct()
{
	//1. 定义和输出二维点
	Point2f p(6, 2);
	cout << "二维点 p = " << p << endl;

	//2.定义和输出三维点
	Point3f p3f(8, 2, 0);
	cout << "三维点 p3f = " << p3f << endl;

	//3. 定义和输出基本Mat的vector
	vector<float> v;
	v.push_back(3);
	v.push_back(5);
	v.push_back(7);
	cout << "基于Mat的vector v = " << Mat(v) << endl;

	//4. 定义和输出vector点
	vector<Point2f> points(20);
	for (size_t i = 0; i < points.size(); ++i)
		points[i] = Point2f(i * 5, i % 7);
	cout << "二维点向量 points = " << points << endl;
}

int main()
{
	CreateMat();
	OutputMat();
	OutputOtherStruct();

	system("pause");
	return 0;
}