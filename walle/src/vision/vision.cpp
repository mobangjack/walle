#include "vision.hpp"
#include "math.h"

Vision::Vision(Detector* detector, Tracker* tracker, cv::Size stdSize, double retargetOnSpeed)
{
	this->detector = detector;
	this->tracker = tracker;
	this->stdSize = stdSize;
	this->retargetOnSpeed = retargetOnSpeed;
}

Vision::~Vision()
{
	delete detector;
	delete tracker;
}

void Vision::capture(cv::Mat& frame)
{
	this->frame = frame;
	this->width = frame.cols;
	this->height = frame.rows;
	this->halfWidth = width / 2.0f;
	this->halfHeight = height / 2.0f;
}

static inline double geometricalDistance(int x, int y) {
	return sqrt(x * x + y * y);
}

bool Vision::spot()
{
	if (lost || trackingDeltaS < retargetOnSpeed) {
		detectionResults = detector->detect(frame);
		if (detectionResults.size() > 0) {
			double min_ds = sqrt(halfWidth * halfWidth + halfHeight * halfHeight);
			for (int i = 0; i < detectionResults.size(); i++) {
				double tmp_ds = geometricalDistance(detectionResults[i].x - halfWidth, detectionResults[i].y - halfHeight);
				if (tmp_ds < min_ds) {
					min_ds = tmp_ds;
					mostCenteredDetectionResult = detectionResults[i];
				}
			}
			target = mostCenteredDetectionResult; //consider the most centered result
			tracker->init(frame, target); //important
			lost = false;
		} else {
			lost = true;
		}
	} else {
		lastTrackingResult = thisTrackingResult;
		thisTrackingResult = tracker->update(frame);
		trackingDeltaX = thisTrackingResult.x - lastTrackingResult.x;
		trackingDeltaY = thisTrackingResult.y - lastTrackingResult.y;
		trackingDeltaS = geometricalDistance(trackingDeltaX, trackingDeltaY);
		target = thisTrackingResult;
	}
	if (!lost)
	{
		dx = target.x - halfWidth;
		dy = target.y - halfHeight;
		dz = target.area() / stdSize.area();
		ds = geometricalDistance(dx, dy);
		return true;
	}
	return false;
}

