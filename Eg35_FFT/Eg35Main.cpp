// using dft() and idft() to accelerate the computation of convolutions

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int main()
{
	Mat A = imread("../../../../data/catoon.jpg", IMREAD_GRAYSCALE);
	if (A.empty())
	{
		cout << "Cannot load images." << endl;
		return -1;
	}

	Size patchSize(100, 100);
	Point topleft(A.cols / 2, A.rows / 2);
	Rect roi(topleft.x, topleft.y, patchSize.width, patchSize.height);
	Mat B = A(roi);

	int dftM = getOptimalDFTSize(A.rows + B.rows - 1);
	int dftN = getOptimalDFTSize(A.cols + B.cols - 1);

	Mat dftA(dftM, dftN, CV_32F, Scalar(0));
	Mat dftB(dftM, dftN, CV_32F, Scalar(0));
	Mat dftAPart = dftA(Rect(0, 0, A.cols, A.rows));
	Mat dftBPart = dftB(Rect(0, 0, B.cols, B.rows));

	A.convertTo(dftAPart, dftAPart.type(), 1, -mean(A)[0]);
	B.convertTo(dftBPart, dftBPart.type(), 1, -mean(B)[0]);

	dft(dftA, dftA, 0, A.rows);
	dft(dftB, dftB, 0, B.rows);

	// set the last parameter to false to compute convolution instead of correlation
	mulSpectrums(dftA, dftB, dftA, 0, true);
	idft(dftA, dftA, DFT_SCALE, A.rows + B.rows - 1);

	Mat corr = dftA(Rect(0, 0, A.cols + B.cols - 1, A.rows + B.rows - 1));
	normalize(corr, corr, 0, 1, NormTypes::NORM_MINMAX, corr.type());
	pow(corr, 3, corr);

	B ^= Scalar::all(255);

	imshow("Image", A);
	imshow("Corrleation", corr);

	waitKey();
	return 0;
}