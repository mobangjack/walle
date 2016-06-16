#pragma once

#include <opencv2/ml/ml.hpp>

class MySVM : public CvSVM
{
public:
	MySVM();
	virtual ~MySVM();

	void MySVM::regulate(const cv::Mat& sampleFeatureMat, const cv::Mat& sampleLabelMat);
	void MySVM::describe(std::vector<float>& descriptors);
	

	int get_alpha_count()
	{
		return this->sv_total;
	}

	int get_sv_dim()
	{
		return this->var_all;
	}

	int get_sv_count()
	{
		return this->decision_func->sv_count;
	}

	double* get_alpha()
	{
		return this->decision_func->alpha;
	}

	float** get_sv()
	{
		return this->sv;
	}

	float get_rho()
	{
		return this->decision_func->rho;
	}
};
