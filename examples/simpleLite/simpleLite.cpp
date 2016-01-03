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
using namespace glm;
// ============================================================================
//	Constants
// ============================================================================

#define VIEW_SCALEFACTOR		1.0         // Units received from ARToolKit tracking will be multiplied by this factor before being used in OpenGL drawing.
#define VIEW_DISTANCE_MIN		40.0        // Objects closer to the camera than this will not be displayed. OpenGL units.
#define VIEW_DISTANCE_MAX		10000.0     // Objects further away from the camera than this will not be displayed. OpenGL units.
#define MAX_MARKER_NUM          60
#define NUM_OF_MARKER           6
#define NUM_OF_NUMBER           10
// ============================================================================
//	Global variables
// ============================================================================
// Marker array
static ARMarker gMarkers[MAX_MARKER_NUM];

// Preferences.
static int windowed = TRUE;                     // Use windowed (TRUE) or fullscreen mode (FALSE) on launch.
static int windowWidth = 1024;					// Initial window width, also updated during program execution.
static int windowHeight = 768;                  // Initial window height, also updated during program execution.
static int windowDepth = 32;					// Fullscreen mode bit depth.
static int windowRefresh = 0;					// Fullscreen mode refresh rate. Set to 0 to use default rate.

// Image acquisition.
static ARUint8		*gARTImage = NULL;
static int          gARTImageSavePlease = FALSE;

// Marker detection.
static ARHandle		*gARHandle = NULL;
static ARPattHandle	*gARPattHandle = NULL;
static long			gCallCountMarkerDetect = 0;

// Transformation matrix retrieval.
static AR3DHandle	*gAR3DHandle = NULL;

// Drawing.
static ARParamLT *gCparamLT = NULL;
static ARGL_CONTEXT_SETTINGS_REF gArglSettings = NULL;
static int gShowHelp = 1;
static int gShowMode = 1;
static int gDrawRotate = TRUE;
static int gStartAnimation = FALSE;
static int gDrawTableFace = FALSE;
static int gDrawTableLeg1 = FALSE;
static int gDrawTableLeg2 = FALSE;
static int gDrawTableLeg3 = FALSE;
static int gDrawTableLeg4 = FALSE;
static int gDrawTranslateLeg1 = TRUE;
static int gDrawTranslateLeg2 = FALSE;
static int gDrawTranslateLeg3 = FALSE;
static int gDrawTranslateLeg4 = FALSE;
static float gDrawRotateAngle = 0;			// For use in drawing.
static float gDrawRotateTableAngle = 0;     // For flipping table face.
static float gDrawTranslateDistanceLeg1 = 0;
static float gDrawTranslateDistanceLeg2 = 0;
static float gDrawTranslateDistanceLeg3 = 0;
static float gDrawTranslateDistanceLeg4 = 0;
int gCounter = 0;
double modelScaleFactor = 1.0;
double modelRotateAngle = 0.0;
bool rotated = false;
bool scaled = false;

// ============================================================================
//	Function prototypes.
// ============================================================================

static void print(const char *text, const float x, const float y, int calculateXFromRightEdge, int calculateYFromTopEdge);
static void drawBackground(const float width, const float height, const float x, const float y);
static void printHelpKeys();
static void printMode();
static void loadText();
static void DrawText(tablePiece piece);
static void View1_Display(void);
static void View2_Display(void);
/********************** DRAW TEXT **********************/
bool objloaded = false;

// Read our .obj file

std::vector<glm::vec3> vertices[NUM_OF_NUMBER];
std::vector<glm::vec2> uvs[NUM_OF_NUMBER];
std::vector<glm::vec3> normals[NUM_OF_NUMBER]; // Won't be used at the moment.
GLuint vertexbuffer[NUM_OF_NUMBER];
GLuint uvbuffer[NUM_OF_NUMBER];
GLuint whiteTexture;
GLuint redTexture;
GLuint greenTexture;
/********************** DRAW TEXT **********************/

/********************** CREATE MULTIPLE VIEWS ********************/
GLuint window, View1, View2, View3, View4;
GLuint sub_width = 256, sub_height = 256;
/********************** CREATE MULTIPLE VIEWS ********************/

// ============================================================================
//	Functions
// ============================================================================
static bool foundPattern()
{
    int pattIdx;
    for(pattIdx = 0; pattIdx < NUM_OF_MARKER; pattIdx++)
    {
        if(gMarkers[pattIdx].gPatt_found)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

// Something to look at, draw a rotating colour cube.
static void DrawModel(void)
{
    // Colour cube data.
    int i;
    /********************************* SCALE DATA **********************************/
    const float TABLE_FACE_SCALE_X = 70.0f;
    const float TABLE_FACE_SCALE_Y = 70.0f;
    const float TABLE_FACE_SCALE_Z = 10.0f;
    const float TABLE_LEG_SCALE_X = 10.0f;
    const float TABLE_LEG_SCALE_Y = 10.0f;
    const float TABLE_LEG_SCALE_Z = 70.0f;
    /********************************* CUBE DATA **********************************/
    const GLfloat cube_vertices [8][3] = {
        /* +z */ {0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},
        /* -z */ {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f} };
    const GLubyte cube_vertex_colors_right[8][4] = {
        {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100},
        {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100} };
    const GLubyte cube_vertex_colors_wrong [8][4] = {
        {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100},
        {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100} };
    const GLubyte cube_vertex_colors_maybe [8][4] = {
        {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100},
        {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100} };

    const GLubyte cube_vertex_colors [8][4] = {
        {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100},
        {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100} };
    const GLubyte cube_faces [6][4] = { /* ccw-winding */
        /* +z */ {3, 2, 1, 0}, /* -y */ {2, 3, 7, 6}, /* +y */ {0, 1, 5, 4},
        /* -x */ {3, 0, 4, 7}, /* +x */ {1, 2, 6, 5}, /* -z */ {4, 5, 6, 7} };
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // TODO: have different markers to recognize the front and the back of the table face
    // if the table face is facing the wrong way, show animation to flip the table
    /********************************* TABLE FACE **********************************/
    glScalef(modelScaleFactor, modelScaleFactor, modelScaleFactor);
    glRotatef(modelRotateAngle, 0.0f, 1.0f, 0.0f);
    
    //if(gStartAnimation && gCounter >= 1)
    //found the marker for the first piece
    if(gMarkers[0].gPatt_found || gMarkers[1].gPatt_found)
    {
        gDrawTableFace = TRUE;
    }
    
    if(1)
    {
        if(gCounter >= 6)
        {
            glTranslatef(0.0f, 50.0f, 0.0f);
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        }
       
        //glRotatef(gDrawRotateAngle, 1.0f, 0.0f, 0.0f);
        glPushMatrix(); // Save world coordinate system.
        if(gMarkers[1].gPatt_found)
        {
            //glTranslatef(0.0f, 50.0f, 0.0f);
            glRotatef(gDrawRotateTableAngle, 1.0f, 0.0f, 0.0f);
        }
        else
        {
            gDrawRotateTableAngle = 0.0f;
        }
        glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
        glScalef(TABLE_FACE_SCALE_X, TABLE_FACE_SCALE_Y, TABLE_FACE_SCALE_Z);
        //glTranslatef(0.0f, 0.0f, 0.5f); // Place base of cube on marker surface.
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //glDisable(GL_BLEND);
        
        if(foundPattern())
        {
            if(gMarkers[0].gPatt_found)
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
            }
            else if(gMarkers[1].gPatt_found)
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_maybe);
            }
            else
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
            }
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
        }
        glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        glDisableClientState(GL_COLOR_ARRAY);
        glColor4ub(0, 0, 0, 255);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        
        //DrawText(FACE);
        glPopMatrix();    // Restore world coordinate system.
    }
    
    /********************************* TABLE LEG1 **********************************/
    if(gMarkers[2].gPatt_found)
    {
        gDrawTableLeg1 = TRUE;
    }
    if(gDrawTableLeg1)
    {
        glPushMatrix(); // Save world coordinate system.
        glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
        glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
        glTranslatef(-2.5f, -2.5f, 1.5f);
        
        if(gStartAnimation)
        {
            glRotatef(gDrawRotateAngle, 0.0f, 0.0f, 1.0f); // Rotate about z axis.
        }
        glTranslatef(0.0f, 0.0f, -gDrawTranslateDistanceLeg1);
        
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //glDisable(GL_BLEND);
        
        if(foundPattern())
        {
            if(gMarkers[2].gPatt_found)
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
            }
            else
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
            }
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
        }

        glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        glDisableClientState(GL_COLOR_ARRAY);
        glColor4ub(0, 0, 0, 255);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        //DrawText(LEG1);

        glPopMatrix();    // Restore world coordinate system.

    }
    
    /********************************* TABLE LEG2 **********************************/
    if(gMarkers[3].gPatt_found)
    {
        gDrawTableLeg2 = TRUE;
    }
    if(gDrawTableLeg2)
    {
        glPushMatrix(); // Save world coordinate system.
        glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
        glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
        glTranslatef(2.5f, 2.5f, 1.5f);
        
        if(gDrawTranslateLeg2)
        {
            glRotatef(gDrawRotateAngle, 0.0f, 0.0f, 1.0f); // Rotate about z axis.
        }
        
        if(!gDrawTranslateLeg1)
        {
            glTranslatef(0.0f, 0.0f, -gDrawTranslateDistanceLeg2);
        }
        
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //glDisable(GL_BLEND);
        
        if(foundPattern())
        {
            if(gMarkers[3].gPatt_found)
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
            }
            else
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
            }
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
        }

        glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        glDisableClientState(GL_COLOR_ARRAY);
        glColor4ub(0, 0, 0, 255);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        //DrawText(LEG2);
        glPopMatrix();    // Restore world coordinate system.
    }
    
    /********************************* TABLE LEG3 **********************************/
    if(gMarkers[4].gPatt_found)
    {
        gDrawTableLeg3 = TRUE;
    }
    if(gDrawTableLeg3)
    {
        glPushMatrix(); // Save world coordinate system.
        glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
        glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
        glTranslatef(-2.5f, 2.5f, 1.5f);
        
        if(gDrawTranslateLeg3)
        {
            glRotatef(gDrawRotateAngle, 0.0f, 0.0f, 1.0f); // Rotate about z axis.
        }
        
        if(!gDrawTranslateLeg2)
        {
            glTranslatef(0.0f, 0.0f, -gDrawTranslateDistanceLeg3);
        }
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //glDisable(GL_BLEND);
        
        if(foundPattern())
        {
            if(gMarkers[4].gPatt_found)
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
            }
            else
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
            }
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
        }

        glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        glDisableClientState(GL_COLOR_ARRAY);
        glColor4ub(0, 0, 0, 255);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        //DrawText(LEG3);
        glPopMatrix();    // Restore world coordinate system.
    }
    
    /********************************* TABLE LEG4 **********************************/
    if(gMarkers[5].gPatt_found)
    {
        gDrawTableLeg4 = TRUE;
    }
    if(gDrawTableLeg4)
    {
        glPushMatrix(); // Save world coordinate system.
        glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
        glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
        glTranslatef(2.5f, -2.5f, 1.5f);
        
        if(gDrawTranslateLeg4)
        {
            glRotatef(gDrawRotateAngle, 0.0f, 0.0f, 1.0f); // Rotate about z axis.
        }
        
        if(!gDrawTranslateLeg3)
        {
            glTranslatef(0.0f, 0.0f, -gDrawTranslateDistanceLeg4);
        }
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //glDisable(GL_BLEND);
        
        if(foundPattern())
        {
            if(gMarkers[5].gPatt_found)
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
            }
            else
            {
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
            }
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
        }
        
        glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        glDisableClientState(GL_COLOR_ARRAY);
        glColor4ub(0, 0, 0, 255);
        for (i = 0; i < 6; i++) {
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
        }
        //DrawText(LEG4);
        glPopMatrix();    // Restore world coordinate system.
        
    }
}

static void DrawText(tablePiece piece)
{
    int textIdx;
    
    switch (piece) {
        case FACE:
            textIdx = 0;
            break;
        case FACE_BACK:
            textIdx = 0;
            break;
        case LEG1:
            textIdx = 1;
            break;
        case LEG2:
            textIdx = 2;
            break;
        case LEG3:
            textIdx = 3;
            break;
        case LEG4:
            textIdx = 4;
            break;
        default:
            break;
    }
    
    /*glPushMatrix(); // Save world coordinate system.
    //glRotatef(gDrawRotateAngle, 0.0f, 0.0f, 1.0f); // Rotate about z axis.
    glRotatef(-90.0, 0.0, 1.0, 0.0);
    glScalef(60, 60, 60);
    glTranslatef(0.0f, 0.0f, 0.5f); // Place base of cube on marker surface.*/
    //glDisable(GL_LIGHTING);
    //glDisable(GL_BLEND);
    
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, redTexture);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[textIdx]);
    //glEnableVertexAttribArray(vertexbuffer[textIdx]);    //We like submitting vertices on stream 0 for no special reason
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(glm::vec3), 0);   //The starting point of the VBO, for the vertices
    
    /*glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
     glClientActiveTexture(GL_TEXTURE0);
     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
     glVertexPointer(2, GL_FLOAT, sizeof(glm::vec2), 0);*/
    
    glDrawArrays(GL_TRIANGLES, 0, vertices[textIdx].size() * 3);
    
    //glDisable(GL_TEXTURE_2D);
    
    //glDisableVertexAttribArray(vertexbuffer[textIdx]);

    //glPopMatrix();
}

static void DrawModelUpdate(float timeDelta)
{
    float speed = 0.2f;
    if (gDrawRotate) {
        gDrawRotateAngle += timeDelta * 45.0f; // Rotate cube at 45 degrees per second.
        if (gDrawRotateAngle > 360.0f) gDrawRotateAngle -= 360.0f;
    }
    
    if(gDrawTableFace)
    {
        gDrawRotateTableAngle += timeDelta * 20.0f;
        if(gDrawRotateTableAngle > 180.0f)
        {
            gDrawRotateTableAngle = 180.0f;
        }
    }
    
    if(gCounter >= 1){
        if(gDrawTranslateDistanceLeg1 > 1.0f)
        {
            speed = 0.0;
            gDrawTranslateLeg1 = FALSE;
            gDrawTranslateLeg2 = TRUE;
            gStartAnimation = FALSE;
        }
        gDrawTranslateDistanceLeg1 += timeDelta * speed; // Translate cube at 0.1 pixels per second.
    }
    
    if(gCounter >= 2){
        speed = 0.2f;
        if(gDrawTranslateDistanceLeg2 > 1.0f)
        {
            speed = 0.0;
            gDrawTranslateLeg2 = FALSE;
            gDrawTranslateLeg3 = TRUE;
        }
        gDrawTranslateDistanceLeg2 += timeDelta * speed; // Translate cube at 0.1 pixels per second.
    }
    
    if(gCounter >= 3){
        speed = 0.2f;
        if(gDrawTranslateDistanceLeg3 > 1.0f)
        {
            speed = 0.0;
            gDrawTranslateLeg3 = FALSE;
            gDrawTranslateLeg4 = TRUE;
        }
        gDrawTranslateDistanceLeg3 += timeDelta * speed; // Translate cube at 0.1 pixels per second.
    }
    
    if(gCounter >= 4){
        speed = 0.2f;
        if(gDrawTranslateDistanceLeg4 > 1.0f)
        {
            speed = 0.0;
            gDrawTranslateLeg4 = FALSE;
        }
        gDrawTranslateDistanceLeg4 += timeDelta * speed; // Translate cube at 0.1 pixels per second.
    }
}

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
            //gDrawRotate = !gDrawRotate;
            gStartAnimation = TRUE;
            gCounter++;
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

static void mainLoop(void)
{
    static int imageNumber = 0;
    static int ms_prev;
    int ms;
    float s_elapsed;
    ARUint8 *image;
    ARdouble err;
    
    int j, k;
    
    // Find out how long since mainLoop() last ran.
    ms = glutGet(GLUT_ELAPSED_TIME);
    s_elapsed = (float)(ms - ms_prev) * 0.001f;
    if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
    ms_prev = ms;
    
    // Update drawing.
    DrawModelUpdate(s_elapsed);
    
    // Grab a video frame.
    if ((image = arVideoGetImage()) != NULL) {
        gARTImage = image;	// Save the fetched image.
        
        if (gARTImageSavePlease) {
            char imageNumberText[15];
            sprintf(imageNumberText, "image-%04d.jpg", imageNumber++);
            if (arVideoSaveImageJPEG(gARHandle->xsize, gARHandle->ysize, gARHandle->arPixelFormat, gARTImage, imageNumberText, 75, 0) < 0) {
                ARLOGe("Error saving video image.\n");
            }
            gARTImageSavePlease = FALSE;
        }
        
        gCallCountMarkerDetect++; // Increment ARToolKit FPS counter.
        
        // Detect the markers in the video frame.
        if (arDetectMarker(gARHandle, gARTImage) < 0) {
            exit(-1);
        }
        
        // Check through the marker_info array for highest confidence
        // visible marker matching our preferred pattern.
        int i;
        for (i = 0; i < NUM_OF_MARKER; i++) {
            
            // Check through the marker_info array for highest confidence
            // visible marker matching our object's pattern.
            k = -1;
            for (j = 0; j < gARHandle->marker_num; j++) {
                if (gARHandle->markerInfo[j].id == gMarkers[i].gPatt_id) {
                    if (k == -1) k = j; // First marker detected.
                    else if (gARHandle->markerInfo[j].cf > gARHandle->markerInfo[k].cf) k = j; // Higher confidence marker detected.
                }
            }
            
            if (k != -1) {
                err = arGetTransMatSquare(gAR3DHandle, &(gARHandle->markerInfo[k]), gMarkers[i].gPatt_width, gMarkers[i].gPatt_trans);
                gMarkers[i].gPatt_found = TRUE;
            } else {
                gMarkers[i].gPatt_found = FALSE;
            }
        }
        
        // Tell GLUT the display has changed.
        // The next iteration through glutMainLoop, the window's display callback will be called to redisplay the window's normal plane.
        glutPostRedisplay();
    }
}

//
//	This function is called on events when the visibility of the
//	GLUT window changes (including when it first becomes visible).
//
static void Visibility(int visible)
{
    if (visible == GLUT_VISIBLE) {
        glutIdleFunc(mainLoop);
        
    } else {
        glutIdleFunc(NULL);
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
static void Main_Display(void)
{
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glutSwapBuffers();
}

static void View1_Display(void)
{
    ARdouble p[16];
    ARdouble m[16];
    
    glutPostRedisplay();
    mainLoop();
    // Select correct buffer for this context.
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers for new frame.
    
    arglPixelBufferDataUpload(gArglSettings, gARTImage);
    arglDispImage(gArglSettings);
    gARTImage = NULL; // Invalidate image data.
    
    // Projection transformation.
    arglCameraFrustumRH(&(gCparamLT->param), VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p);
    glMatrixMode(GL_PROJECTION);
#ifdef ARDOUBLE_IS_FLOAT
    glLoadMatrixf(p);
#else
    glLoadMatrixd(p);
#endif
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    
    // Viewing transformation.
    glLoadIdentity();
    // Lighting and geometry that moves with the camera should go here.
    // (I.e. must be specified before viewing transformations.)
    //none
    
    /*if (gPatt_found_1) {*/
    if(TRUE) {
    
        m[0] = 1.0;
        m[1] = 0.0;
        m[2] = 0.0;
        m[3] = 0.0;
        m[4] = 0.0;
        m[5] = 1.0;
        m[6] = 0.0;
        m[7] = 0.0;
        m[8] = 0.0;
        m[9] = 0.0;
        m[10] = 1.0;
        m[11] = 0.0;
        m[12] = 0.0;
        m[13] = -50.0;
        m[14] = -200.0;
        m[15] = 1.0;
        
        //arglCameraViewRH(gPatt_trans_1, m, VIEW_SCALEFACTOR);
        
#ifdef ARDOUBLE_IS_FLOAT
        glLoadMatrixf(m);
#else
        glLoadMatrixd(m);
#endif
        
        // All lighting and geometry to be drawn relative to the marker goes here.
        //DrawModel();
    } // gPatt_foundq
    
    
    int markerIdx;
    for(markerIdx = 0; markerIdx < NUM_OF_MARKER; markerIdx++)
    {
        if(gMarkers[markerIdx].gPatt_found)
        {
            arglCameraViewRH(gMarkers[markerIdx].gPatt_trans, m, VIEW_SCALEFACTOR);
#ifdef ARDOUBLE_IS_FLOAT
            glLoadMatrixf(m);
#else
            glLoadMatrixd(m);
#endif
            
            // All lighting and geometry to be drawn relative to the marker goes here.
            glPushMatrix();
            glRotatef(-90.0, 0.0, 1.0, 0.0);
            glScalef(60, 60, 60);
            glTranslatef(0.0f, 0.0f, 0.5f);
            DrawText(gMarkers[markerIdx].piece);
            glPopMatrix();
        }
    }
    
    
    // Any 2D overlays go here.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, (GLdouble)windowWidth, 0, (GLdouble)windowHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    //
    // Draw help text and mode.
    //
    if (gShowMode) {
        printMode();
    }
    /*if (gShowHelp) {
        if (gShowHelp == 1) {
            printHelpKeys();
        }
    }*/
    
    glutSwapBuffers();
}


void View2_Display(void) {
    int i;
    
    glutPostRedisplay();
    /********************************* SCALE DATA **********************************/
    const float TABLE_FACE_SCALE_X = 1.0f;//70.0f;
    const float TABLE_FACE_SCALE_Y = 1.0f;//70.0f;
    const float TABLE_FACE_SCALE_Z = 0.15f; //10.0f;
    const float TABLE_LEG_SCALE_X = 0.15f;
    const float TABLE_LEG_SCALE_Y = 0.15f;
    const float TABLE_LEG_SCALE_Z = 1.0f;
    /********************************* CUBE DATA **********************************/
    const GLfloat cube_vertices [8][3] = {
        /* +z */ {0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},
        /* -z */ {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f} };
    const GLubyte cube_vertex_colors_right[8][4] = {
        {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100},
        {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100} };
    const GLubyte cube_vertex_colors_wrong [8][4] = {
        {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100},
        {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100} };
    const GLubyte cube_vertex_colors_maybe [8][4] = {
        {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100},
        {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100} };
    
    const GLubyte cube_vertex_colors [8][4] = {
        {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100},
        {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100} };
    const GLubyte cube_faces [6][4] = { /* ccw-winding */
        /* +z */ {3, 2, 1, 0}, /* -y */ {2, 3, 7, 6}, /* +y */ {0, 1, 5, 4},
        /* -x */ {3, 0, 4, 7}, /* +x */ {1, 2, 6, 5}, /* -z */ {4, 5, 6, 7} };
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    
    // Render a color-cube consisting of 6 quads with different colors
    
    if(!scaled)
    {
        glScalef(modelScaleFactor, modelScaleFactor, modelScaleFactor);
        scaled = true;
    }
    glTranslatef(0.0f, -0.5f, -2.0f);
    if(!rotated)
    {
        glRotatef(modelRotateAngle, 0.0f, 1.0f, 0.0f);
        rotated = true;
    }
    glTranslatef(0.0f, 0.5f, 2.0f);
    
    glPushMatrix();
    //rotate the table face is marker 1 is found, i.e. the orientation is wrong
    if(gMarkers[1].gPatt_found)
    {
        glRotatef(gDrawRotateTableAngle, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        gDrawRotateTableAngle = 0.0f;
    }
    if(foundPattern())
    {
        if(gMarkers[0].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else if(gMarkers[1].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_maybe);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    // Reset the model-view matrix
    glTranslatef(0.0f, -0.5f, -2.0f);  // Move right and into the screen
    glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_FACE_SCALE_X, TABLE_FACE_SCALE_Y, TABLE_FACE_SCALE_Z);
    
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    
    /********************************* TABLE LEG1 **********************************/
    glPushMatrix();
    glTranslatef(-0.4f, 0.0f, -2.4f);
    glRotatef(90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
    
    if(foundPattern())
    {
        if(gMarkers[2].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    
    /********************************* TABLE LEG2 **********************************/
    glPushMatrix();
    glTranslatef(0.4f, 0.0f, -2.4f);
    glRotatef(90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
    
    if(foundPattern())
    {
        if(gMarkers[3].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    
    /********************************* TABLE LEG3 **********************************/
    glPushMatrix();
    glTranslatef(0.4f, 0.0f, -1.6f);
    glRotatef(90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
    
    if(foundPattern())
    {
        if(gMarkers[4].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    /********************************* TABLE LEG4 **********************************/
    glPushMatrix();
    glTranslatef(-0.4f, 0.0f, -1.6f);
    glRotatef(90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
    
    if(foundPattern())
    {
        if(gMarkers[5].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    
    glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}

void View3_Display(void) {
    int i;
    glutPostRedisplay();
    /********************************* SCALE DATA **********************************/
    const float TABLE_FACE_SCALE_X = 1.0f;//70.0f;
    const float TABLE_FACE_SCALE_Y = 1.0f;//70.0f;
    const float TABLE_FACE_SCALE_Z = 0.15f; //10.0f;
    const float TABLE_LEG_SCALE_X = 0.15f;
    const float TABLE_LEG_SCALE_Y = 0.15f;
    const float TABLE_LEG_SCALE_Z = 1.0f;
    /********************************* CUBE DATA **********************************/
    const GLfloat cube_vertices [8][3] = {
        /* +z */ {0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},
        /* -z */ {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f} };
    const GLubyte cube_vertex_colors_right[8][4] = {
        {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100},
        {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100}, {0, 255, 0, 100} };
    const GLubyte cube_vertex_colors_wrong [8][4] = {
        {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100},
        {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100}, {255, 0, 0, 100} };
    const GLubyte cube_vertex_colors_maybe [8][4] = {
        {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100},
        {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100}, {255, 255, 0, 100} };
    
    const GLubyte cube_vertex_colors [8][4] = {
        {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100},
        {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100}, {255, 255, 255, 100} };
    const GLubyte cube_faces [6][4] = { /* ccw-winding */
        /* +z */ {3, 2, 1, 0}, /* -y */ {2, 3, 7, 6}, /* +y */ {0, 1, 5, 4},
        /* -x */ {3, 0, 4, 7}, /* +x */ {1, 2, 6, 5}, /* -z */ {4, 5, 6, 7} };
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    
    // Render a color-cube consisting of 6 quads with different colors
    
    glPushMatrix();
    //rotate the table face is marker 1 is found, i.e. the orientation is wrong
    if(gMarkers[1].gPatt_found)
    {
        glRotatef(gDrawRotateTableAngle, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        gDrawRotateTableAngle = 0.0f;
    }
    if(foundPattern())
    {
        if(gMarkers[0].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else if(gMarkers[1].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_maybe);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    // Reset the model-view matrix
    glTranslatef(-1.5f, 0.0f, -2.0f);  // Move right and into the screen
    //glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_FACE_SCALE_X, TABLE_FACE_SCALE_Y, TABLE_FACE_SCALE_Z);
    
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    
    /********************************* TABLE LEG1 **********************************/
    glPushMatrix();
    glTranslatef(-0.5f, 0.0f, -2.0f);
    glRotatef(90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
    
    if(foundPattern())
    {
        if(gMarkers[2].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    
    /********************************* TABLE LEG2 **********************************/
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
    
    if(foundPattern())
    {
        if(gMarkers[3].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    
    /********************************* TABLE LEG3 **********************************/
    glPushMatrix();
    glTranslatef(0.5f, 0.0f, -2.0f);
    glRotatef(90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
    
    if(foundPattern())
    {
        if(gMarkers[4].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    /********************************* TABLE LEG4 **********************************/
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, -2.0f);
    glRotatef(90.0, 1.0f, 0.0f, 0.0f);
    glScalef(TABLE_LEG_SCALE_X, TABLE_LEG_SCALE_Y, TABLE_LEG_SCALE_Z);
    
    if(foundPattern())
    {
        if(gMarkers[5].gPatt_found)
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_right);
        }
        else
        {
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors_wrong);
        }
    }
    else
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    }
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
    }
    glPopMatrix();
    
    glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}


void setupMarker()
{
    gMarkers[0].gPatt_name = "Data/patt.0";
    gMarkers[0].gPatt_width = 80.0;
    gMarkers[0].piece = FACE;
    gMarkers[1].gPatt_name = "Data/patt.0b";
    gMarkers[1].gPatt_width = 80.0;
    gMarkers[1].piece = FACE_BACK;
    gMarkers[2].gPatt_name = "Data/patt.1";
    gMarkers[2].gPatt_width = 80.0;
    gMarkers[2].piece = LEG1;
    gMarkers[3].gPatt_name = "Data/patt.2";
    gMarkers[3].gPatt_width = 80.0;
    gMarkers[3].piece = LEG2;
    gMarkers[4].gPatt_name = "Data/patt.3";
    gMarkers[4].gPatt_width = 80.0;
    gMarkers[4].piece = LEG3;
    gMarkers[5].gPatt_name = "Data/patt.4";
    gMarkers[5].gPatt_width = 80.0;
    gMarkers[5].piece = LEG4;
}

int main(int argc, char** argv)
{
    char glutGamemode[32];
    char cparam_name[] = "Data/external_camera_para.dat";
    char vconf[] = "";
    
    // TODO: parse a file and read all marker info
    setupMarker();
    
    //
    // Library inits.
    //
    
    glutInit(&argc, argv);
    
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
        window = glutCreateWindow(argv[0]);
    }
    
    View1 = glutCreateSubWindow(window, 0, 0, 615, 460);
    View2 = glutCreateSubWindow(window, 615, 0, 409, 460);
    View3 = glutCreateSubWindow(window, 0, 460, 1024, 308);
    
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
        return (FALSE);
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
            return (FALSE);
        }
        arPattAttach(gARHandle, gARPattHandle);
    }
    
    whiteTexture = loadBMP_custom("Data/mesh/white.bmp");
    redTexture = loadBMP_custom("Data/mesh/red.bmp");
    greenTexture = loadBMP_custom("Data/mesh/green.bmp");
    
    loadText();
    
    // Register GLUT event-handling callbacks.
    // NB: mainLoop() is registered by Visibility.
    glutSetWindow(window);
    glutDisplayFunc(Main_Display);
    glutKeyboardFunc(Keyboard);

    
    //TODO: create multiple views here
    glutSetWindow(View1);
    glutDisplayFunc(View1_Display);
    glutKeyboardFunc(Keyboard);

    
    glutSetWindow(View2);
    glutReshapeFunc(Reshape);
    glutDisplayFunc(View2_Display);
    glutKeyboardFunc(Keyboard);
    
    glutSetWindow(View3);
    glutDisplayFunc(View3_Display);
    
   
    glutReshapeFunc(Reshape);
    //glutVisibilityFunc(Visibility);
    glutKeyboardFunc(Keyboard);
    
    glutMainLoop();
    
    return (0);
}


static void loadText()
{
    int textIdx;
    for(textIdx = 0; textIdx < NUM_OF_NUMBER; textIdx++)
    {
        char number[1];
        sprintf(number, "%d", textIdx);
        
        char file_extension[] = ".obj";
        char file_dir[] = "Data/mesh/";
        
        char *file_name = strcat(number, file_extension);
        char *obj_file = strcat(file_dir, file_name);
        
        // printf("obj file name is: %s\n", obj_file);
        
        objloaded  = loadOBJ(obj_file, vertices[textIdx], uvs[textIdx], normals[textIdx]);
        
        if(!objloaded)
        {
            printf("%d is not loaded!!", textIdx);
        }
        
        glGenBuffers(1, &vertexbuffer[textIdx]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[textIdx]);
        glBufferData(GL_ARRAY_BUFFER, vertices[textIdx].size() * sizeof(glm::vec3), &vertices[textIdx][0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &uvbuffer[textIdx]);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[textIdx]);
        glBufferData(GL_ARRAY_BUFFER, uvs[textIdx].size() * sizeof(glm::vec2), &uvs[textIdx][0], GL_STATIC_DRAW);
    }
}

//
// The following functions provide the onscreen help text and mode info.
//

static void print(const char *text, const float x, const float y, int calculateXFromRightEdge, int calculateYFromTopEdge)
{
    int i, len;
    GLfloat x0, y0;
    
    if (!text) return;
    
    if (calculateXFromRightEdge) {
        x0 = windowWidth - x - (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)text);
    } else {
        x0 = x;
    }
    if (calculateYFromTopEdge) {
        y0 = windowHeight - y - 10.0f;
    } else {
        y0 = y;
    }
    glRasterPos2f(x0, y0);
    
    len = (int)strlen(text);
    for (i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, text[i]);
}

static void drawBackground(const float width, const float height, const float x, const float y)
{
    GLfloat vertices[4][2];
    
    vertices[0][0] = x; vertices[0][1] = y;
    vertices[1][0] = width + x; vertices[1][1] = y;
    vertices[2][0] = width + x; vertices[2][1] = height + y;
    vertices[3][0] = x; vertices[3][1] = height + y;
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);	// 50% transparent black.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Opaque white.
    //glLineWidth(1.0f);
    //glDrawArrays(GL_LINE_LOOP, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_BLEND);
}

static void printHelpKeys()
{
    int i;
    GLfloat  w, bw, bh;
    const char *helpText[] = {
        "Keys:\n",
        " ? or /        Show/hide this help.",
        " q or [esc]    Quit program.",
        " d             Activate / deactivate debug mode.",
        " m             Toggle display of mode info.",
        " a             Toggle between available threshold modes.",
        " - and +       Switch to manual threshold mode, and adjust threshhold up/down by 5.",
        " x             Change image processing mode.",
        " c             Calulcate frame rate.",
        " space         Start animation.",
        " r             Rotate the model CCW.",
        " R             Rotate the model CW.",
        " s             Scale up the model.",
        " S             Scale down the model."
    };
#define helpTextLineCount (sizeof(helpText)/sizeof(char *))
    
    bw = 0.0f;
    for (i = 0; i < helpTextLineCount; i++) {
        w = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (unsigned char *)helpText[i]);
        if (w > bw) bw = w;
    }
    bh = helpTextLineCount * 10.0f /* character height */+ (helpTextLineCount - 1) * 2.0f /* line spacing */;
    drawBackground(bw, bh, 2.0f, 2.0f);
    
    for (i = 0; i < helpTextLineCount; i++) print(helpText[i], 2.0f, (helpTextLineCount - 1 - i)*12.0f + 2.0f, 0, 0);;
}

static void printMode()
{
    int len, thresh, line, mode, xsize, ysize;
    AR_LABELING_THRESH_MODE threshMode;
    ARdouble tempF;
    char text[256], *text_p;
    
    glColor3ub(255, 255, 255);
    line = 1;
    
    // Image size and processing mode.
    arVideoGetSize(&xsize, &ysize);
    arGetImageProcMode(gARHandle, &mode);
    if (mode == AR_IMAGE_PROC_FRAME_IMAGE) text_p = "full frame";
    else text_p = "even field only";
    snprintf(text, sizeof(text), "Processing %dx%d video frames %s", xsize, ysize, text_p);
    print(text, 2.0f,  (line - 1)*12.0f + 2.0f, 0, 1);
    line++;
    
    // Threshold mode, and threshold, if applicable.
    arGetLabelingThreshMode(gARHandle, &threshMode);
    switch (threshMode) {
        case AR_LABELING_THRESH_MODE_MANUAL: text_p = "MANUAL"; break;
        case AR_LABELING_THRESH_MODE_AUTO_MEDIAN: text_p = "AUTO_MEDIAN"; break;
        case AR_LABELING_THRESH_MODE_AUTO_OTSU: text_p = "AUTO_OTSU"; break;
        case AR_LABELING_THRESH_MODE_AUTO_ADAPTIVE: text_p = "AUTO_ADAPTIVE"; break;
        case AR_LABELING_THRESH_MODE_AUTO_BRACKETING: text_p = "AUTO_BRACKETING"; break;
        default: text_p = "UNKNOWN"; break;
    }
    snprintf(text, sizeof(text), "Threshold mode: %s", text_p);
    if (threshMode != AR_LABELING_THRESH_MODE_AUTO_ADAPTIVE) {
        arGetLabelingThresh(gARHandle, &thresh);
        len = (int)strlen(text);
        snprintf(text + len, sizeof(text) - len, ", thresh=%d", thresh);
    }
    print(text, 2.0f,  (line - 1)*12.0f + 2.0f, 0, 1);
    line++;
    
    // Border size, image processing mode, pattern detection mode.
    arGetBorderSize(gARHandle, &tempF);
    snprintf(text, sizeof(text), "Border: %0.1f%%", tempF*100.0);
    arGetPatternDetectionMode(gARHandle, &mode);
    switch (mode) {
        case AR_TEMPLATE_MATCHING_COLOR: text_p = "Colour template (pattern)"; break;
        case AR_TEMPLATE_MATCHING_MONO: text_p = "Mono template (pattern)"; break;
        case AR_MATRIX_CODE_DETECTION: text_p = "Matrix (barcode)"; break;
        case AR_TEMPLATE_MATCHING_COLOR_AND_MATRIX: text_p = "Colour template + Matrix (2 pass, pattern + barcode)"; break;
        case AR_TEMPLATE_MATCHING_MONO_AND_MATRIX: text_p = "Mono template + Matrix (2 pass, pattern + barcode "; break;
        default: text_p = "UNKNOWN"; break;
    }
    len = (int)strlen(text);
    snprintf(text + len, sizeof(text) - len, ", Pattern detection mode: %s", text_p);
    print(text, 2.0f,  (line - 1)*12.0f + 2.0f, 0, 1);
    line++;
    
    // Window size.
    snprintf(text, sizeof(text), "Drawing into %dx%d window", windowWidth, windowHeight);
    print(text, 2.0f,  (line - 1)*12.0f + 2.0f, 0, 1);
    line++;
    
}
