/********************************************************************
	创建日期:	2012/10/28
	文件名称: .\StereoVision\StereoVision\StereoMatch.h
	类名:	StereoMatch
	作者:	邹宇华
	
	功能:	双目匹配与三维重建类
*********************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#ifndef _STEREO_MATCH_H_
#define _STEREO_MATCH_H_

#pragma once

#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "FeatureMatch.h"
#include "StereoCalib.h"

using namespace std;

class StereoMatch
{
public:
	StereoMatch(void);
	virtual ~StereoMatch(void);

	/*----------------------------
	 * 功能 : 初始化内部变量，载入双目定标结果数据
	 *----------------------------
	 * 函数 : StereoMatch::init
	 * 访问 : public 
	 * 返回 : 1		成功
	 *		 0		读入校正参数失败
	 *		 -1		定标参数的图像尺寸与当前配置的图像尺寸不一致
	 *		 -2		校正方法不是 BOUGUET 方法
	 *		 -99	未知错误
	 *
	 * 参数 : imgWidth		[in]	图像宽度
	 * 参数 : imgHeight		[in]	图像高度
	 * 参数 : xmlFilePath	[in]	双目定标结果数据文件
	 */
	int init(int imgWidth, int imgHeight, const char* xmlFilePath, string _remapXml);

	/*----------------------------
	 * 功能 : 基于 BM 算法计算视差
	 *----------------------------
	 * 函数 : StereoMatch::bmMatch
	 * 访问 : public 
	 * 返回 : 0 - 失败，1 - 成功
	 *
	 * 参数 : frameLeft		[in]	左摄像机帧图
	 * 参数 : frameRight		[in]	右摄像机帧图
	 * 参数 : disparity		[out]	视差图
	 * 参数 : imageLeft		[out]	处理后的左视图，用于显示
	 * 参数 : imageRight		[out]	处理后的右视图，用于显示
	 */
	int bmMatch(cv::Mat& frameLeft, cv::Mat& frameRight, cv::Mat& disparity, cv::Mat& imageLeft, cv::Mat& imageRight);

	/*----------------------------
	 * 功能 : 基于 SGBM 算法计算视差
	 *----------------------------
	 * 函数 : StereoMatch::sgbmMatch
	 * 访问 : public 
	 * 返回 : 0 - 失败，1 - 成功
	 *
	 * 参数 : frameLeft		[in]	左摄像机帧图
	 * 参数 : frameRight		[in]	右摄像机帧图
	 * 参数 : disparity		[out]	视差图
	 * 参数 : imageLeft		[out]	处理后的左视图，用于显示
	 * 参数 : imageRight		[out]	处理后的右视图，用于显示
	 */
	int sgbmMatch(cv::Mat& frameLeft, cv::Mat& frameRight, cv::Mat& disparity, cv::Mat& imageLeft, cv::Mat& imageRight);

	int stereoFeatureMatch(cv::Mat& frameLeft, cv::Mat& frameRight, Mat& LeftMap1, Mat& LeftMap2, Mat& RightMap1, Mat& RightMap2, Mat& m_LeftH, Mat& m_RightH, Mat& h, int siftOrSurf);

	int uncalibratedSgbmMatch(cv::Mat& frameLeft, cv::Mat& frameRight, cv::Mat& disparity, cv::Mat& disparityL, cv::Mat& disparityR, cv::Mat& imageLeft, cv::Mat& imageRight, int writeOrRead = 0, int siftOrSurf=0);
	
	int uncalibratedBmMatch(cv::Mat& frameLeft, cv::Mat& frameRight, cv::Mat& disparityL, cv::Mat& disparityR, cv::Mat& imageLeft, cv::Mat& imageRight, int writeOrRead = 0, int siftOrSurf=0);
	
	int uncalibratedGCMatch(cv::Mat& frameLeft, cv::Mat& frameRight, cv::Mat& disparity, cv::Mat& imageLeft, cv::Mat& imageRight);

	/*----------------------------
	 * 功能 : 计算三维点云
	 *----------------------------
	 * 函数 : StereoMatch::getPointClouds
	 * 访问 : public 
	 * 返回 : 0 - 失败，1 - 成功
	 *
	 * 参数 : disparity		[in]	视差数据
	 * 参数 : pointClouds	[out]	三维点云
	 */
	int getPointClouds(cv::Mat& disparity, cv::Mat& pointClouds);

	/*----------------------------
	 * 功能 : 获取伪彩色视差图
	 *----------------------------
	 * 函数 : StereoMatch::getDisparityImage
	 * 访问 : public 
	 * 返回 : 0 - 失败，1 - 成功
	 *
	 * 参数 : disparity		[in]	原始视差数据
	 * 参数 : disparityImage	[out]	视差图像
	 * 参数 : isColor		[in]	是否采用伪彩色，默认为 true，设为 false 时返回灰度视差图
	 */
	int getDisparityImage(cv::Mat& disparity, cv::Mat& disparityImage, bool isColor = true);

	/*----------------------------
	 * 功能 : 保存三维点云到本地 txt 文件
	 *----------------------------
	 * 函数 : StereoMatch::savePointClouds
	 * 访问 : public 
	 * 返回 : void
	 *
	 * 参数 : pointClouds	[in]	三维点云数据
	 * 参数 : filename		[in]	文件路径
	 */
	void savePointClouds(cv::Mat& pointClouds, const char* filename);

	/***
	 *	公开变量
	 */
	cv::StereoBM	m_BM;				// 立体匹配 BM 方法
	cv::StereoSGBM	m_SGBM;				// 立体匹配 SGBM 方法
	double			m_FL;				// 左摄像机校正后的焦距值

private:

	/*----------------------------
	 * 功能 : 载入双目定标结果数据
	 *----------------------------
	 * 函数 : StereoMatch::loadCalibData
	 * 访问 : public 
	 * 返回 : 1		成功
	 *		 0		读入校正参数失败
	 *		 -1		定标参数的图像尺寸与当前配置的图像尺寸不一致
	 *		 -2		校正方法不是 BOUGUET 方法
	 *		 -99	未知错误
	 * 
	 * 参数 : xmlFilePath	[in]	双目定标结果数据文件
	 */
	int loadCalibData(const char* xmlFilePath);

	/***
	 *	私有变量
	 */
	bool	m_Calib_Data_Loaded;		// 是否成功载入定标参数
	cv::Mat m_Calib_Mat_Q;				// Q 矩阵
	cv::Mat m_Calib_Mat_Remap_X_L;		// 左视图畸变校正像素坐标映射矩阵 X
	cv::Mat m_Calib_Mat_Remap_Y_L;		// 左视图畸变校正像素坐标映射矩阵 Y
	cv::Mat m_Calib_Mat_Remap_X_R;		// 右视图畸变校正像素坐标映射矩阵 X
	cv::Mat m_Calib_Mat_Remap_Y_R;		// 右视图畸变校正像素坐标映射矩阵 Y
	cv::Mat m_Calib_Mat_Mask_Roi;		// 左视图校正后的有效区域
	cv::Rect m_Calib_Roi_L;				// 左视图校正后的有效区域矩形
	cv::Rect m_Calib_Roi_R;				// 右视图校正后的有效区域矩形

	int m_frameWidth;					// 帧宽
	int m_frameHeight;					// 帧高
	int m_numberOfDisparies;			// 视差变化范围

	cv::Mat	cameraMatrix_L;				// 摄像机矩阵
	cv::Mat	distortionCoefficients_L;	// 摄像机畸变参数
	cv::Mat	cameraMatrix_R;				// 摄像机矩阵
	cv::Mat	distortionCoefficients_R;	// 摄像机畸变参数
	cv::Mat FunMat;

	string xmlFile;
	string remapXml;
};

#endif