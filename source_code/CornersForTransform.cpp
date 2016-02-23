#include "CornersForTransform.h"
#include <math.h>


Point calculatePointByContour(vector<Point> contour) {
	Moments moment = moments(contour);
	int x = moment.m10 / moment.m00;
	int y = moment.m01 / moment.m00;
	return Point(x, y);
}

bool compareContoursByPositionX(vector<Point> contourOne, vector<Point> contourTwo) {
	int xOne = calculatePointByContour(contourOne).x;
	int xTwo = calculatePointByContour(contourTwo).x;
	return xOne < xTwo;
}

bool compareContoursByPositionY(vector<Point> contourOne, vector<Point> contourTwo) {
	int yOne = calculatePointByContour(contourOne).y;
	int yTwo = calculatePointByContour(contourTwo).y;
	return yOne < yTwo;
}

bool comparePointsByPositionY(Point pointOne, Point pointTwo) {
	return pointOne.y < pointTwo.y;
}

bool comparePointsByPositionX(Point pointOne, Point pointTwo) {
	return pointOne.x < pointTwo.x;
}

vector<Point> findCornersNonRectangularShape(vector<vector<Point>> contours) {
	vector<Point> result;
	if (contours.size() != 4) {
		return result;
	}

	sort(contours.begin(), contours.end(), compareContoursByPositionY);
	sort(contours.begin(), contours.begin() + 2, compareContoursByPositionX);
	sort(contours.begin() + 2, contours.end(), compareContoursByPositionX);
	// now appropriate order is: 0, 1, 3, 2

	Point first = calculatePointByContour(contours[0]);
	Point second = calculatePointByContour(contours[1]);
	Point third = calculatePointByContour(contours[3]);
	Point fourth = calculatePointByContour(contours[2]);

	result.push_back(first);
	result.push_back(second);
	result.push_back(third);
	result.push_back(fourth);

	return result;
}

vector<Point> findCornersRectangularShape(vector<vector<Point>> contours, Mat imageColor) {
	vector<Point> result;
	if (contours.size() != 4) {
		return result;
	}

	// coefficients of the lines
	float a[4];
	float b[4];
	calculateLineCoefficients(a, b, contours);
	int x[4] = { 0, 0, 0, 0 };
	int y[4] = { 0, 0, 0, 0 };
	calculatePointsOfIntersection(contours, x, y, a, b, imageColor);

	for (int i = 0; i < 4; i++) {
		if (x[i] == 0 || y[i] == 0) {
			return result;		// don't return any points if there was any mistake
		}
	}

	result.push_back(Point(x[0], y[0]));
	result.push_back(Point(x[1], y[1]));
	result.push_back(Point(x[2], y[2]));
	result.push_back(Point(x[3], y[3]));

	sort(result.begin(), result.end(), comparePointsByPositionY);
	sort(result.begin(), result.begin() + 2, comparePointsByPositionX);
	sort(result.begin() + 2, result.end(), comparePointsByPositionX);
	// now appropriate order is: 0, 1, 3, 2
	Point temp = result[3];
	result[3] = result[2];
	result[2] = temp;

	return result;
}

void calculateLineCoefficients(float* a, float* b, vector<vector<Point>> contours) {
	float alfa;
	RotatedRect rect;

	for (int i = 0; i < contours.size(); i++) {
		rect = minAreaRect(contours[i]);
		if (rect.size.width < rect.size.height){
			alfa = rect.angle + 180;
		}
		else{
			alfa = rect.angle + 90;
		}

		if ((int)alfa == 0 || (int)alfa == 180) {  // in this case, line is parallel to y axis
			a[i] = 0;
			b[i] = 0;
		}
		else if ((int)alfa == 90) {
			a[i] = 0;
			b[i] = rect.center.y;
		}
		else {
			a[i] = -(cos(alfa*3.14159 / 180) / sin(alfa*3.14159 / 180));
			b[i] = rect.center.y + rect.center.x * (sin((90 - alfa)*3.14159 / 180) / cos((90 - alfa)*3.14159 / 180));
		}

	}
}

void calculatePointsOfIntersection(vector<vector<Point>> contours, int* x, int* y, float* a, float* b, Mat image) {
	int tempX;
	int tempY;
	bool contains;

	for (int i = 0; i < contours.size(); i++) {   // current line
		for (int j = 0; j < contours.size(); j++) { // other lines
			if (i != j) {  // avoid compare with itself

				/****** parallel lines or(horizontal + vertical line)  *****/
				if (a[i] == a[j]) { 
					if (a[i] != 0 && a[j] != 0) { // parallel lines
						continue;
					}
					else if ((b[i] == 0 && b[j] == 0)
						|| (b[i] != 0 && b[j] != 0)){ // parallel vertical or parallel horizontal lines
						continue;
					}
					else {  // horizontal + vertical
						if (b[i] == 0 && b[j] != 0) { // current line is vertical
							tempX = minAreaRect(contours[i]).center.x;
							tempY = b[j];
						}
						else {   // other line is vertical
							tempX = minAreaRect(contours[j]).center.x;
							tempY = b[i];
						}

					}
				}

				/****** no parallel lines  *****/
				else {
					if (a[i] == 0 && b[i] == 0) {   // current line is vertical and the other is inclined
						tempX = minAreaRect(contours[i]).center.x;
						tempY = a[j] * tempX + b[j];
					}
					else if (a[j] == 0 && b[j] == 0) {  // current line is inclined and the other line is vertical
						tempX = minAreaRect(contours[j]).center.x;
						tempY = a[i] * tempX + b[i];
					}
					else {  // the most common case
						tempX = (b[j] - b[i]) / (a[i] - a[j]);
						tempY = a[i] * tempX + b[i];
					}
				}

				// check if there is already similar point
				contains = false;
				for (int k = 0; k < 4; k++) {
					if (abs(x[k] - tempX) < 30 && abs(y[k] - tempY) < 30) {
						contains = true;
					}
				}

				// save point of intersection only if result array does not contain this point
				// and, of course, this point is inside image 
				if (!contains
					&& tempX > 10 && tempY > 10
					&& tempX < image.cols - 10 && tempY < image.rows - 10) {
					for (int k = 0; k < 4; k++) {
						if (x[k] == 0) {   // check if it won't overwrite existing point 
							x[k] = tempX;
							y[k] = tempY;
							break;
						}
					}
				}
			}
		}
	}
}

vector<int> calculateWidthAndHeight(vector<Point> points) {
	vector<int> result;
	if (points.size() != 4) {
		return result;
	}
	
	int width = sqrt(pow(abs(points[0].x - points[1].x), 2) + pow(abs(points[0].y - points[1].y), 2));
	int height = sqrt(pow(abs(points[0].x - points[3].x), 2) + pow(abs(points[0].y - points[3].y), 2));
	result.push_back(width);
	result.push_back(height);

	return result;
}