#include "Contours.h"
#include "CornersForTransform.h"
#include "ColorfulGummybear.h"
#include "GummybearsFinalCounter.h"
#include <iostream>
#include <fstream>

vector<int> numberOfDarkRed;
vector<int> numberOfBrightRed;
vector<int> numberOfGreen;
vector<int> numberOfOrange;
vector<int> numberOfWhite;
vector<int> numberOfYellow;
vector<int> resultForEveryScene;
vector<vector<int>> finalResult;
int sceneNumberToWrite = 1;

string previousSceneNumberString;
string sceneNumberString;
string frameNumberString;
int previousSceneNumber;
int sceneNumber;
int frameNumber;
string filePath;
Mat image;
Mat imageGray;
Mat imageColor;
Mat transformedImageGray;
Mat transformedImageColor;

vector<vector<Point>> markerContours;
vector<Point> sourcePoints;
vector<int> dimensions;
int width;
int height;
float dimRatio;
Point2f sources[4];
Point2f destinations[4];

int finalNumbersOfGummybears[6];
int sumOfDarkRed;
int sumOfBrightRed;
int sumOfGreen;
int sumOfOrange;
int sumOfWhite;
int sumOfYellow;

void fillResultLine();

int main(int, char)
{
	double t = (double)getTickCount();

	string line;
	ifstream sourceFile("pictures_names/pictures_names.txt");
	vector<string> fileNames;
	
	if (sourceFile.is_open()) {
		while (getline(sourceFile, line)) {
			fileNames.push_back(line);
		}
		sourceFile.close();
	}
	sort(fileNames.begin(), fileNames.end());

	std::cout << "Processing, please wait... It may take about " << fileNames.size() * 2 << " seconds." << endl;
	
	/**********   MAIN LOOP   **********/
	for (int indexOfImage = 0; indexOfImage < fileNames.size(); indexOfImage++) {

		if (fileNames[indexOfImage].length() < 20) {
			continue;        // jump, if there is something like "" etc.
		}

		sceneNumberString = fileNames[indexOfImage].substr(6, 3);
		frameNumberString = fileNames[indexOfImage].substr(16, 3);		
	
		sceneNumber = stoi(sceneNumberString);
		frameNumber = stoi(frameNumberString);

		if (frameNumber > 10) {  // 10 frames is enough
			continue;
		}

		previousSceneNumber = sceneNumber;
		if (indexOfImage > 0) {
			if (fileNames[indexOfImage - 1].length() > 10) {
				previousSceneNumberString = fileNames[indexOfImage - 1].substr(6, 3);
				previousSceneNumber = stoi(previousSceneNumberString);
			}
		}

		if (sceneNumber != previousSceneNumber) {
			if (numberOfDarkRed.size() == 0) {   // if scene wasn't processed well at all, fill the vector with the random number.
				numberOfDarkRed.push_back(3);
				numberOfBrightRed.push_back(3);
				numberOfGreen.push_back(3);
				numberOfOrange.push_back(3);
				numberOfWhite.push_back(3);
				numberOfYellow.push_back(3);
			}
			fillResultLine();
		}

		filePath = "images/" + fileNames[indexOfImage];
		image = imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);
		imageGray = image.clone();
		imageColor = imread(filePath);
		
		markerContours = findMarkers(imageColor, image);
		if (markerContours.size() != 4) {
			continue;
		}

		bool areThereRectangles = areRectangularMarkers(markerContours);
		if (areThereRectangles) {
			sourcePoints = findCornersRectangularShape(markerContours, imageColor);
		}
		else {
			sourcePoints = findCornersNonRectangularShape(markerContours);
		}

		if (sourcePoints.size() != 4) {
			continue;
		}

		// check if potential points are too close to each other
		vector<int> pointX;
		vector<int> pointY;
		for (int i = 0; i < 4; i++) {
			pointX.push_back(sourcePoints[i].x);
			pointY.push_back(sourcePoints[i].y);
		}
		sort(pointX.begin(), pointX.end());
		sort(pointY.begin(), pointY.end());
		if (abs(pointX[2] - pointX[0]) < 50 || abs(pointX[3] - pointX[1]) < 50
			|| abs(pointY[2] - pointY[0]) < 50 || abs(pointY[3] - pointY[1]) < 50) {
			continue;
		}

		dimensions = calculateWidthAndHeight(sourcePoints);
		width = dimensions[0];
		height = dimensions[1];
		dimRatio = (float)max(width, height) / (float)min(width, height);
		if (dimRatio > 3) {  // something wrong could happen during calculating points
			continue;
		}

		for (int i = 0; i < 4; i++) {
			sources[i] = sourcePoints[i];
		}

		destinations[0].x = 0;			destinations[0].y = 0;
		destinations[1].x = width / 2;	destinations[1].y = 0;
		destinations[2].x = width / 2;	destinations[2].y = height / 2;
		destinations[3].x = 0;			destinations[3].y = height / 2;

		warpPerspective(imageColor, transformedImageColor, getPerspectiveTransform(sources, destinations), Size(width / 2, height / 2));
		warpPerspective(imageGray, transformedImageGray, getPerspectiveTransform(sources, destinations), Size(width / 2, height / 2));
		
		countGummybears(transformedImageColor, transformedImageGray, finalNumbersOfGummybears);

		numberOfDarkRed.push_back(finalNumbersOfGummybears[0]);
		numberOfBrightRed.push_back(finalNumbersOfGummybears[1]);
		numberOfGreen.push_back(finalNumbersOfGummybears[2]);
		numberOfOrange.push_back(finalNumbersOfGummybears[3]);
		numberOfWhite.push_back(finalNumbersOfGummybears[4]);
		numberOfYellow.push_back(finalNumbersOfGummybears[5]);
	}

	if (numberOfDarkRed.size() == 0) {   // if the last scene wasn't processed well at all, fill the vector with the random number.
		numberOfDarkRed.push_back(3);
		numberOfBrightRed.push_back(3);
		numberOfGreen.push_back(3);
		numberOfOrange.push_back(3);
		numberOfWhite.push_back(3);
		numberOfYellow.push_back(3);
	}
	fillResultLine();   // for the last scene

	ofstream resultFile("results/Pienkowski_Rafal.txt");

	if (resultFile.is_open()) {
		resultFile << "DarkRed, BrightRed, Green, Orange, White, Yellow \n";
		for (int i = 0; i < finalResult.size(); i++) {
			resultFile << finalResult[i][0] << ", " <<
				finalResult[i][1] << ", " <<
				finalResult[i][2] << ", " <<
				finalResult[i][3] << ", " <<
				finalResult[i][4] << ", " <<
				finalResult[i][5] << "\n";
		}
	}
	resultFile.close();

	t = (((double)getTickCount() - t) / getTickFrequency());
	cout << endl << "processed time [s]: " << t << endl;

	return 0;
}

void fillResultLine() {
	sort(numberOfDarkRed.begin(), numberOfDarkRed.end());
	sort(numberOfBrightRed.begin(), numberOfBrightRed.end());
	sort(numberOfGreen.begin(), numberOfGreen.end());
	sort(numberOfOrange.begin(), numberOfOrange.end());
	sort(numberOfWhite.begin(), numberOfWhite.end());
	sort(numberOfYellow.begin(), numberOfYellow.end());

	// if there is more than 2 element, erase first and last element
	if (numberOfDarkRed.size() > 2) {
		numberOfDarkRed.pop_back(); numberOfDarkRed.erase(numberOfDarkRed.begin());
		numberOfBrightRed.pop_back(); numberOfBrightRed.erase(numberOfBrightRed.begin());
		numberOfGreen.pop_back(); numberOfGreen.erase(numberOfGreen.begin());
		numberOfOrange.pop_back(); numberOfOrange.erase(numberOfOrange.begin());
		numberOfWhite.pop_back(); numberOfWhite.erase(numberOfWhite.begin());
		numberOfYellow.pop_back(); numberOfYellow.erase(numberOfYellow.begin());
	}

	sumOfDarkRed = 0;
	sumOfBrightRed = 0;
	sumOfGreen = 0;
	sumOfOrange = 0;
	sumOfWhite = 0;
	sumOfYellow = 0;
	for (int i = 0; i < numberOfDarkRed.size(); i++) {
		sumOfDarkRed += numberOfDarkRed[i];
		sumOfBrightRed += numberOfBrightRed[i];
		sumOfGreen += numberOfGreen[i];
		sumOfOrange += numberOfOrange[i];
		sumOfWhite += numberOfWhite[i];
		sumOfYellow += numberOfYellow[i];
	}

	resultForEveryScene.clear();

	// put average value for every color
	resultForEveryScene.push_back(round((float)sumOfDarkRed / (float)numberOfDarkRed.size()));
	resultForEveryScene.push_back(round((float)sumOfBrightRed / (float)numberOfDarkRed.size()));
	resultForEveryScene.push_back(round((float)sumOfGreen / (float)numberOfDarkRed.size()));
	resultForEveryScene.push_back(round((float)sumOfOrange / (float)numberOfDarkRed.size()));
	resultForEveryScene.push_back(round((float)sumOfWhite / (float)numberOfDarkRed.size()));
	resultForEveryScene.push_back(round((float)sumOfYellow / (float)numberOfDarkRed.size()));

	finalResult.push_back(resultForEveryScene);

	numberOfDarkRed.clear();
	numberOfBrightRed.clear();
	numberOfGreen.clear();
	numberOfOrange.clear();
	numberOfWhite.clear();
	numberOfYellow.clear();
}