//
//  opencvUtilities.cpp
//  ARToolKit5
//
//  Created by Hao Yue Sheng on 2016-01-09.
//
//

#include "opencvUtilities.hpp"

ColoredObj opencvUtilities::gScrews[MAX_COLOUR_NUM];
string opencvUtilities::gScrewTypes[MAX_COLOUR_NUM] = {"red", "green", "yellow"};

string opencvUtilities::intToString(int number){
    std::stringstream ss;
    ss << number;
    return ss.str();
}

void opencvUtilities::drawObject(vector<ColoredObj> objs,Mat &frame, bool calibrateMode){
    
    for (int i = 0; i < objs.size(); i++) {
        cv::circle(frame,cv::Point(objs.at(i).getxPos(),objs.at(i).getyPos()),10,cv::Scalar(0,0,255));
        cv::putText(frame,intToString(objs.at(i).getxPos())+ " , " + intToString(objs.at(i).getyPos()),cv::Point(objs.at(i).getxPos(),objs.at(i).getyPos()+20),1,1,Scalar(0,255,0));
        if (!calibrateMode) {
            cv::putText(frame, objs.at(i).getType(), cv::Point(objs.at(i).getxPos(),objs.at(i).getyPos()-30),1,2,Scalar(0,255,0));
        }
    }
}

void opencvUtilities::morphOps(Mat &thresh){
    
    //create structuring element that will be used to "dilate" and "erode" image.
    //the element chosen here is a 3px by 3px rectangle
    
    Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    //dilate with larger element so make sure object is nicely visible
    Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));
    
    erode(thresh,thresh,erodeElement);
    erode(thresh,thresh,erodeElement);
    
    
    dilate(thresh,thresh,dilateElement);
    dilate(thresh,thresh,dilateElement);
    
}

void opencvUtilities::trackFilteredObject(int objectIndex, Mat threshold, Mat &cameraFeed){
    
    vector<ColoredObj> coloredObjs;
    
    Mat temp;
    threshold.copyTo(temp);
    //these two vectors needed for output of findContours
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    //use moments method to find our filtered object
    if (hierarchy.size() > 0) {
        int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
            int count = 0;
            for (int index = 0; index >= 0; index = hierarchy[index][0]) {
                Moments moment = moments((cv::Mat)contours[index]);
                double area = moment.m00;
                
                //if the area is less than 20 px by 20px then it is probably just noise
                //if the area is the same as the 3/2 of the image size, probably just a bad filter
                //we only want the object with the largest area so we safe a reference area each
                //iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA){
                    ColoredObj obj(gScrews[objectIndex].getType());
                    obj.setxPos(moment.m10/area);
                    obj.setyPos(moment.m01/area);
                    
                    coloredObjs.push_back(obj);
                    count++;
                    gScrews[objectIndex].objectFound = true;
                    
                } else
                    gScrews[objectIndex].objectFound = false;
                
                
            }
            
            gScrews[objectIndex].setNumbersToShow(count);
            
            //let user know you found an object
            if(gScrews[objectIndex].objectFound == true){
                //draw object location on screen
                drawObject(coloredObjs, cameraFeed, false);
            }
            
        }else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
    }
}