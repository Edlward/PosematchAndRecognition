#include "keras_model.h"

#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <direct.h>
#include <io.h>
#include <stdio.h>

#include "StereoMatch.h"
#include "filesTool.h"

using namespace std;
using namespace keras;

#define MATCH 0
#define RECOGNITION 1
#define MODE RECOGNITION				//����ģʽ,MATCHΪѡ��ȥƥ���ȡ���ͼ��RECOGNITIONΪʶ��

string leftPath = "left";
string rightPath = "right";
string depthPath = "depth";

string xml_filename = "calib_paras1.xml";
string remap_filename = "remap.xml";
string path = "ѵ��";				//����·��
string path1 = "ѵ��\\05����\\left";
string path2 = "ѵ��\\05����\\right";

void F_Gray2Color(Mat gray_mat, Mat& color_mat)
{
	vector<Mat> channels(3);
	Mat color(gray_mat.size(), CV_8UC3);
	// �������ɫͨ��������
	for (int i = 0; i < gray_mat.rows; i++)
	{
		for (int j = 0; j < gray_mat.cols; j++)
		{
			if ((int)gray_mat.at<uchar>(i, j) != 0)
			{
				color.at<cv::Vec3b>(i, j)[0] = 255 - (int)gray_mat.at<uchar>(i, j);
				color.at<cv::Vec3b>(i, j)[2] = (int)gray_mat.at<uchar>(i, j);
				if (gray_mat.at<uchar>(i, j) < 128)
				{
					color.at<cv::Vec3b>(i, j)[1] = (int)gray_mat.at<uchar>(i, j);
				}
				else
				{
					color.at<cv::Vec3b>(i, j)[1] = 255 - (int)gray_mat.at<uchar>(i, j);
				}
			}
			else
			{
				color.at<cv::Vec3b>(i, j)[0] = 0;
				color.at<cv::Vec3b>(i, j)[1] = 0;
				color.at<cv::Vec3b>(i, j)[2] = 0;
			}
		}
	}
	color.copyTo(color_mat);
}

int getRightAndLeftDisp(string leftFile, string rightFile, Mat& dispColor, Mat& dispLColor, Mat& dispRColor,int siftOrSurf)
{

	Mat leftImage = imread(leftFile, CV_LOAD_IMAGE_GRAYSCALE);
	Mat rightImage = imread(rightFile, CV_LOAD_IMAGE_GRAYSCALE);

	if (leftImage.empty())
	{
		cout << "leftImage empty" << endl;
		return 1;
	}
	if (rightImage.empty())
	{
		cout << "rightImage empty" << endl;
		return 1;
	}

	StereoMatch m_stereoMatcher;
	m_stereoMatcher.init(leftImage.cols, leftImage.rows, xml_filename.c_str(), remap_filename);
	// ��ʼ����ͼ���Ӳ�
	m_stereoMatcher.m_SGBM.disp12MaxDiff = -1;
	m_stereoMatcher.m_SGBM.preFilterCap = 63;
	m_stereoMatcher.m_SGBM.SADWindowSize = 3;
	m_stereoMatcher.m_SGBM.P1 = 8 * leftImage.channels() * m_stereoMatcher.m_SGBM.SADWindowSize * m_stereoMatcher.m_SGBM.SADWindowSize;
	m_stereoMatcher.m_SGBM.P2 = 32 * leftImage.channels() * m_stereoMatcher.m_SGBM.SADWindowSize * m_stereoMatcher.m_SGBM.SADWindowSize;
	m_stereoMatcher.m_SGBM.minDisparity = 0;
	m_stereoMatcher.m_SGBM.numberOfDisparities = 256;
	m_stereoMatcher.m_SGBM.uniquenessRatio = 50;
	m_stereoMatcher.m_SGBM.speckleWindowSize = 100;
	m_stereoMatcher.m_SGBM.speckleRange = 32;
	m_stereoMatcher.m_SGBM.fullDP = true;

	Mat img1, img2, img1p, img2p, disp, dispL, dispR, disp8u, dispL8u, dispR8u, pointCloud;
	m_stereoMatcher.uncalibratedSgbmMatch(leftImage, rightImage, disp, dispL, dispR, img1p, img2p, 0, siftOrSurf);
	m_stereoMatcher.getDisparityImage(dispL, dispL8u, false);
	m_stereoMatcher.getDisparityImage(dispR, dispR8u, false);
	m_stereoMatcher.getDisparityImage(disp, disp8u, false);

	F_Gray2Color(dispL8u, dispLColor);
	F_Gray2Color(dispR8u, dispRColor);
	F_Gray2Color(disp8u, dispColor);
	return 0;

	//resize(dispL8u, dispL8u, Size(640, 480));
	//resize(dispR8u, dispR8u, Size(640, 480));
	//resize(img1p, img1p, Size(640, 480));
	//resize(img2p, img2p, Size(640, 480));
	//imshow("dispL", dispL8u);
	//imshow("dispR", dispR8u);
	//imshow("img1p", img1p);
	//imshow("img2p", img2p);
	//waitKey(0);
}
void findName(string src,string& dst)
{
  int index1,index2;  
  index1 = src.find_last_of('\\');
  index2 = src.find_last_of('.');
  int len = index2 - index1-1;
  dst = src.substr(index1 + 1, len);
}
string labels[10] = { "˫��ƽ��", "����", "����", "���", "���ֻ���", "������չ", "����", "ſ��", "��绰", "��������" };
//��������
/*
	ģʽһ:
	ƥ�䲢�������ͼ
	���ո����һ��ƥ�䣬��q�˳�
	ģʽ��:
	ʶ��
	�ȼ���ѵ���õ�ģ�ͺͲ������������ȡһ��ͼƬ�����ÿ��label�ĸ��ʺ����շ�������
	���ո����һ�ŷ��࣬��q�˳�
*/
int main(int argc, char *argv[])
{
#if MODE==MATCH
	string filesPath;
	vector<string> files;
	getJustCurrentDir(path, files);
	Mat leftDisp, rightDisp,disp;
	for (int i = 0; i < files.size(); i++)
	{
		vector<string> leftFilesName, rightFilesName;
		filesPath = path + "\\" + files[i];
		getFilesAllName(filesPath + "\\" + leftPath, leftFilesName);
		getFilesAllName(filesPath + "\\" + rightPath, rightFilesName);
		sort(leftFilesName.begin(), leftFilesName.end());
		sort(rightFilesName.begin(), rightFilesName.end());
		for (int i = 0; i < leftFilesName.size(); i++)
		{
			string leftName = filesPath + "\\" + leftPath + "\\" + leftFilesName[i];
			string rightName = filesPath + "\\" + rightPath + "\\" + rightFilesName[i];
			//leftName = "ѵ��\\01˫��ƽ��\\left\\Left0012.bmp";
			//rightName = "ѵ��\\01˫��ƽ��\\Right\\Right0012.bmp";
			cout << leftName << endl;

			//���һ������ѡ��ʹ��sift��surf.0Ϊsurf,1Ϊsift
			if (!getRightAndLeftDisp(leftName,rightName, disp, leftDisp, rightDisp,1))
			{
				string savePath = filesPath + "\\" + depthPath;
				_mkdir(savePath.c_str());
				string saveLeftFile = savePath + "\\disp" + leftFilesName[i];
				string saveRightFile = savePath + "\\disp" + rightFilesName[i];
				//imwrite(saveLeftFile, leftDisp);
				//imwrite(saveRightFile, rightDisp);
				resize(leftDisp, leftDisp, Size(640, 480));
				resize(rightDisp, rightDisp, Size(640, 480));
				resize(disp, disp, Size(640, 480));
				imshow("dispL", leftDisp);
				imshow("dispR", rightDisp);
				char key = waitKey(0);
				if (key == 'q')
				{
					return 1;
				}
			}
			else
			{
				return 1;
			}
		}
	}
	cout << "finish" << endl;
#elif MODE==RECOGNITION
	string dumped_cnn = "cnn1.dumped";
	vector<string> fileList;
	getFilesAll(path, fileList);
	vector<string> files;
	getJustCurrentDir(path, files);

	//����cnnģ��
	cout << "read cnn" << endl;
	KerasModel m(dumped_cnn, true);

	//�����
	RNG rng(0xFFFFFFFF);
	while (1)
	{
		int ind = rng.uniform(0, fileList.size());
		//��������
		DataChunk *sample = new DataChunk2D();

		//��ͼƬ
		cout << endl << "read data" << endl;
		//sample->read_from_file(input_data);
		string fileName = "E:\\work\\finish26\\Left0508.bmp";
		//Mat image = imread(fileList[ind]);
		Mat image = imread(fileName);
		//ͳһת����224*224��С
		resize(image, image, Size(224, 224));
		//��ͼƬת����ģ�ͷ����ʽ
		sample->read_from_image(image);

		//��ʼ����
		cout << endl << "compute" << endl;
		std::vector<float> response = m.compute_output(sample);
		delete sample;

		//������
		cout << endl << "result:" << endl;
		//string outFile;
		//findName(fileList[ind], outFile);
		//ofstream fout("out\\" + outFile + ".dat");
		int resultLabel = 0;
		float maxResult = 0.0;
		//Ѱ�Ҹ�������Ϊ���ս��
		for (unsigned int i = 0; i < response.size(); i++)
		{
			cout << labels[i] << ": " << response[i] << endl;
			if (maxResult < response[i])
			{
				maxResult = response[i];
				resultLabel = i;
			}
			//fout << response[i] << " ";
		}
		cout << "��ǰ����Ϊ: " << labels[resultLabel] << endl;
		//fout << labels[resultLabel] << " ";
		//fout.close();
		imshow("image", image);
		char key = waitKey(0);
		if (key == 'q')
		{
			return 1;
		}
	}
	cout << "finish" << endl;
#endif
	return 0;
}
