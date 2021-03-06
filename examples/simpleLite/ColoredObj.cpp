//
//  ColoredObj.cpp
//  ARToolKit5
//
//  Created by Chu Xi Huang on 2016-01-04.
//
//

#include "ColoredObj.hpp"

ColoredObj::ColoredObj() {

}

ColoredObj::ColoredObj(string type_) {
    type = type_;
    
    if (type_ == "red") {
        setBGRmin(Scalar(0, 0, 140));
        setBGRmax(Scalar(100, 93, 255));
    } else if (type_ == "green") {
        setBGRmin(Scalar(0, 77, 0));
        setBGRmax(Scalar(88, 144, 81));
    } else if (type_ == "yellow") {
        setBGRmin(Scalar(0, 123, 0));
        setBGRmax(Scalar(69, 255, 255));
    } else if (type_ == "orange") {
        setBGRmin(Scalar(15, 10, 183));
        setBGRmax(Scalar(84, 255, 255));
    } else if (type_ == "blue") {
        setBGRmin(Scalar(92, 1, 0));
        setBGRmax(Scalar(255, 113, 67));
    } else if (type_ == "purple") {
        setBGRmin(Scalar(101, 65, 81));
        setBGRmax(Scalar(156, 103, 133));
    } else if (type_ == "light green") {
        setBGRmin(Scalar(51, 73, 10));
        setBGRmax(Scalar(123, 135, 82));
    }
}

ColoredObj::~ColoredObj() {
    
}

string ColoredObj::getType() {
    return type;
}

void ColoredObj::setType(string type_) {
    type = type_;
    
    if (type_ == "red") {
        setBGRmin(Scalar(0, 0, 140));
        setBGRmax(Scalar(100, 93, 255));
    } else if (type_ == "green") {
        setBGRmin(Scalar(0, 77, 0));
        setBGRmax(Scalar(88, 144, 81));
    } else if (type_ == "yellow") {
        setBGRmin(Scalar(0, 123, 0));
        setBGRmax(Scalar(69, 255, 255));
    }
}

int ColoredObj::getxPos() {
    return xPos;
}

int ColoredObj::getyPos() {
    return yPos;
}

void ColoredObj::setxPos(int x) {
    xPos = x;
}

void ColoredObj::setyPos(int y) {
    yPos = y;
}

void ColoredObj::setBGRmax(Scalar BGR) {
    BGRmax = BGR;
}

void ColoredObj::setBGRmin(Scalar BGR) {
    BGRmin = BGR;
}

Scalar ColoredObj::getBGRmax() {
    return BGRmax;
}

Scalar ColoredObj::getBGRmin() {
    return BGRmin;
}
