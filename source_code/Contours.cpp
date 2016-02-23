#include "Contours.h"
#include <algorithm> 

bool compareContoursBySize(vector<Point> contourOne, vector<Point> contourTwo) {
	return contourArea(contourOne) < contourArea(contourTwo);
}

void rejectPixelsCloseToEdge(Mat& imageGray) {
	if (imageGray.cols > imageGray.rows) {
		for (int i = 0; i < imageGray.rows; i++) {
			for (int j = 0; j < imageGray.cols / 20; j++) {
				imageGray.at<uchar>(i, j) = 0;
			}
			for (int j = 19 * imageGray.cols / 20; j < imageGray.cols; j++) {
				imageGray.at<uchar>(i, j) = 0;
			}
		}
	}
	else {
		for (int j = 0; j < imageGray.cols; j++) {
			for (int i = 0; i < imageGray.rows / 20; i++) {
				imageGray.at<uchar>(i, j) = 0;
			}
			for (int i = 19 * imageGray.rows / 20; i < imageGray.rows; i++) {
				imageGray.at<uchar>(i, j) = 0;
			}
		}
	}
}

vector<vector<Point>> rejectTooSmallContours(vector<vector<Point>> contours) {
	vector<vector<Point>> result;
	for (int i = 0; i < contours.size(); i++) {
		if (contourArea(contours[i]) > 200) {
			result.push_back(contours[i]);
		}
	}
	return result;
}

vector<vector<Point>> getFourHighestAreaAndRejectTooBig(vector<vector<Point>>& contours) {
	vector<vector<Point>> result;
	if (contours.size() < 4) {
		return result;
	}
	sort(contours.begin(), contours.end(), compareContoursBySize);  // sort by size
	float maxArea = contourArea(contours[contours.size() - 1]);
	float fourthArea = contourArea(contours[contours.size() - 4]);
	while (maxArea / fourthArea > 7 && contours.size() > 4) {   // if there is too big contour, reject it
		contours.pop_back();
		maxArea = contourArea(contours[contours.size() - 1]);
		fourthArea = contourArea(contours[contours.size() - 4]);
	}

	int j = contours.size() - 1;
	int k = contours.size() - 4;
	for (int i = j; i >= k; i--) {
		result.push_back(contours[i]);
	}
	return result;
}

vector<vector<Point>> rejectTooOblong(vector<vector<Point>> contours) {
	vector<vector<Point>> result;
	for (int i = 0; i < contours.size(); i++) {
		RotatedRect rect = minAreaRect(contours[i]);
		// check if contour is oblong and is not ordinary rectangle
		if (max(rect.size.height, rect.size.width) / min(rect.size.height, rect.size.width) > 5
			&& (rect.size.height * rect.size.width) / contourArea(contours[i]) > 1.5) {
			continue;
		}
		result.push_back(contours[i]);
	}
	return result;
}

vector<vector<Point>> findMarkers(Mat imageColor, Mat imageGray) {
	vector<vector<Point>> result;
	vector<vector<Point>> temp;
	Mat grayCopy = imageGray.clone();
	int blueMin = 10; int blueMax = 40;
	int greenMin = 10; int greenMax = 40;
	int redMin = 10; int redMax = 40;
	inRange(imageColor, Scalar(blueMin, greenMin, redMin), Scalar(blueMax, greenMax, redMax), imageGray);
	erode(imageGray, imageGray, getStructuringElement(MORPH_RECT, Size(2, 2)));
	dilate(imageGray, imageGray, getStructuringElement(MORPH_RECT, Size(8, 8)));

	threshold(grayCopy, grayCopy, 40, 255, THRESH_BINARY);
	vector<vector<Point>> contours;

	bitwise_not(grayCopy, grayCopy);
	bitwise_and(grayCopy, imageGray, imageGray);
	rejectPixelsCloseToEdge(imageGray);
	
	findContours(imageGray, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
	temp = rejectTooSmallContours(contours);
	
	
	contours = rejectTooOblong(temp);
	result = getFourHighestAreaAndRejectTooBig(contours);
	if (contours.size() < 5) {
		return result;
	}
	
	vector<RotatedRect> rectangles;
	for (int i = 0; i < result.size(); i++) {
		rectangles.push_back(minAreaRect(result[i]));  // create best-matched rectangle for every contour
	}
	rejectOtherShape(result, rectangles, contours);

	return result;
}

void rejectOtherShape(vector<vector<Point>>& resultContours, vector<RotatedRect> rectangles, vector<vector<Point>> oldContours) {
	float shorterSideCurrent = 0;
	float shorterSideOther = 0;
	float longerSideCurrent = 0;
	float longerSideOther = 0;

	for (int i = 0; i < resultContours.size(); i++) {

		int shorterDimensionDifferences = 0;
		int longerDimensionsDifferences = 0;
		
		// i - current contour
		// j - others
		for (int j = 0; j < resultContours.size(); j++) {  // compare shorter sides
			if (j != i) {  // avoid compare with itself
				shorterSideCurrent = min(rectangles[i].size.height, rectangles[i].size.width);
				shorterSideOther = min(rectangles[j].size.height, rectangles[j].size.width);
				if (max(shorterSideCurrent, shorterSideOther) / min(shorterSideCurrent, shorterSideOther) > 2) {
					shorterDimensionDifferences++;
				}
			}
		}
		for (int j = 0; j < resultContours.size(); j++) {  // compare longer sides
			if (j != i) {  // avoid compare with itself
				longerSideCurrent = max(rectangles[i].size.height, rectangles[i].size.width);
				longerSideOther = max(rectangles[j].size.height, rectangles[j].size.width);
				if (max(longerSideCurrent, longerSideOther) / min(longerSideCurrent, longerSideOther) > 2) {
					longerDimensionsDifferences++;
				}
			}
		}

		if (shorterDimensionDifferences == 3 || longerDimensionsDifferences == 3) {
			resultContours[i] = oldContours[oldContours.size() - 5];  // reject current contour; attach next contour in the queue
			break;
		}
	}
}


bool areRectangularMarkers(vector<vector<Point>> resultContours) {
	if (resultContours.size() != 4) {
		return false;
	}
	RotatedRect rect;
	Point2f circleCenter;
	float radius;
	float ratio;
	int shapes[4] = { 0, 0, 0, 0 }; // 0 means rectangle, 1 - L shape, 2 - circle
	for (int i = 0; i < resultContours.size(); i++) {
		/**** way to find L-shape marker ***/
		rect = minAreaRect(resultContours[i]);
		ratio = (rect.size.height * rect.size.width) / contourArea(resultContours[i]);
		if (ratio > 2) {
			shapes[i] = 1;
			continue;
		}

		/*****  way to find circular marker  *****/
		minEnclosingCircle(resultContours[i], circleCenter, radius);
		ratio = (radius*radius*3.14) / contourArea(resultContours[i]);
		if (ratio < 1.9) {
			shapes[i] = 2;
		}
	}

	bool isRectangularShape = false;
	int sum = 0;
	for (int i = 0; i < 4; i++) {
		sum += shapes[i];
	}
	if (sum == 0) {
		isRectangularShape = true;
	}

	return isRectangularShape;
}

