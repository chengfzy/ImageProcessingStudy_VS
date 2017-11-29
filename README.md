# Image Processing Study
Image Processing Study

# Study Content
1. Eg01_HelloOpenCV
	OpenCV的基本应用:图像腐蚀、模糊、边缘检测
2. Eg02_SavePicture
	保存图像到文件
3. Eg03_BasicMergePicture
	载入图像，进行简单的混合，显示并输出到文件
4. Eg04_TrackBar
	控制滑动条来控制两幅图像的Alpha混合
5. Eg05_MouseOperator
	鼠标操作
6. Eg06_MatOperation
	Mat操作
7. Eg07_BasicStruct:
	基本的OpenCV数据结构
8. Eg08_BasicGraphDraw
	绘制简单图形
9. Eg09_ReadImagePixel
	访问图像中的像素，例为颜色空间缩减，即将256颜色的原图像变成64种颜色
10. Eg10_ROIOperation
	ROI操作，图像的叠加与混合
11. Eg11_MultiChannelBlending
	多通道图像混合Split,Merge

27. Eg27_AffineTransform
	仿射变换
28. Eg28_LogPolar
	图像变换,极坐标与直角坐标变换
29. Eg29_Remap
	图像变换,Remap
30. Eg30_ReadWriteVideo
	Video的读写操作
31. Eg31_AlphaBlending
	Alpha Blending，不使用addWeighted方法，识别出其轮廓，然后计算距离再做blending，有点问题
32. Eg32_Deghost
	去鬼影算法
33. Eg33_OpticalFlow
	光流, Lucas-Kanade, Gunnar Farneback方法求光流
	Ref: http://blog.csdn.net/on2way/article/details/48969649; http://blog.csdn.net/on2way/article/details/48969649
34. Eg34_OperatePixel
	访问逐个像素，使用三种方法将RGB转换成灰度图，并比较效率：at<Vec>(r, c): 68 ms, iterator: 56 ms, c pointer[]: 34 ms
35. Eg35_FFT
	DFT和IDFT算法
36. Eg36_HoughTransform
	Hough变换识别圆和直线
37. Eg37_Keypoints
	各种KeyPoint检测方法
	Harris-Shi-Tomasi: 效果尚可
	Simple Blob: 效果很差
	FAST(Features from Accelerated Segments Test): 效果还行，比Harris-Shi-Tomasi差点，挤在一堆的现象依然存在
	SIFT(Scale Invariant Feature Transform): 效果很好，并提供了128维特征描述矢量
	SURF(Speeded Up Robost Features): 效果不错，速度快
38. Eg38_KeypointMatch
	KeyPoint匹配方法, Brute Force, FLANN
39. Eg39_CameraCalibration
	相机校正: 参考OpenCV tourials file:///D:/Programs/opencv3.3.0/doc/html/d4/d94/tutorial_camera_calibration.html.	无图片，未实验
40. Eg40_VideoTracking
	Video Tracking, using CamShift algorithm, 参考OpenCV_dir\samples\cpp\camshiftdemo.cpp，无摄像头，不好测试，结果待定
41. Eg41_CalcHist
	计算图像的RGB直方图和2维HS直方图
42. Eg42_HistCompare
	直方图比较: 由于直方图的局限性，仅反映图像像素各灰度值的数量，不能反映纹理结构，因此存在很多误判。
	比如纹理结构相同，但明暗不同的图像，相似度会很低，纹理结构不同，但明暗相近的图像，相似度却很高。
43. Eg43_EMDHistCompare
	EMD(陆地移动距离)比较直方图，Ref: Learning OpenCV 3, P390, Example13-2
44. Eg44_BackProjection
	计算(BackProjection)反向投影，只使用了HSV中的H通道
	Ref: https://github.com/opencv/opencv/blob/master/samples/cpp/tutorial_code/Histograms_Matching/calcBackProject_Demo1.cpp
45. Eg45_ImprovedBackProjection
	计算(BackProjection)反向投影，使用了HSV中的HS通道，并使用FloodFill作为mask
	Ref: https://github.com/opencv/opencv/blob/master/samples/cpp/tutorial_code/Histograms_Matching/calcBackProject_Demo2.cpp
46. Eg46_TemplateMatch
	模板匹配，Ref: file:///D:/Programs/opencv3.3.1/doc/html/de/da9/tutorial_template_matching.html
47. Eg47_FindContour
	查找并绘制轮廓(Contour)，Ref: Learning OpenCV 3, P415, Example14-1
48. Eg48_ImprovedFindContour
	查找并绘制轮廓(Contour)，使用按键查看绘制轮廓步骤，Ref Learning OpenCV 3, P416, Example14-2
49. Eg49_FindContourUsingCanny
	使用Canny找到边缘后，再查找并绘制轮廓(Contour)，Ref: file:///D:/Programs/opencv3.3.1/doc/html/df/d0d/tutorial_find_contours.html
50. Eg50_DrawLabeledConnectedComponents
	将图片按区域连通，并用不同颜色绘制出来，Ref: Learning OpenCV 3, P419, Example14-3
51. Eg51_PolygonApprox
	多边形近似图形, Ref: file:///D:/Programs/opencv3.3.1/doc/html/d0/d2a/contours2_8cpp-example.html#a24
52. Eg52_BoundingRect
	针对识别出的图像轮廓(contours)，找到其多边形近似，边界矩形和圆近似等(approxPolyDP, boundingRect, minAreaRect, minEnclosingCircle, fitEllipse)
	Ref:
	https://github.com/opencv/opencv/blob/master/samples/cpp/tutorial_code/ShapeDescriptors/generalContours_demo1.cpp
	https://github.com/opencv/opencv/blob/master/samples/cpp/tutorial_code/ShapeDescriptors/generalContours_demo2.cpp
53. Eg53_PointPolygonTest
	检测点是否在多边形内，并计算出距多边形边界的值，并将值绘制出来.
	Ref: https://github.com/opencv/opencv/blob/master/samples/cpp/tutorial_code/ShapeDescriptors/pointPolygonTest_demo.cpp






# Project
1. Prj01_HazeRemoval
	图片去雾算法
	Ref: [1] Single Image Haze Removal using Dark Channel Prior, Kaiming He, Jian Sun, and Xiaoou Tang
2. Prj02_CameraCalibration
	相机校正，用同一相机在两个不同的位置拍摄两张照片，使用这两张图片来计算出相机的参数及运动矩阵R,t。未完成，优化时不收敛