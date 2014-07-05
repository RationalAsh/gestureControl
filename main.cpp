/*
    OK. List of things to implement:

    1. Automatic thresholding
    2. Save thresholds in thresholds vector
    3. Add in animated trackbars
    4. Apply convexity defects to find number of fingers
*/
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
//#include "gestureDetector.h"

#define CAMERA 0
#define VWIDTH 480
#define VHEIGHT 640
#define VERTICAL 1
#define THRESHES 6
#define ROIWIDTH 10

using namespace cv;
using namespace std;

//Class that holds regions of interest
//Colors are sampled from these regions
//I'll declare a vector of these
class roi
{
public:
    roi(Point p1, Point p2);
    roi();
    Point pt1;
    Point pt2;
};

roi::roi()
{
    pt1 = Point(100, 100);
    pt2 = Point(110, 110);
}

roi::roi(Point p1, Point p2)
{
    pt1 = p1;
    pt2 = p2;
}

//Class to hold thresholds for each sample
//I'll declare a vector of this too.
class threshes
{
public:
    threshes();
    threshes(Scalar lower_th, Scalar upper_th);
    Scalar lower;
    Scalar upper;
};

threshes::threshes()
{
    lower = Scalar(0,0,0);
    upper = Scalar(255,255,255);
}

threshes::threshes(Scalar lower_th, Scalar upper_th)
{
    lower = lower_th;
    upper = upper_th;
}

//Global variables
roi click(Point(100, 100), Point(110, 110));
roi r1(Point(100, 100), Point(120, 120));
vector<roi> samples;
vector<threshes> thresholds(6, threshes());
Mat region;
//vector<roi> samples2(6, roi());
int clickCount = 0;

int max_thresh = 255;
int hue_lower  = 0;   int sat_lower = 0;   int val_lower = 0;
int hue_upper  = 255; int sat_upper = 255; int val_upper = 255;

//Function to write text to image
void printText(Mat src, string text)
{
    int fontFace = FONT_HERSHEY_PLAIN;
    putText(src, text, Point(src.cols/2, src.rows/10),
	    fontFace, 1.2f, Scalar(200, 0, 0), 2);
}

//function to draw rectangles at rois
void drawRects(Mat &img)
{
    unsigned int i;
    for(i=0; i<samples.size(); i++)
    {
	rectangle(img, samples[i].pt1, samples[i].pt2,
		  Scalar(0, 255, 0));
    }
}

void getCentroid(Mat &thresholded_image, Point &Centroid, int &Area)
{
    ///The object that holds all the centroids.
    ///Pass in the image. The boolean true tells the function that 
    ///the image is binary
    Moments m = moments(thresholded_image, true);
    ///Moment along x axis
    double M10 = m.m10;
    ///Moment along y-axis;
    double M01 = m.m01;
    ///Area
    double M00 = m.m00;
    Centroid.x  = int(M10/M00);
    Centroid.y  = int(M01/M00);
    Area        = int(M00);
}

int getArea(Mat &thresholded_image)
{
    Moments m = moments(thresholded_image, true);
    return int(m.m00);
}

void updateThresholds(Mat &frame, Mat &threshImg, Mat &camfeed, 
		      VideoCapture &camera)
{
    int regionArea;
    char keypress;
}

//Mouse callback function which is fired when a mouse event
//Occurs over a namedWindow
void mousecb(int event, int x, int y, int flags, void* userdata)
{
    Mat* data = (Mat*)userdata;
    Mat img = *data;

    if((event == EVENT_LBUTTONDOWN)&&(clickCount < 6))
    {
	cout<<"Left mouse button pressed at ("<<x<<","<<y<<")\n";
	samples.push_back(roi(Point(x, y), Point(x+ROIWIDTH, y+ROIWIDTH)));
	cout<<"Vector Size: "<<samples.size()<<endl;
	clickCount++;
    }
}

int main(int argc, char** argv)
{
    Mat frame,  hsv, 
	thresh2, camfeed;
    Mat thresh(VWIDTH, VHEIGHT, CV_8U, Scalar(0));

    vector<Mat> binaries(6, Mat(VWIDTH, VHEIGHT, CV_8U, Scalar(0)));
    imshow("vector", binaries[0]);
    //Camera object
    VideoCapture camera;
    Scalar test(20 ,30, 40);
    int counter    = 0;
    
    namedWindow("camfeed");
    namedWindow("rgb_thresh");
    
    setMouseCallback("camfeed", mousecb, &frame);
    //Trackbars for thresholding
    createTrackbar("Hue upper", "camfeed", &hue_upper, max_thresh, NULL);
    createTrackbar("Hue lower", "camfeed", &hue_lower, max_thresh, NULL);
    createTrackbar("Sat upper", "camfeed", &sat_upper, max_thresh, NULL);
    createTrackbar("Sat lower", "camfeed", &sat_lower, max_thresh, NULL);
    createTrackbar("Val upper", "camfeed", &val_upper, max_thresh, NULL);
    createTrackbar("Val lower", "camfeed", &val_lower, max_thresh, NULL);

    
    camera.open(CAMERA);
    if(!camera.isOpened())
    {
	cerr<<"ERROR: COULD NOT ACCESS CAMERA!";
	exit(1);
    }
    //Setting came resolution
    camera.set(CV_CAP_PROP_FRAME_WIDTH, VWIDTH);
    camera.set(CV_CAP_PROP_FRAME_WIDTH, VHEIGHT);

    while(1)
    {
	camera >> frame;
	flip(frame, frame, VERTICAL);
	frame.copyTo(camfeed);
	//rectangle(camfeed, r1.pt1, r1.pt2,
	//	  Scalar(0, 255, 0));
	drawRects(camfeed);
	printText(camfeed, "Hello World!");
	imshow("camfeed", camfeed);

	thresh = Mat(VWIDTH, VHEIGHT, CV_8U, Scalar(0));
	for(int i=0; i<samples.size(); i++)
	{
	    //Get the median of regions of interest
	    //Get the mean value of regions of interest
	    //Get threshold based on that
	    region = frame(Rect(samples[i].pt1, samples[i].pt2));
	    Scalar mean, stdev;
	    meanStdDev(region, mean, stdev);
	    inRange(frame, mean-stdev*1.9, mean+stdev*1.9, binaries[i]);
	    add(binaries[i], thresh, thresh);
	}
	
	medianBlur(thresh, thresh, 11);
	//region = frame(Rect(100, 100, 10, 10));
	
	//meanStdDev(region, mean, stdev);
	//cout<<"mean: "<<mean<<"stdev: "<<stdev<<endl;
	//imshow("region", region);
	//inRange(frame, Scalar(hue_lower, sat_lower, val_lower),
	//	Scalar(hue_upper, sat_upper, val_upper), thresh2);
	//inRange(frame, mean - stdev*1.5, mean + stdev*1.5, thresh2);
        //medianBlur(thresh2, thresh2, 7);
	//Point cent;
	//int area;
	//getCentroid(thresh2, cent, area);
	//area = getArea(thresh2);
	//cout<<"white: "<<area/(VWIDTH*VHEIGHT*1.0)<<endl;
	//imshow("rgb_thresh", thresh2);
	
	//imshow("one", binaries[0]);
	//imshow("two", binaries[1]);
	imshow("thresh", thresh);    
	char keypress = waitKey(10);

	

	if(keypress == 27) break;
    }

    return 0;
}
