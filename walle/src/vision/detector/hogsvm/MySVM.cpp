#include "MySVM.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;

MySVM::MySVM()
{
	
}

MySVM::~MySVM()
{

}

/*************************************************************************************************
线性SVM训练完成后得到的XML文件里面，有一个数组，叫做support vector，还有一个数组，叫做alpha,有一个浮点数，叫做rho;
将alpha矩阵同support vector相乘，注意，alpha*supportVector,将得到一个列向量。之后，再该列向量的最后添加一个元素rho。
如此，便得到了一个分类器，利用该分类器，直接替换opencv中行人检测默认的那个分类器（cv::HOGDescriptor::setSVMDetector()），
就可以利用你的训练样本训练出来的分类器进行行人检测了。
***************************************************************************************************/
void MySVM::regulate(const cv::Mat& sampleFeatureMat, const cv::Mat& sampleLabelMat)
{
	cout << "************************************************************" << endl;
	cout << "regulating SVM..." << endl;

	CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, FLT_EPSILON);//迭代终止条件，当迭代满1000次或误差小于FLT_EPSILON时停止迭代
	CvSVMParams param(CvSVM::C_SVC, CvSVM::LINEAR, 0, 1, 0, 0.01, 0, 0, 0, criteria);//SVM参数：SVM类型为C_SVC；线性核函数；松弛因子C=0.01
	train(sampleFeatureMat, sampleLabelMat, Mat(), Mat(), param);//训练分类器
	cout << "SVM regulating done." << endl;
}

void MySVM::describe(vector<float>& descriptors)
{
	size_t descriptorSize = get_var_count();//特征向量的维数，即HOG描述子的维数
	int supportVectorCount = get_support_vector_count();//支持向量的个数
	cout << "support vector count: " << supportVectorCount << endl;

	Mat alphaMat = Mat::zeros(1, supportVectorCount, CV_32FC1);//alpha向量，长度等于支持向量个数
	Mat supportVectorMat = Mat::zeros(supportVectorCount, descriptorSize, CV_32FC1);//支持向量矩阵
	Mat resultMat = Mat::zeros(1, descriptorSize, CV_32FC1);//alpha向量乘以支持向量矩阵的结果

	for (int i = 0; i < supportVectorCount; i++)//将支持向量的数据复制到supportVectorMat矩阵中
	{
		const float * pSVData = get_support_vector(i);//返回第i个支持向量的数据指针
		for (int j = 0; j < descriptorSize; j++)
		{
			supportVectorMat.at<float>(i, j) = pSVData[j];
		}
	}

	//将alpha向量的数据复制到alphaMat中
	double * pAlphaData = get_alpha();//返回SVM的决策函数中的alpha向量
	for (int i = 0; i < supportVectorCount; i++)
	{
		alphaMat.at<float>(0, i) = pAlphaData[i];
	}

	//计算-(alphaMat * supportVectorMat),结果放到resultMat中
	//gemm(alphaMat, supportVectorMat, -1, 0, 1, resultMat);//不知道为什么加负号？
	resultMat = -1 * alphaMat * supportVectorMat;

	//得到最终的setSVMDetector(const vector<float>& detector)参数中可用的检测子
	//将resultMat中的数据复制到数组mySVMDetector中
	for (int i = 0; i < descriptorSize; i++)
	{
		descriptors.push_back(resultMat.at<float>(0, i));
	}
	//最后添加偏移量rho，得到检测子
	descriptors.push_back(get_rho());
}




