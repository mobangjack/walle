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

//模型文件
static string model = "F:/sample/model";

static string firstSvmModel = model + ".NO1.xml";
static string secndSvmModel = model + ".NO2.xml";

static string firstFeature = model + ".NO1.feature";
static string secndFeature = model + ".NO2.feature";

static string firstDescritor = model + ".NO1.descritor";
static string secndDescritor = model + ".NO2.descritor";

//original 训练样本路径名
static string posTrainingSamplesDir = "F:/sample/pos/";
static string originalNegTrainingSamplesDir = "F:/sample/neg/";

//从原始负样本随机裁剪的图像
static string randNegSamplesDir = "F:/sample/random/";

//测试样本路径名
static string testPosDir = "F:/sample/test/pos/";
static string testNegDir = "F:/sample/test/neg/";
static string testSamplesDir = "F:/sample/test/";
//生成的难例存储路径：
static string hardSampleDir = "F:/sample/hard/";

HogSvm::HogSvm() {
	
}

HogSvm::~HogSvm() {

}


/************************************************************************/
/* 目标检测：全局搜索                                                   */
/************************************************************************/
std::vector<cv::Rect> HogSvm::detect(const cv::Mat& image)
{
	std::vector<cv::Rect> found, found_filtered;
	hog.detect(image, found);//对图片进行多尺度行人检测
	hog.filter(found, found_filtered);
	return found_filtered;
}

/************************************************************************/
/* 保存计算出来的特征描述                                               */
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
/* 从文件加载特征描述                                                   */
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
/* 前训练：处理原始样本图片                                             */
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
		Mat img = imread(imgName);//读取图片

		for (int j = 0; j < 10; j++)
		{
			int x = (rand() % (img.cols - hog.winSize.width)); //左上角x坐标
			int y = (rand() % (img.rows - hog.winSize.height));  //左上角y坐标
			Mat roi = img(Rect(x, y, hog.winSize.width, hog.winSize.height));
			sprintf_s(saveName, "random_neg_%06d.jpg", ++slides);//生成裁剪出的负样本图片的文件名
			imwrite(randNegSamplesDir + saveName, roi);//保存裁剪后的负样本图片文件
		}
	}
	cout << "pre-train done." << endl;
}

/************************************************************************/
/* 训练：基于pre_train处理后的样本                                      */
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

	Mat sampleFeatureMat = Mat::zeros(totalSampleCount, MY_HOG_DESCRIPTOR_VECTOR_SIZE(), CV_32FC1);//初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数sampleFeatureMat
	Mat sampleLabelMat = Mat::zeros(totalSampleCount, 1, CV_32FC1);//初始化训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有正例，0表示无正例

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
/* 在负样本集上检测，找出难例                                           */
/************************************************************************/
vector<string> HogSvm::findHardSamples()
{
	vector<string> hardExamples;
	vector<string> negativeSamples = utils::getFileList(randNegSamplesDir, "jpg,png,bmp", true);
	//在Neg中检测出来的都是误报的，将多尺度检测出来的窗口作为难例。
	char saveName[256];
	long hardExampleCount = 0;
	vector<Rect> found;
	// Walk over negative training samples, generate images and detect
	for (vector<string>::const_iterator it = negativeSamples.begin(); it != negativeSamples.end(); ++it)
	{
		const Mat img = imread(*it);
		hog.detect(img, found);
		//遍历从图像中检测出来的矩形框，得到hard example
		Rect r;
		for (int i = 0; i < found.size(); i++)
		{
			//检测出来的很多矩形框都超出了图像边界，将这些矩形框都强制规范在图像边界内部
			r = found[i];
			if (r.x < 0)
				r.x = 0;
			if (r.y < 0)
				r.y = 0;
			if (r.x + r.width > img.cols)
				r.width = img.cols - r.x;
			if (r.y + r.height > img.rows)
				r.height = img.rows - r.y;

			//将矩形框保存为图片，就是Hard Example
			Mat hardExampleImg = img(r);//从原图上截取矩形框大小的图片
			resize(hardExampleImg, hardExampleImg, MY_HOG_WIN_SIZE);//将剪裁出来的图片缩放为窗口大小
			sprintf_s(saveName, "hard_sample_%09ld.jpg", hardExampleCount++);//生成hard example图片的文件名
			imwrite(hardSampleDir + saveName, hardExampleImg);//保存文件
			hardExamples.push_back(hardSampleDir + saveName);
		}
	}
	cout << hardExampleCount << "hard examples were detected and saved to " << randNegSamplesDir << endl;
	return hardExamples;
}

/************************************************************************/
/* 后训练：处理难例                                                     */
/************************************************************************/
void HogSvm::post_train()
{
	vector<string> hardSamples = findHardSamples();
	const size_t hardSampleSize = hardSamples.size();

	Mat hardSampleFeatureMat = Mat::zeros(hardSampleSize, MY_HOG_DESCRIPTOR_VECTOR_SIZE(), CV_32FC1);//初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数sampleFeatureMat
	Mat hardSampleLabelMat = Mat::zeros(hardSampleSize, 1, CV_32FC1);//初始化训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有正例，0表示无正例

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
