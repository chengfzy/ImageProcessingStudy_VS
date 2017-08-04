#include <iostream>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

// read the color image for every pixel, and convert to gray image


// use at<Vec3b>(r,c)
void useAt(const Mat& src, Mat& dst)
{
	for (int r = 0; r < src.rows; ++r)
	{
		for (int c = 0; c < src.cols; ++c)
		{
			const cv::Vec3b& srcPix = src.at<Vec3b>(r, c);
			dst.at<uchar>(r, c) = static_cast<uchar>(0.114 * srcPix[0] + 0.587 * srcPix[1] + 0.299 * srcPix[2]);
		}
	}
}

// using iteration
void useIter(const Mat& src, Mat& dst)
{
	MatConstIterator_<Vec3b> itSrc = src.begin<Vec3b>(), itEnd = src.end<Vec3b>();
	for (MatIterator_<uchar> itDst = dst.begin<uchar>(); itSrc != itEnd; ++itSrc, ++itDst)
	{
		*itDst = static_cast<uchar>(0.114 * (*itSrc)[0] + 0.587 * (*itSrc)[1] + 0.299 * (*itSrc)[2]);
	}
}

// using c pointer []
void usePointer(const Mat& src, Mat& dst)
{
	int nSrcCols = src.cols * src.channels();
	int nDstCols = dst.cols * dst.channels();
	int nRows = src.rows;
	if (src.isContinuous() && dst.isContinuous())
	{
		nSrcCols *= nRows;
		nDstCols *= nRows;
		nRows = 1;
	}

	for (int r = 0; r < nRows; ++r)
	{
		const uchar* pSrc = src.ptr<uchar>(r);
		uchar* pDst = dst.ptr<uchar>(r);
		for (int c0 = 0, c1 = 0; c0 < nSrcCols; c0 += 3, ++c1)
		{
			const uchar& B = pSrc[c0];
			const uchar& G = pSrc[c0 + 1];
			const uchar& R = pSrc[c0 + 2];
			pDst[c1] = static_cast<uchar>(0.114 * B + 0.587 * G + 0.299 * R);
		}
	}
}

int main()
{
	Mat src = imread("../../../../Data/lawn.jpg", IMREAD_UNCHANGED);
	Mat dst(src.size(), CV_8UC1);
	
	const int N = 10;
	int64 start, end;

	// at<Vec3b>(r, c)
	for (int n = 0; n < N; ++n)
	{
		cout << "1. Use at<Vec3b>(r,c), Time = ";
		start = getCPUTickCount();
		useAt(src, dst);
		end = getCPUTickCount();
		cout << (end - start) / getNumberOfCPUs() / 1000 << " ms" << endl;
	}

	// iterator
	for (int n = 0; n < N; ++n)
	{
		cout << "2. Use iterator, Time = ";
		start = getCPUTickCount();
		useIter(src, dst);
		end = getCPUTickCount();
		cout << (end - start) / getNumberOfCPUs() / 1000 << " ms" << endl;
	}

	// c pointer
	for (int n = 0; n < N; ++n)
	{
		cout << "2. Use C Pointer[], Time = ";
		start = getCPUTickCount();
		usePointer(src, dst);
		end = getCPUTickCount();
		cout << (end - start) / getNumberOfCPUs() / 1000 << " ms" << endl;
	}


	system("pause");
} 