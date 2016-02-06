/*
 *  simpleLite.c
 *
 *  Some code to demonstrate use of ARToolKit.
 *
 *  Press '?' while running for help on available key commands.
 *
 *  Disclaimer: IMPORTANT:  This Daqri software is supplied to you by Daqri
 *  LLC ("Daqri") in consideration of your agreement to the following
 *  terms, and your use, installation, modification or redistribution of
 *  this Daqri software constitutes acceptance of these terms.  If you do
 *  not agree with these terms, please do not use, install, modify or
 *  redistribute this Daqri software.
 *
 *  In consideration of your agreement to abide by the following terms, and
 *  subject to these terms, Daqri grants you a personal, non-exclusive
 *  license, under Daqri's copyrights in this original Daqri software (the
 *  "Daqri Software"), to use, reproduce, modify and redistribute the Daqri
 *  Software, with or without modifications, in source and/or binary forms;
 *  provided that if you redistribute the Daqri Software in its entirety and
 *  without modifications, you must retain this notice and the following
 *  text and disclaimers in all such redistributions of the Daqri Software.
 *  Neither the name, trademarks, service marks or logos of Daqri LLC may
 *  be used to endorse or promote products derived from the Daqri Software
 *  without specific prior written permission from Daqri.  Except as
 *  expressly stated in this notice, no other rights or licenses, express or
 *  implied, are granted by Daqri herein, including but not limited to any
 *  patent rights that may be infringed by your derivative works or by other
 *  works in which the Daqri Software may be incorporated.
 *
 *  The Daqri Software is provided by Daqri on an "AS IS" basis.  DAQRI
 *  MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 *  THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE, REGARDING THE DAQRI SOFTWARE OR ITS USE AND
 *  OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 *
 *  IN NO EVENT SHALL DAQRI BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 *  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 *  MODIFICATION AND/OR DISTRIBUTION OF THE DAQRI SOFTWARE, HOWEVER CAUSED
 *  AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 *  STRICT LIABILITY OR OTHERWISE, EVEN IF DAQRI HAS BEEN ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  Copyright 2015 Daqri LLC. All Rights Reserved.
 *  Copyright 2002-2015 ARToolworks, Inc. All Rights Reserved.
 *
 *  Author(s): Philip Lamb.
 *
 */

// ============================================================================
//	Includes
// ============================================================================

#include "viewDisplay.hpp"
#include "opencvUtilities.hpp"
#include "drawUtilities.hpp"
#include "shader.hpp"
//#include <gtk/gtk.h>

using namespace cv;
using namespace glm;
// ============================================================================
//	Constants
// ============================================================================

#define MAX_MARKER_NUM          60
#define NUM_OF_NUMBER           10
// ============================================================================
//	Global variables
// ============================================================================

static ARPattHandle	*gARPattHandle = NULL;
static int gShowHelp = 1;
static int windowed = TRUE;                     // Use windowed (TRUE) or fullscreen mode (FALSE) on launch.
static int windowDepth = 32;					// Fullscreen mode bit depth.
static int windowRefresh = 0;					// Fullscreen mode refresh rate. Set to 0 to use default rate.

ARHandle *gARHandle = NULL;
int gShowMode = 1;

int windowWidth = 1024;					// Initial window width, also updated during program execution.
int windowHeight = 768;                  // Initial window height, also updated during program execution.

ARUint8	*gARTImage = NULL;
ARGL_CONTEXT_SETTINGS_REF gArglSettings = NULL;
ARParamLT *gCparamLT = NULL;

// Image acquisition
int gARTImageSavePlease = FALSE;
long CallCountMarkerDetect = 0;
long gCallCountMarkerDetect = 0;
AR3DHandle *gAR3DHandle = NULL;

/********************** MARKER **********************/
int NUM_OF_MARKER = 0;
ARMarker *gMarkers;
/********************** MARKER **********************/

/********************** ANIMATION **********************/
bool rotated = false;
bool scaled = false;
double modelScaleFactor = 1.0;
double modelRotateAngle = 0.0;

float gDrawRotateTableAngle = 0;     // For flipping table face.
int gDrawRotate = TRUE;
int gStartAnimation = FALSE;
int gDrawTableFace = FALSE;
int gDrawTableLeg1 = FALSE;
int gDrawTableLeg2 = FALSE;
int gDrawTableLeg3 = FALSE;
int gDrawTableLeg4 = FALSE;
int gDrawTranslateLeg1 = TRUE;
int gDrawTranslateLeg2 = FALSE;
int gDrawTranslateLeg3 = FALSE;
int gDrawTranslateLeg4 = FALSE;
float gDrawRotateAngle = 0;			// For use in drawing.

float gDrawTranslateDistanceLeg1 = 0;
float gDrawTranslateDistanceLeg2 = 0;
float gDrawTranslateDistanceLeg3 = 0;
float gDrawTranslateDistanceLeg4 = 0;

int gCounter = 0;

bool welcomed = false;
bool windowNotSplit = true;
/********************** ANIMATION **********************/


/********************** SHADER **********************/
GLuint programID;
GLuint vertexPosition;
GLuint vertexUVID;
GLuint TextureID;
/********************** SHADER **********************/

/********************** CREATE MULTIPLE VIEWS ********************/
GLuint window, View1, View2, View3, View4;
GLuint sub_width = 256, sub_height = 256;
/********************** CREATE MULTIPLE VIEWS ********************/

// ============================================================================
//	Functions
// ============================================================================

static int setupCamera(const char *cparam_name, char *vconf, ARParamLT **cparamLT_p, ARHandle **arhandle, AR3DHandle **ar3dhandle)
{
    ARParam			cparam;
    int				xsize, ysize;
    AR_PIXEL_FORMAT pixFormat;
    
    // Open the video path.
    if (arVideoOpen(vconf) < 0) {
        ARLOGe("setupCamera(): Unable to open connection to camera.\n");
        return (FALSE);
    }
    
    // Find the size of the window.
    if (arVideoGetSize(&xsize, &ysize) < 0) {
        ARLOGe("setupCamera(): Unable to determine camera frame size.\n");
        arVideoClose();
        return (FALSE);
    }
    ARLOGi("Camera image size (x,y) = (%d,%d)\n", xsize, ysize);
    
    // Get the format in which the camera is returning pixels.
    pixFormat = arVideoGetPixelFormat();
    if (pixFormat == AR_PIXEL_FORMAT_INVALID) {
        ARLOGe("setupCamera(): Camera is using unsupported pixel format.\n");
        arVideoClose();
        return (FALSE);
    }
    
    // Load the camera parameters, resize for the window and init.
    if (arParamLoad(cparam_name, 1, &cparam) < 0) {
        ARLOGe("setupCamera(): Error loading parameter file %s for camera.\n", cparam_name);
        arVideoClose();
        return (FALSE);
    }
    if (cparam.xsize != xsize || cparam.ysize != ysize) {
        ARLOGw("*** Camera Parameter resized from %d, %d. ***\n", cparam.xsize, cparam.ysize);
        arParamChangeSize(&cparam, xsize, ysize, &cparam);
    }
#ifdef DEBUG
    ARLOG("*** Camera Parameter ***\n");
    arParamDisp(&cparam);
#endif
    if ((*cparamLT_p = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL) {
        ARLOGe("setupCamera(): Error: arParamLTCreate.\n");
        return (FALSE);
    }
    
    if ((*arhandle = arCreateHandle(*cparamLT_p)) == NULL) {
        ARLOGe("setupCamera(): Error: arCreateHandle.\n");
        return (FALSE);
    }
    if (arSetPixelFormat(*arhandle, pixFormat) < 0) {
        ARLOGe("setupCamera(): Error: arSetPixelFormat.\n");
        return (FALSE);
    }
    if (arSetDebugMode(*arhandle, AR_DEBUG_DISABLE) < 0) {
        ARLOGe("setupCamera(): Error: arSetDebugMode.\n");
        return (FALSE);
    }
    if ((*ar3dhandle = ar3DCreateHandle(&cparam)) == NULL) {
        ARLOGe("setupCamera(): Error: ar3DCreateHandle.\n");
        return (FALSE);
    }
    
    if (arVideoCapStart() != 0) {
        ARLOGe("setupCamera(): Unable to begin camera data capture.\n");
        return (FALSE);
    }
    
    return (TRUE);
}

static void cleanup(void)
{
    arglCleanup(gArglSettings);
    gArglSettings = NULL;
    arPattDetach(gARHandle);
    arPattDeleteHandle(gARPattHandle);
    arVideoCapStop();
    ar3DDeleteHandle(&gAR3DHandle);
    arDeleteHandle(gARHandle);
    arParamLTFree(&gCparamLT);
    arVideoClose();
}

static void Keyboard(unsigned char key, int x, int y)
{
    int mode, threshChange = 0;
    AR_LABELING_THRESH_MODE modea;
    
    switch (key) {
        case 0x1B:						// Quit.
        case 'Q':
        case 'q':
            cleanup();
            exit(0);
            break;
        case ' ':
            break;
        case 'X':
        case 'x':
            arGetImageProcMode(gARHandle, &mode);
            switch (mode) {
                case AR_IMAGE_PROC_FRAME_IMAGE:  mode = AR_IMAGE_PROC_FIELD_IMAGE; break;
                case AR_IMAGE_PROC_FIELD_IMAGE:
                default: mode = AR_IMAGE_PROC_FRAME_IMAGE; break;
            }
            arSetImageProcMode(gARHandle, mode);
            break;
        case 'C':
        case 'c':
            ARLOGe("*** Camera - %f (frame/sec)\n", (double)gCallCountMarkerDetect/arUtilTimer());
            gCallCountMarkerDetect = 0;
            arUtilTimerReset();
            break;
        case 'a':
        case 'A':
            arGetLabelingThreshMode(gARHandle, &modea);
            switch (modea) {
                case AR_LABELING_THRESH_MODE_MANUAL:        modea = AR_LABELING_THRESH_MODE_AUTO_MEDIAN; break;
                case AR_LABELING_THRESH_MODE_AUTO_MEDIAN:   modea = AR_LABELING_THRESH_MODE_AUTO_OTSU; break;
                case AR_LABELING_THRESH_MODE_AUTO_OTSU:     modea = AR_LABELING_THRESH_MODE_AUTO_ADAPTIVE; break;
                case AR_LABELING_THRESH_MODE_AUTO_ADAPTIVE: modea = AR_LABELING_THRESH_MODE_AUTO_BRACKETING; break;
                case AR_LABELING_THRESH_MODE_AUTO_BRACKETING:
                default: modea = AR_LABELING_THRESH_MODE_MANUAL; break;
            }
            arSetLabelingThreshMode(gARHandle, modea);
            break;
        case '-':
            threshChange = -5;
            break;
        case '+':
        case '=':
            threshChange = +5;
            break;
        case 'D':
        case 'd':
            arGetDebugMode(gARHandle, &mode);
            arSetDebugMode(gARHandle, !mode);
            break;
        case 's':
            //scale up
            modelScaleFactor += 0.1;
            scaled = false;
            break;
        case 'S':
            //scale down
            //if (!gARTImageSavePlease) gARTImageSavePlease = TRUE;
            modelScaleFactor -= 0.1;
            scaled = false;
            break;
        case '?':
        case '/':
            gShowHelp++;
            if (gShowHelp > 1) gShowHelp = 0;
            break;
        case 'm':
        case 'M':
            gShowMode = !gShowMode;
            break;
        case 'r':
            modelRotateAngle += 5.0;
            rotated = false;
            break;
        case 'R':
            modelRotateAngle -= 5.0;
            rotated = false;
            break;
        case 'n':
            //gDrawRotate = !gDrawRotate;
            gStartAnimation = TRUE;
            gCounter++;
            break;
        case 'p':
            gCounter--;
            break;
        default:
            break;
    }
    if (threshChange) {
        int threshhold;
        arGetLabelingThresh(gARHandle, &threshhold);
        threshhold += threshChange;
        if (threshhold < 0) threshhold = 0;
        if (threshhold > 255) threshhold = 255;
        arSetLabelingThresh(gARHandle, threshhold);
    }
    
}

//
//	This function is called when the
//	GLUT window is resized.
//

static void Reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;

    GLfloat aspect = (GLfloat)w / (GLfloat)h;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    
    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    
    glLoadIdentity();             // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

}

//
// This function is called when the window needs redrawing.
//

static void Main_Display(void) {
    glClearColor(BACKGROUND_R_v2, BACKGROUND_G_v2, BACKGROUND_B_v2, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glutSwapBuffers();
}

static void Welcome_Display(void)
{
    loadObjects();
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "Data/shader/simple.vertexshader", "Data/shader/simple.fragmentshader");
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    
    // Get a handle for our buffers
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");


    if (!welcomed) {
        glClearColor(BACKGROUND_R_v3, BACKGROUND_G_v3, BACKGROUND_B_v3, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(programID);
        
        // Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 Projection = glm::perspective(45.0f, (float)FRAME_WIDTH / FRAME_HEIGHT, 0.1f, 100.0f);
        // Camera matrix
        glm::mat4 View       = glm::lookAt(
                                           glm::vec3(0,0,100), // Camera is at (4,3,3), in World Space
                                           glm::vec3(0,0,0), // and looks at the origin
                                           glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                           );
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 Model;
        // Our ModelViewProjection : multiplication of our 3 matrices
       
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, labelTexture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        //glUniform1i(TextureID, 0);
        
        glPushMatrix();
        
        glRotatef(180.0, 0.0, 0.0, 1.0);
        glScalef(20.0, 10.0, 1.0);
        glGetFloatv(GL_MODELVIEW_MATRIX, &Model[0][0]);
        
        glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
        
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glBindBuffer(GL_ARRAY_BUFFER, label_vertexbuffer);
        glVertexAttribPointer(vertexPosition_modelspaceID,  // The attribute we want to configure
                              3,                            // size
                              GL_FLOAT,                     // type
                              GL_FALSE,                     // normalized?
                              0,                            // stride
                              (void*)0                      // array buffer offset
                              );
        
        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(vertexUVID);
        glBindBuffer(GL_ARRAY_BUFFER, label_uvbuffer);
        glVertexAttribPointer(vertexUVID,                   // The attribute we want to configure
                              2,                            // size : U+V => 2
                              GL_FLOAT,                     // type
                              GL_FALSE,                     // normalized?
                              0,                            // stride
                              (void*)0                      // array buffer offset
                              );
        
        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, label_vertices.size()); // 12*3 indices starting at 0 -> 12 triangles
        
        glDisableVertexAttribArray(vertexPosition_modelspaceID);
        glDisableVertexAttribArray(vertexUVID);
        glPopMatrix();
        
        glutSwapBuffers();

        welcomed = true;
        glutPostRedisplay();
        
        
        //glutKeyboardFunc(Keyboard);

    } else if (windowNotSplit){

        sleep(2);
        
        windowNotSplit = false;
        
        char glutGamemode[32];
        char cparam_name[] = "Data/external_camera_para.dat";
        char markerFilename[] = "Data/marker.txt";
        char vconf[] = "";
        
        //
        // Marker setup.
        //
        
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            fprintf(stdout, "Current working dir: %s\n", cwd);
        
        
        if(!setupMarker(markerFilename))
        {
            ARLOGe("main(): Unable to set up AR markers.\n");
            exit(-1);
        }
        
        //
        // Video setup.
        //
        
        if (!setupCamera(cparam_name, vconf, &gCparamLT, &gARHandle, &gAR3DHandle)) {
            ARLOGe("main(): Unable to set up AR camera.\n");
            exit(-1);
        }
        
        //
        // Graphics setup.
        //
        
        // Set up GL context(s) for OpenGL to draw into.
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        if (!windowed) {
            if (windowRefresh) sprintf(glutGamemode, "%ix%i:%i@%i", windowWidth, windowHeight, windowDepth, windowRefresh);
            else sprintf(glutGamemode, "%ix%i:%i", windowWidth, windowHeight, windowDepth);
            glutGameModeString(glutGamemode);
            glutEnterGameMode();
        } else {
            glutInitWindowSize(windowWidth, windowHeight);
            window = glutCreateWindow("HELLO");
            //glutCreateWindow(argv[0]);
        }
        
        //define the position of the views
        
        View1 = glutCreateSubWindow(window, 0, 0, VIEW1_WIDTH, VIEW1_HEIGHT);
        View2 = glutCreateSubWindow(window, VIEW1_WIDTH+GAP, 0, VIEW2_WIDTH, VIEW2_HEIGHT);
        View3 = glutCreateSubWindow(window, 0, VIEW2_HEIGHT+VIEW4_HEIGHT+GAP, VIEW3_WIDTH, VIEW3_HEIGHT);
        View4 = glutCreateSubWindow(window, VIEW1_WIDTH+GAP, VIEW2_HEIGHT+GAP, VIEW4_WIDTH, VIEW4_HEIGHT);
        
        glutSetWindow(View1);
        
        // Setup ARgsub_lite library for current OpenGL context.
        if ((gArglSettings = arglSetupForCurrentContext(&(gCparamLT->param), arVideoGetPixelFormat())) == NULL) {
            ARLOGe("main(): arglSetupForCurrentContext() returned error.\n");
            cleanup();
            exit(-1);
        }
        arglSetupDebugMode(gArglSettings, gARHandle);
        arUtilTimerReset();
        
        // Load marker(s).
        if ((gARPattHandle = arPattCreateHandle()) == NULL) {
            ARLOGe("setupMarker(): Error: arPattCreateHandle.\n");
            cleanup();
            exit(-1);
        }
        
        int markerIdx;
        for(markerIdx = 0; markerIdx < NUM_OF_MARKER; markerIdx++)
        {
            if(gMarkers[markerIdx].gPatt_name == NULL)
            {
                break;
            }
            if ((gMarkers[markerIdx].gPatt_id = arPattLoad(gARPattHandle, gMarkers[markerIdx].gPatt_name)) < 0) {
                ARLOGe("setupMarker(): Error loading pattern file %s.\n", gMarkers[markerIdx].gPatt_name);
                arPattDeleteHandle(gARPattHandle);
                cleanup();
                exit(-1);
            }
            arPattAttach(gARHandle, gARPattHandle);
        }
        
        
        // Register GLUT event-handling callbacks.
        // NB: mainLoop() is registered by Visibility.
        glutSetWindow(window);
        glutDisplayFunc(Main_Display);
        //glutReshapeFunc(Reshape);
        
        glutSetWindow(View1);
        glutDisplayFunc(View1_Display);
        glutKeyboardFunc(Keyboard);
         
        glutSetWindow(View2);
        //glutReshapeFunc(Reshape);
        glutDisplayFunc(View2_Display);
        glutKeyboardFunc(Keyboard);
         
        glutSetWindow(View3);
        //glutReshapeFunc(Reshape);
        glutDisplayFunc(View3_Display);
        //glutKeyboardFunc(Keyboard);
        
        glutSetWindow(View4);
        glutDisplayFunc(View4_Display);
        
        glutMainLoop();
        
        
        
    } else {
        glClearColor(BACKGROUND_R_v3, BACKGROUND_G_v3, BACKGROUND_B_v3, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glutSwapBuffers();
    }


}

int main(int argc, char** argv)
{
    
    char glutGamemode[32];
    //
    // Library inits.
    //
    
    glutInit(&argc, argv);
    
    // Set up GL context(s) for OpenGL to draw into.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    if (!windowed) {
        if (windowRefresh) sprintf(glutGamemode, "%ix%i:%i@%i", windowWidth, windowHeight, windowDepth, windowRefresh);
        else sprintf(glutGamemode, "%ix%i:%i", windowWidth, windowHeight, windowDepth);
        glutGameModeString(glutGamemode);
        glutEnterGameMode();
    } else {
        glutInitWindowSize(windowWidth, windowHeight);
        //window = glutCreateWindow(argv[0]);
        glutCreateWindow(argv[0]);
    }
 
    glutDisplayFunc(Welcome_Display);
    glutMainLoop();
    
    return (0);
}
