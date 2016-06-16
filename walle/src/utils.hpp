#pragma once

#include <io.h>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace utils
{
	using namespace std;
	using namespace cv;

	/************************************************************************/
	/* תСд����                                                           */
	/************************************************************************/
	string toLowerCase(const string& in);

	/************************************************************************/
	/* ȥ�ո�                                                               */
	/************************************************************************/
	string trim(const string& in);

	/************************************************************************/
	/* ��ָ���ļ��л�ȡָ����׺���ļ����б��ɵݹ����                     */
	/************************************************************************/
	void getFileList(vector<string>& fileList, const string& dir, const string& ext, const bool& re);

	/************************************************************************/
	/* ��ָ���ļ��л�ȡָ����׺���ļ����б��ɵݹ����                     */
	/************************************************************************/
	vector<string> getFileList(const string& dir, const string& ext, const bool& re);

	/************************************************************************/
	/* ��ͼƬ������ת�任                                                   */
	/************************************************************************/
	Size2f rotateImg(int degree, const Mat &img, Mat &img_rotate);

	/************************************************************************/
	/* ��ͼƬ������ת�任                                                   */
	/************************************************************************/
	Mat rotateImg(int degree, const Mat &img);




	/************************************************************************/




	/************************************************************************/
	/* תСд����                                                           */
	/************************************************************************/
	string toLowerCase(const string& in) {
		string t;
		for (string::const_iterator i = in.begin(); i != in.end(); ++i) {
			t += tolower(*i);
		}
		return t;
	}
	
	/************************************************************************/
	/* ȥ�ո�                                                               */
	/************************************************************************/
	string trim(const string& in)
	{
		int i = in.length()-1;
		for (; i > 0; i++)
		{
			if (in.at(i) != ' ')
			{
				return in.substr(0, i + 1);
			}
		}
		return in;
	}

	/************************************************************************/
	/* ��ָ���ļ��л�ȡָ����׺���ļ����б��ɵݹ����                     */
	/************************************************************************/
	void getFileList(vector<string>& fileList, const string& dir, const string& ext, const bool& re) 
	{
		long hFile = 0;
		struct _finddata_t fileInfo;
		string  tempPathName1 = trim(dir);
		if (dir.find_last_of('/') != tempPathName1.length() - 1)
		{
			tempPathName1 += "/";
		}
		string tempPathName2 = tempPathName1;
		if ((hFile = _findfirst(tempPathName1.append("*").c_str(), &fileInfo)) == -1)
			return;
		
		do {
			if (fileInfo.attrib&_A_SUBDIR)//�ļ���
			{
				if(re)
					getFileList(fileList, tempPathName2+fileInfo.name, ext, re);
				else
					continue;
			}
			else
			{
				int i = string(fileInfo.name).find_last_of(".");
				string tempExt = toLowerCase(string(fileInfo.name).substr(i + 1));
				
				if (ext.find(tempExt) != string::npos)
				{
					fileList.push_back(dir + (string(fileInfo.name)));
				}
			}
		} while (_findnext(hFile, &fileInfo) == 0);
		_findclose(hFile);
	}

	/************************************************************************/
	/* ��ָ���ļ��л�ȡָ����׺���ļ����б��ɵݹ����                     */
	/************************************************************************/
	vector<string> getFileList(const string& dir, const string& ext, const bool& re)
	{
		vector<string> fileList;
		getFileList(fileList, dir, ext, re);
		return fileList;
	}

	/************************************************************************/
	/* ��ͼƬ������ת�任                                                   */
	/************************************************************************/
	Size2f rotateImg(int degree, const Mat& img, Mat& img_rotate)
	{
		if (degree == 0)
		{
			img.copyTo(img_rotate);
			return img.size();
		}
		double angle = degree  * CV_PI / 180.; // ����    
		double a = sin(angle), b = cos(angle);
		int width = img.cols;
		int height = img.rows;
		int width_rotate = int(height * fabs(a) + width * fabs(b));
		int height_rotate = int(width * fabs(a) + height * fabs(b));
		img_rotate = Mat(cvSize(width_rotate, height_rotate), 8, 3);
		Point2f center = Point2f(width / 2, height / 2);
		Mat map_matrix = getRotationMatrix2D(center, degree, 1.0);
		map_matrix.at<double>(0, 2) += int((width_rotate - width) / 2);
		map_matrix.at<double>(1, 2) += int((height_rotate - height) / 2);
		warpAffine(img, img_rotate, map_matrix, img_rotate.size());
		return img_rotate.size();
	}
	/************************************************************************/
	/* ��ͼƬ������ת�任                                                   */
	/************************************************************************/
	Mat rotateImg(int degree, const Mat &img)
	{
		Mat rotatedimage;
		rotateImg(degree, img, rotatedimage);
		return rotatedimage;
	}

}