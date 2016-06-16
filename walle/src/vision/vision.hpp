#pragma once

#include "detector/detector.hpp"
#include "tracker/tracker.hpp"

#include "detector/hogsvm/HogSvm.hpp"
#include "tracker/kcf\kcftracker.hpp"

#define STD_SIZE cv::Size(168,64)
#define RETARGET_ON_SPEED 10

class Vision {
public:
	Vision(Detector* detector = new HogSvm(), Tracker* tracker = new KCFTracker(), cv::Size stdSize = STD_SIZE, double retargetOnSpeed = RETARGET_ON_SPEED);
	~Vision();

	void capture(cv::Mat& frame);
	bool spot();

	cv::Rect  target;
	cv::Size  stdSize;

	int       dx;
	int       dy;
	double    ds;
	double    dz;

	double    retargetOnSpeed;
	
protected:
	Detector* detector;
	Tracker* tracker;

protected:
	cv::Mat frame;
	int width, height;
	float halfWidth, halfHeight;
	
	std::vector<cv::Rect> detectionResults;
	cv::Rect  mostCenteredDetectionResult;

	cv::Rect  lastTrackingResult;
	cv::Rect  thisTrackingResult;

	int       trackingDeltaX;
	int       trackingDeltaY;
	double    trackingDeltaS;

	bool      lost;
	
};
