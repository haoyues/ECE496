//
//  opencvUtilities.hpp
//  ARToolKit5
//
//  Created by Hao Yue Sheng on 2016-01-09.
//
//

#ifndef opencvUtilities_hpp
#define opencvUtilities_hpp

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/types_c.h"
#include "opencv2/xphoto.hpp"
#include "ColoredObj.hpp"

#define MAX_NUM_OBJECTS         50
#define MIN_OBJECT_AREA         40*40

string intToString(int number);
void drawObject(vector<ColoredObj> objs,Mat &frame, bool calibrateMode);
void morphOps(Mat &thresh);
void trackFilteredObject(ColoredObj coloredObj, Mat threshold, Mat &cameraFeed);

#endif /* opencvUtilities_hpp */
