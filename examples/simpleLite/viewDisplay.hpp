//
//  viewDisplay.hpp
//  ARToolKit5
//
//  Created by Hao Yue Sheng on 2016-01-07.
//
//

#ifndef viewDisplay_hpp
#define viewDisplay_hpp

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#  define snprintf _snprintf
#endif
#include <stdlib.h>					// malloc(), free()
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#include <OpenGl/glext.h>

#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>			// arParamDisp()
#include <AR/ar.h>
#include <AR/gsub_lite.h>

#include "objloader.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/types_c.h"
#include "opencv2/xphoto.hpp"
#include "ColoredObj.hpp"


#define VIEW_SCALEFACTOR		1.0 
#define VIEW_DISTANCE_MIN		40.0        // Objects closer to the camera than this will not be displayed. OpenGL units.
#define VIEW_DISTANCE_MAX		10000.0     // Objects further away from the camera than this will not be displayed. OpenGL units.

//default capture width and height
#define FRAME_WIDTH             640
#define FRAME_HEIGHT            480
#define MAX_OBJECT_AREA         FRAME_HEIGHT*FRAME_WIDTH/1.5

extern ARHandle		*gARHandle;
extern int gShowMode;

extern int windowWidth;					// Initial window width, also updated during program execution.
extern int windowHeight;                  // Initial window height, also updated during program execution.

extern ARUint8		*gARTImage;
extern ARGL_CONTEXT_SETTINGS_REF gArglSettings;
extern ARParamLT *gCparamLT;
// Image acquisition.
extern int          gARTImageSavePlease;
extern long			gCallCountMarkerDetect;
extern AR3DHandle	*gAR3DHandle;

extern int NUM_OF_MARKER;
extern ARMarker *gMarkers;

extern bool rotated;
extern bool scaled;
extern double modelScaleFactor;
extern double modelRotateAngle;

extern float gDrawRotateTableAngle;     // For flipping table face.
extern int gDrawRotate;
extern int gStartAnimation;
extern int gDrawTableFace;
extern int gDrawTableLeg1;
extern int gDrawTableLeg2;
extern int gDrawTableLeg3;
extern int gDrawTableLeg4;
extern int gDrawTranslateLeg1;
extern int gDrawTranslateLeg2;
extern int gDrawTranslateLeg3;
extern int gDrawTranslateLeg4;
extern float gDrawRotateAngle;			// For use in drawing.

extern float gDrawTranslateDistanceLeg1;
extern float gDrawTranslateDistanceLeg2;
extern float gDrawTranslateDistanceLeg3;
extern float gDrawTranslateDistanceLeg4;

extern int gCounter;

void View1_Display(void);
void View2_Display(void);
void View3_Display(void);
void loadObjects(void);
int setupMarker(char *filename);

#endif /* viewDisplay_hpp */
