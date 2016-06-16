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
����SVMѵ����ɺ�õ���XML�ļ����棬��һ�����飬����support vector������һ�����飬����alpha,��һ��������������rho;
��alpha����ͬsupport vector��ˣ�ע�⣬alpha*supportVector,���õ�һ����������֮���ٸ���������������һ��Ԫ��rho��
��ˣ���õ���һ�������������ø÷�������ֱ���滻opencv�����˼��Ĭ�ϵ��Ǹ���������cv::HOGDescriptor::setSVMDetector()����
�Ϳ����������ѵ������ѵ�������ķ������������˼���ˡ�
***************************************************************************************************/
void MySVM::regulate(const cv::Mat& sampleFeatureMat, const cv::Mat& sampleLabelMat)
{
	cout << "************************************************************" << endl;
	cout << "regulating SVM..." << endl;

	CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, FLT_EPSILON);//������ֹ��������������1000�λ����С��FLT_EPSILONʱֹͣ����
	CvSVMParams param(CvSVM::C_SVC, CvSVM::LINEAR, 0, 1, 0, 0.01, 0, 0, 0, criteria);//SVM������SVM����ΪC_SVC�����Ժ˺������ɳ�����C=0.01
	train(sampleFeatureMat, sampleLabelMat, Mat(), Mat(), param);//ѵ��������
	cout << "SVM regulating done." << endl;
}

void MySVM::describe(vector<float>& descriptors)
{
	size_t descriptorSize = get_var_count();//����������ά������HOG�����ӵ�ά��
	int supportVectorCount = get_support_vector_count();//֧�������ĸ���
	cout << "support vector count: " << supportVectorCount << endl;

	Mat alphaMat = Mat::zeros(1, supportVectorCount, CV_32FC1);//alpha���������ȵ���֧����������
	Mat supportVectorMat = Mat::zeros(supportVectorCount, descriptorSize, CV_32FC1);//֧����������
	Mat resultMat = Mat::zeros(1, descriptorSize, CV_32FC1);//alpha��������֧����������Ľ��

	for (int i = 0; i < supportVectorCount; i++)//��֧�����������ݸ��Ƶ�supportVectorMat������
	{
		const float * pSVData = get_support_vector(i);//���ص�i��֧������������ָ��
		for (int j = 0; j < descriptorSize; j++)
		{
			supportVectorMat.at<float>(i, j) = pSVData[j];
		}
	}

	//��alpha���������ݸ��Ƶ�alphaMat��
	double * pAlphaData = get_alpha();//����SVM�ľ��ߺ����е�alpha����
	for (int i = 0; i < supportVectorCount; i++)
	{
		alphaMat.at<float>(0, i) = pAlphaData[i];
	}

	//����-(alphaMat * supportVectorMat),����ŵ�resultMat��
	//gemm(alphaMat, supportVectorMat, -1, 0, 1, resultMat);//��֪��Ϊʲô�Ӹ��ţ�
	resultMat = -1 * alphaMat * supportVectorMat;

	//�õ����յ�setSVMDetector(const vector<float>& detector)�����п��õļ����
	//��resultMat�е����ݸ��Ƶ�����mySVMDetector��
	for (int i = 0; i < descriptorSize; i++)
	{
		descriptors.push_back(resultMat.at<float>(0, i));
	}
	//������ƫ����rho���õ������
	descriptors.push_back(get_rho());
}




