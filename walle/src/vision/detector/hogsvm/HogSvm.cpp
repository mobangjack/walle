#include "HogSvm.hpp"

#include "../../../utils.hpp"
#include <math.h>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;

//ģ���ļ�
static string model = "F:/sample/model";

static string firstSvmModel = model + ".NO1.xml";
static string secndSvmModel = model + ".NO2.xml";

static string firstFeature = model + ".NO1.feature";
static string secndFeature = model + ".NO2.feature";

static string firstDescritor = model + ".NO1.descritor";
static string secndDescritor = model + ".NO2.descritor";

//original ѵ������·����
static string posTrainingSamplesDir = "F:/sample/pos/";
static string originalNegTrainingSamplesDir = "F:/sample/neg/";

//��ԭʼ����������ü���ͼ��
static string randNegSamplesDir = "F:/sample/random/";

//��������·����
static string testPosDir = "F:/sample/test/pos/";
static string testNegDir = "F:/sample/test/neg/";
static string testSamplesDir = "F:/sample/test/";
//���ɵ������洢·����
static string hardSampleDir = "F:/sample/hard/";

HogSvm::HogSvm() {
	
}

HogSvm::~HogSvm() {

}


/************************************************************************/
/* Ŀ���⣺ȫ������                                                   */
/************************************************************************/
std::vector<cv::Rect> HogSvm::detect(const cv::Mat& image)
{
	std::vector<cv::Rect> found, found_filtered;
	hog.detect(image, found);//��ͼƬ���ж�߶����˼��
	hog.filter(found, found_filtered);
	return found_filtered;
}

/************************************************************************/
/* ��������������������                                               */
/************************************************************************/
static inline void saveFeatures(const string& saveFeatureFileName, const Mat& sampleFeatures, const Mat& sampleLabel)
{
	ofstream writeFeatureFile(saveFeatureFileName.c_str(), ios::out);
	if (writeFeatureFile.good() && writeFeatureFile.is_open())
	{
		for (int i = 0; i < sampleFeatures.rows; i++)
		{
			writeFeatureFile << sampleLabel.at<float>(i) << " ";
			for (int j = 0; j< sampleFeatures.cols; j++)
			{
				writeFeatureFile << sampleFeatures.at<float>(i, j) << " ";
			}
			writeFeatureFile << endl;
		}
		writeFeatureFile.close();
		cout << "Save FeatureMat Done." << endl;;
	} else
	{
		cout << "Feature file open error." << saveFeatureFileName << endl;
	}

}

/************************************************************************/
/* ���ļ�������������                                                   */
/************************************************************************/
static void inline loadFeatures(const string& featureFileName, Mat& featureMat, Mat& labelMat)
{
	ifstream readFeature(featureFileName.c_str(), ios::in);
	int rows = featureMat.rows;
	int cols = featureMat.cols;
	float * p;
	for (int i = 0; i<rows; i++)
	{
		p = featureMat.ptr<float>(i);
		for (int j = 0; j<cols; j++)
		{
			readFeature >> p[j];
		}
	}
	readFeature.close();
}

/************************************************************************/
/* ǰѵ��������ԭʼ����ͼƬ                                             */
/************************************************************************/
void HogSvm::pre_train()
{
	cout << "pre-train: croping original negative image samples..." << endl;
	vector<string> negativeSamples = utils::getFileList(originalNegTrainingSamplesDir, "jpg,png,bmp", true);
	size_t slides = 0;
	char saveName[256];
	for (int i = 0; i < negativeSamples.size(); i++)
	{
		string imgName = negativeSamples.at(i);
		cout << "processing: " << imgName << endl;
		Mat img = imread(imgName);//��ȡͼƬ

		for (int j = 0; j < 10; j++)
		{
			int x = (rand() % (img.cols - hog.winSize.width)); //���Ͻ�x����
			int y = (rand() % (img.rows - hog.winSize.height));  //���Ͻ�y����
			Mat roi = img(Rect(x, y, hog.winSize.width, hog.winSize.height));
			sprintf_s(saveName, "random_neg_%06d.jpg", ++slides);//���ɲü����ĸ�����ͼƬ���ļ���
			imwrite(randNegSamplesDir + saveName, roi);//����ü���ĸ�����ͼƬ�ļ�
		}
	}
	cout << "pre-train done." << endl;
}

/************************************************************************/
/* ѵ��������pre_train����������                                      */
/************************************************************************/
void HogSvm::train()
{
	vector<string> positiveSamples = utils::getFileList(posTrainingSamplesDir, "jpg,png,bmp", true);
	vector<string> negativeSamples = utils::getFileList(randNegSamplesDir, "jpg,png,bmp", true);

	size_t positiveSampleCount = positiveSamples.size();
	size_t negativeSampleCount = negativeSamples.size();
	size_t totalSampleCount = positiveSampleCount + negativeSampleCount;

	cout << "//////////////////////////////////////////////////////////////////" << endl;
	cout << "totalSampleCount: " << totalSampleCount << endl;
	cout << "positiveSampleCount: " << positiveSampleCount << endl;
	cout << "negativeSampleCount: " << negativeSampleCount << endl;

	cout << "************************************************************" << endl;
	cout << "start training with positive samples..." << endl;

	Mat sampleFeatureMat = Mat::zeros(totalSampleCount, MY_HOG_DESCRIPTOR_VECTOR_SIZE(), CV_32FC1);//��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��sampleFeatureMat
	Mat sampleLabelMat = Mat::zeros(totalSampleCount, 1, CV_32FC1);//��ʼ��ѵ����������������������������������ĸ�������������1��1��ʾ��������0��ʾ������

	hog.describe(positiveSamples, 1, sampleFeatureMat, sampleLabelMat);

	cout << "************************************************************" << endl;
	cout << "start training with negative samples..." << endl;
	hog.describe(negativeSamples, -1, sampleFeatureMat, sampleLabelMat);

	cout << "************************************************************" << endl;
	cout << "start regulating SVM..." << endl;

	svm.regulate(sampleFeatureMat, sampleLabelMat);

	cout << "saving firstSvmModel to file " << firstSvmModel <<  "  ...  " << endl;
	svm.save(firstSvmModel.c_str());
	cout << "firstSvmModel saved to " << firstSvmModel << endl;

	cout << "saving firstDescritor to file " << firstDescritor << "  ...  " << endl;
	vector<float> descriptors;
	svm.describe(descriptors);
	hog.saveDescriptors(descriptors, firstDescritor);
	cout << "firstDescritor saved to " << firstDescritor << endl;

	cout << "saving firstFeature to file " << firstFeature << "  ...  " << endl;
	saveFeatures(firstFeature, sampleFeatureMat, sampleLabelMat);
	cout << "firstFeature saved to " << firstFeature << endl;

	cout << "train proccess done." << endl;
}

/************************************************************************/
/* �ڸ��������ϼ�⣬�ҳ�����                                           */
/************************************************************************/
vector<string> HogSvm::findHardSamples()
{
	vector<string> hardExamples;
	vector<string> negativeSamples = utils::getFileList(randNegSamplesDir, "jpg,png,bmp", true);
	//��Neg�м������Ķ����󱨵ģ�����߶ȼ������Ĵ�����Ϊ������
	char saveName[256];
	long hardExampleCount = 0;
	vector<Rect> found;
	// Walk over negative training samples, generate images and detect
	for (vector<string>::const_iterator it = negativeSamples.begin(); it != negativeSamples.end(); ++it)
	{
		const Mat img = imread(*it);
		hog.detect(img, found);
		//������ͼ���м������ľ��ο򣬵õ�hard example
		Rect r;
		for (int i = 0; i < found.size(); i++)
		{
			//�������ĺܶ���ο򶼳�����ͼ��߽磬����Щ���ο�ǿ�ƹ淶��ͼ��߽��ڲ�
			r = found[i];
			if (r.x < 0)
				r.x = 0;
			if (r.y < 0)
				r.y = 0;
			if (r.x + r.width > img.cols)
				r.width = img.cols - r.x;
			if (r.y + r.height > img.rows)
				r.height = img.rows - r.y;

			//�����ο򱣴�ΪͼƬ������Hard Example
			Mat hardExampleImg = img(r);//��ԭͼ�Ͻ�ȡ���ο��С��ͼƬ
			resize(hardExampleImg, hardExampleImg, MY_HOG_WIN_SIZE);//�����ó�����ͼƬ����Ϊ���ڴ�С
			sprintf_s(saveName, "hard_sample_%09ld.jpg", hardExampleCount++);//����hard exampleͼƬ���ļ���
			imwrite(hardSampleDir + saveName, hardExampleImg);//�����ļ�
			hardExamples.push_back(hardSampleDir + saveName);
		}
	}
	cout << hardExampleCount << "hard examples were detected and saved to " << randNegSamplesDir << endl;
	return hardExamples;
}

/************************************************************************/
/* ��ѵ������������                                                     */
/************************************************************************/
void HogSvm::post_train()
{
	vector<string> hardSamples = findHardSamples();
	const size_t hardSampleSize = hardSamples.size();

	Mat hardSampleFeatureMat = Mat::zeros(hardSampleSize, MY_HOG_DESCRIPTOR_VECTOR_SIZE(), CV_32FC1);//��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��sampleFeatureMat
	Mat hardSampleLabelMat = Mat::zeros(hardSampleSize, 1, CV_32FC1);//��ʼ��ѵ����������������������������������ĸ�������������1��1��ʾ��������0��ʾ������

	cout << "************************************************************" << endl;
	cout << "start training with negative samples..." << endl;
	hog.describe(hardSamples, -1, hardSampleFeatureMat, hardSampleLabelMat);

	Mat sampleFeatureMat;
	Mat sampleLabelMat;

	cout << "merging features..." << endl;
	loadFeatures(model + "", sampleFeatureMat, sampleLabelMat);
	sampleFeatureMat.push_back(hardSampleFeatureMat);
	sampleLabelMat.push_back(hardSampleLabelMat);

	cout << "************************************************************" << endl;
	cout << "start regulating SVM..." << endl;

	svm.regulate(sampleFeatureMat, sampleLabelMat);

	cout << "saving secndSvmModel to file " << secndSvmModel << "  ...  " << endl;
	svm.save(secndSvmModel.c_str());
	cout << "secndSvmModel saved to " << secndSvmModel << endl;

	cout << "saving secndDescritor to file " << secndDescritor << "  ...  " << endl;
	vector<float> descriptors;
	svm.describe(descriptors);
	hog.saveDescriptors(descriptors, secndDescritor);
	cout << "secndDescritor saved to " << secndDescritor << endl;

	cout << "saving secndFeature to file " << secndFeature << "  ...  " << endl;
	saveFeatures(secndFeature, sampleFeatureMat, sampleLabelMat);
	cout << "secndFeature saved to " << secndFeature << endl;

	cout << "post-train proccess done." << endl;
}


/**
void main()
{
HogSvmTrainer trainer;
trainer.pre_train();
trainer.train();
trainer.post_train();
}
*/
