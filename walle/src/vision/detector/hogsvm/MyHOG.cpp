#include "MyHOG.hpp"

#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;

MyHOG::MyHOG() : HOGDescriptor(MY_HOG_WIN_SIZE, MY_HOG_BLOCK_SIZE, MY_HOG_BLOCK_STRIDE, MY_HOG_CELL_SIZE, MY_HOG_N_BINS) 
{

}

MyHOG::~MyHOG()
{

}

void MyHOG::describe(const cv::Mat& img, std::vector<float>& descriptors)
{
	Mat dst;
	resize(img, dst, MY_HOG_WIN_SIZE);//����
	compute(dst, descriptors, MY_HOG_WIN_STRIDE);//����HOG������
}

std::vector<float> MyHOG::describe(const cv::Mat& img)
{
	vector<float> descriptors;
	describe(img, descriptors);
	return descriptors;
}

void MyHOG::describe(const vector<string>& imgset, const int& label, Mat& featureMat, Mat& labelMat)
{
	for (int i = 0; i < imgset.size(); i++)
	{
		string imgName = imgset.at(i);
		//cout << "processing: " << imgName << endl;
		Mat img = imread(imgName);//��ȡͼƬ
		resize(img, img, MY_HOG_WIN_SIZE);//����
		vector<float> descriptors;//HOG����������
		describe(img, descriptors);//����HOG������

		for (int j = 0; j < descriptors.size(); j++)//������õ�HOG�����Ӹ��Ƶ�������������featureMat
			featureMat.at<float>(i, j) = descriptors[j];//��i�����������������еĵ�j��Ԫ��
		labelMat.at<float>(i, 0) = label;//���������Ϊ1�����������Ϊ-1
	}
}

void MyHOG::saveDescriptors(const vector<float>& descriptors, const string& path) const
{
	ofstream outstream(path);
	if (outstream.good() && outstream.is_open()) {
		for (int feature = 0; feature < descriptors.size(); ++feature)
			outstream << descriptors.at(feature) << " ";	//д�����������÷ָ�����

		outstream.flush();
		outstream.close();
		cout << "MyHOG descriptor model was saved to: " << path << endl;
	}
	else
	{
		cout << "MyHOG fail to save descriptor model: " << path << endl;
	}
}

void MyHOG::loadDescriptors(const std::string& path)
{
	vector<float> descriptors;
	ifstream instream(path, ios::in);
	if (instream.good() && instream.is_open())
	{
		float temp = 0;
		while (!instream.eof()) {
			instream >> temp;
			descriptors.push_back(temp);
		}
		setSVMDetector(descriptors);
		cout << "MyHOG descriptor model load done: " << path << endl;
	}
	else
	{
		cout << "MyHOG failed to load descriptor model: " << path << endl;
	}
}

void MyHOG::detect(const cv::Mat &img, std::vector<cv::Rect>& found)
{
	detectMultiScale(img, found, 0, MY_HOG_WIN_STRIDE, MY_HOG_PADDING, MY_HOG_SCALE_F, 2);//��ͼƬ���ж�߶����˼��
}

void MyHOG::filter(const std::vector<cv::Rect>& found, std::vector<cv::Rect>& found_filtered)
{
	//�ҳ�����û��Ƕ�׵ľ��ο�r,������found_filtered��,�����Ƕ�׵Ļ�,��ȡ���������Ǹ����ο����found_filtered��
	int i = 0, j = 0;
	Rect r;
	for (i = 0; i < found.size(); i++) {
		r = found[i];
		for (j = 0; j < found.size(); j++)
			if (j != i && (r & found[j]) == r)
				break;
		if (j == found.size()) {
			// the HOG detector returns slightly larger rectangles than the real objects.
			// so we slightly shrink the rectangles to get a nicer output.
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			found_filtered.push_back(r);
		}
	}
}
