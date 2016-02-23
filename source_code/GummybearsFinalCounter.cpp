#include "GummybearsFinalCounter.h"
#include "CornersForTransform.h"
#include "ColorfulGummybear.h"
#include <math.h>

int countColorfulContours(vector<vector<Point>> colorfulContours, Mat image) {
	int result = 0;
	if (colorfulContours.size() == 0) { // frequent situation, so let's minimize time
		return result;
	}
	int indexes[100];  // 0 - contour doesn't matter, 1 - contour found gummybear
	for (int i = 0; i < 100; i++) {
		indexes[i] = 0;
	}
	float ratio;
	int numberOfPixels = image.rows * image.cols;
	int sum = 0;
	for (int i = 0; i < colorfulContours.size(); i++) {
		if (i > 99) {
			break;
		}
		indexes[i] = 1;
		ratio = numberOfPixels / contourArea(colorfulContours[i]);
	 
		if (colorfulContours[i].size() == 4) {
			indexes[i] = 0;     // reject if it is square
		}
		
		if (ratio > 700) {
			indexes[i] = 0;   // reject too small contours
		}
		else if (ratio < 95 && ratio > 60) {
			indexes[i] = 2;   // current contour contains 2 gummybears
		}
		else if (ratio < 61 && ratio > 35) {
			indexes[i] = 3;   // current contour contains 3 gummybears
		}
		else if (ratio < 36 && ratio > 20) {
			indexes[i] = 4;   // current contour contains 4 gummybears
		}
		else if (ratio < 21 && ratio > 15) {
			indexes[i] = 5;   // hope there are 5 gummybears for real
		}
		else if (ratio < 16) {
			indexes[i] = 6;  // hope there are 6 gummybears for real
		}
	}
	
	for (int i = 0; i < 100; i++) {
		sum += indexes[i];
	}
	result = sum;
	return result;
}

void countGummybears(Mat transformedImageColor, Mat transformedImageGray, int* result) {
	vector<vector<Point>> contoursOfGummyBears;

	findDarkRed(transformedImageColor, transformedImageGray);
	erode(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(5, 5)));
	dilate(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(16, 16)));
	findContours(transformedImageGray, contoursOfGummyBears, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int numberOfDarkRed = countColorfulContours(contoursOfGummyBears, transformedImageColor);
	
	findBrightRed(transformedImageColor, transformedImageGray);
	erode(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(6, 6)));
	dilate(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(15, 15)));
	findContours(transformedImageGray, contoursOfGummyBears, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int numberOfBrightRed = countColorfulContours(contoursOfGummyBears, transformedImageColor);


	findGreen(transformedImageColor, transformedImageGray);
	erode(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(6, 6)));
	dilate(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(14, 14)));
	findContours(transformedImageGray, contoursOfGummyBears, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int numberOfGreen = countColorfulContours(contoursOfGummyBears, transformedImageColor);
	
	findOrange(transformedImageColor, transformedImageGray);
	erode(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(6, 6)));
	dilate(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(14, 14)));
	findContours(transformedImageGray, contoursOfGummyBears, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int numberOfOrange = countColorfulContours(contoursOfGummyBears, transformedImageColor);

	findWhite(transformedImageColor, transformedImageGray);
	erode(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(3, 3)));
	dilate(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(11, 11)));
	findContours(transformedImageGray, contoursOfGummyBears, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int numberOfWhite = countColorfulContours(contoursOfGummyBears, transformedImageColor);	

	findYellow(transformedImageColor, transformedImageGray);
	erode(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(6, 6)));
	dilate(transformedImageGray, transformedImageGray, getStructuringElement(MORPH_RECT, Size(14, 14)));
	findContours(transformedImageGray, contoursOfGummyBears, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int numberOfYellow = countColorfulContours(contoursOfGummyBears, transformedImageColor);
	
	result[0] = numberOfDarkRed;
	result[1] = numberOfBrightRed;
	result[2] = numberOfGreen;
	result[3] = numberOfOrange;
	result[4] = numberOfWhite;
	result[5] = numberOfYellow;
}