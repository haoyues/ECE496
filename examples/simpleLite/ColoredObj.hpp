//
//  ColoredObj.hpp
//  ARToolKit5
//
//  Created by Chu Xi Huang on 2016-01-04.
//
//

#ifndef ColoredObj_hpp
#define ColoredObj_hpp

#include <stdio.h>
#include <string.h>
#include "opencv/cv.h"

using namespace std;
using namespace cv;

class ColoredObj {
public:
    ColoredObj();
    ColoredObj(string type);
    ~ColoredObj();
    
    string getType();
    
    int getxPos();
    int getyPos();
    
    void setxPos(int x);
    void setyPos(int y);
    
    Scalar getBGRmin();
    Scalar getBGRmax();
    
    void setBGRmin(Scalar BGR);
    void setBGRmax(Scalar BGR);
    
private:
    int xPos, yPos;
    string type;
    Scalar BGRmin, BGRmax;
};

#endif /* ColoredObj_hpp */
