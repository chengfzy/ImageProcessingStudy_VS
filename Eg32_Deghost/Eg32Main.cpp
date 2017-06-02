#include <opencv2\highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

const std::string szPicFolder = "../../Data/pic/";
const std::string szOutputFolder = "../../Data/result/";


template <typename T>
inline T lerp(const T x0, const T x1, const T alpha)
{
	return x0 * (T(1) - alpha) + x1 * alpha;
}

//some reference code for deghost
cv::Mat flattenLayersDeghostPreferBase(const cv::Mat & bottomLayer, const cv::Mat & topLayer)
{
	Mat mergedImage(bottomLayer.size(), CV_8UC4);
	for (int y = 0; y < bottomLayer.rows; ++y)
	{
		for (int x = 0; x < bottomLayer.cols; ++x)
		{
			Vec4b baseColor = bottomLayer.at<Vec4b>(y, x);
			Vec4b topColor = topLayer.at<Vec4b>(y, x);

			const float colorDiff = (std::abs(baseColor[0] - topColor[0]) + std::abs(baseColor[1] - topColor[1])
				+ std::abs(baseColor[2] - topColor[2])) / 255.0f;

			static const float kColorDiffCoef = 5.0f;
			static const float kSoftmaxSharpness = 5.0f;
			static const float kBaseLayerBias = 2.0f;
			const float deghostCoef = tanhf(colorDiff * kColorDiffCoef);
			const float alphaR = topColor[3] / 255.0f;
			const float alphaL = 1.0f - alphaR;
			const double expL = exp(kSoftmaxSharpness * alphaL * kBaseLayerBias);
			const double expR = exp(kSoftmaxSharpness * alphaR);
			const double sumExp = expL + expR + 0.00001;
			const float softmaxL = float(expL / sumExp);
			const float softmaxR = 1.0f - softmaxL;

			const unsigned char outAlpha = max(topColor[3], baseColor[3]);
			mergedImage.at<Vec4b>(y, x) = Vec4b(
				float(baseColor[0]) * lerp(alphaL, softmaxL, deghostCoef) + float(topColor[0]) * lerp(alphaR, softmaxR, deghostCoef),
				float(baseColor[1]) * lerp(alphaL, softmaxL, deghostCoef) + float(topColor[1]) * lerp(alphaR, softmaxR, deghostCoef),
				float(baseColor[2]) * lerp(alphaL, softmaxL, deghostCoef) + float(topColor[2]) * lerp(alphaR, softmaxR, deghostCoef),
				outAlpha);
		}
	}
	return mergedImage;
}


int main()
{
	Mat m1 = imread(szPicFolder + "overlap_3_L.png", IMREAD_UNCHANGED);
	Mat m2 = imread(szPicFolder + "overlap_3_R.png", IMREAD_UNCHANGED);
	const int width = m1.cols;

	Mat result = flattenLayersDeghostPreferBase(m1, m2);
	imwrite(szOutputFolder + "deghost.png", result);


	system("pause");
	return 0;
}