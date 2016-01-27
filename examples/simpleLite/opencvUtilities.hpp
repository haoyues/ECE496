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
#include <OpenGL/gl.h>

#define MAX_NUM_OBJECTS         50
#define MIN_OBJECT_AREA         40*40
#define MAX_COLOUR_NUM          3

class opencvUtilities {
public:
    
    static ColoredObj gScrews[MAX_COLOUR_NUM];
    static string gScrewTypes[MAX_COLOUR_NUM];
    static GLuint gTextures[MAX_COLOUR_NUM];
    static GLuint silverTexture;
    
    static void loadScrewsTexture();
    static string intToString(int number);
    static void drawObject(vector<ColoredObj> objs,Mat &frame, bool calibrateMode);
    static void morphOps(Mat &thresh);
    static void trackFilteredObject(int objectIndex, Mat threshold, Mat &cameraFeed);
    
};

#endif /* opencvUtilities_hpp */
