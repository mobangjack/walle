#pragma once

#define YES 1
#define NO  0

/************ Platform Definition *************/
#define OS_UNIX    0
#define OS_WINDOWS 1
#define OS OS_WINDOWS

/********** Retarget Mode Definition **********/
#define MANUAL_MODE   0
#define AUTO_MODE     1
#define RETARGET_MODE MANUAL_MODE

/********* Defense Mode Configuration *********/
#define DEFENSE_MODE_PASSIVE 0
#define DEFENSE_MODE_ACTIVE  1
#define DEFENSE_MODE DEFENSE_MODE_ACTIVE

/****** Guidance Module Functional State ******/
#define USE_GUIDANCE_MODULE YES

/****** Obstacle Avoidance Configuration ******/
#define OBSTACLE_AVOIDANCE_D 10 //Default (in millimeters)
#define OBSTACLE_AVOIDANCE_F OBSTACLE_AVOIDANCE_D
#define OBSTACLE_AVOIDANCE_B OBSTACLE_AVOIDANCE_D
#define OBSTACLE_AVOIDANCE_L OBSTACLE_AVOIDANCE_D
#define OBSTACLE_AVOIDANCE_R OBSTACLE_AVOIDANCE_D

/********* Serial Communication Port **********/
#define ROBOT_WINDOWS_COMM "COM12"
#define ROBOT_LINUX_DEVICE "/dev/ttyS0"

/********* Vision Algorithm Choosing **********/
#define DETECTOR_ALGORITHM "HOG_SVM"
#define TRACKER_ALGORITHM  "KCF"

/** Policy: Track On High Speed, Detect On Still **/
#define RETARGET_ON_SPEED 5

/********* Fire On The Given Accuracy *********/
#define FIRE_ON_ACCURACY  5

/************ Show Vision Window **************/
#define SHOW_VISION_WINDOW YES

#include "robot/robot.hpp"
#include "vision/vision.hpp"

class Walle {
public:
	Walle(const char* port, double fireOnAccuracy = FIRE_ON_ACCURACY);
	~Walle() {};

	bool shakeHand();
	cv::Rect guide(cv::Mat& frame);

	double fireOnAccuracy;

protected:
	Vision vision;
	Robot robot;
};
