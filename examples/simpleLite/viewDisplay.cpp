
#include "viewDisplay.hpp"
#include "drawUtilities.hpp"
#include "opencvUtilities.hpp"

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

void loadObjects(void)
{
    //whiteTexture = loadBMP_custom("Data/mesh/white.bmp");
    redTexture = loadBMP_custom("Data/mesh/red.bmp");
    greenTexture = loadBMP_custom("Data/mesh/green.bmp");
    
    loadText();
    loadCube();
}

static void mainLoop(void)
{
    static int imageNumber = 0;
    static int ms_prev;
    int ms;
    float s_elapsed;
    ARUint8 *image;
    ARdouble err;
    
    // OpenCV vars
    Mat matCV( gARHandle->ysize, gARHandle->xsize, CV_8UC4 );
    Mat res(matCV.size(), matCV.type());
    Mat threshold;
    
    
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
        
        /*********************
         * Opencv part starts
         *********************/
        
        memcpy( matCV.data, image, matCV.rows * matCV.cols * matCV.channels() );
        
        xphoto::balanceWhite(matCV, res, cv::xphoto::WHITE_BALANCE_SIMPLE);
        
        ColoredObj redObj("redObj"), blueObj("greenObj"), yellowObj("yellowObj");
        inRange(res, redObj.getBGRmin(), redObj.getBGRmax(), threshold);
        morphOps(threshold);
        trackFilteredObject(redObj, threshold,res);
        
        inRange(res, blueObj.getBGRmin(), blueObj.getBGRmax(), threshold);
        morphOps(threshold);
        trackFilteredObject(blueObj, threshold,res);
        
        inRange(res, yellowObj.getBGRmin(), yellowObj.getBGRmax(), threshold);
        morphOps(threshold);
        trackFilteredObject(yellowObj, threshold,res);
        
        //imshow("Threshold",threshold);
        cv::imshow("after white balancing", res);
        
        
        /**********************
         * Opencv part ends
         **********************/
        
        // Tell GLUT the display has changed.
        // The next iteration through glutMainLoop, the window's display callback will be called to redisplay the window's normal plane.
        glutPostRedisplay();
    }
}


void View1_Display(void)
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
            DrawCube();
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


int setupMarker(char *filename)
{
    FILE *markerFile = NULL;
    int markerCount = 0;
    
    int numofMarker;
    char name[MAX_PATTERN_NAME_LEN];
    double dimension;
    tablePiece piece;
    markerFile = fopen("Data/marker.txt", "rb");
    
    if(markerFile == NULL)
    {
        perror("file open error");
        return (FALSE);
    }
    
    fscanf(markerFile, "Count=%d\n", &numofMarker);
    NUM_OF_MARKER = numofMarker;
    gMarkers = (ARMarker *)malloc(numofMarker * sizeof(ARMarker));
    memset(gMarkers, 0x0, numofMarker * sizeof(ARMarker));
    
    while (!feof(markerFile))
    {
        if (fscanf(markerFile,"Name=%s Dimension=%lf Piece=%d\n",name, &dimension, &piece) != 3)
        {
            // found a line that does match this pattern: try again later, the file might be currently written
            perror("error");
            return (FALSE);
        }
        
        // add here your code processing data
        strcpy(gMarkers[markerCount].gPatt_name, name);
        gMarkers[markerCount].gPatt_width = dimension;
        gMarkers[markerCount].piece = piece;
        
        markerCount++;
        
    }
    
    fclose(markerFile);
    return (TRUE);
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
    
    /*static void DrawModel(ARMarker marker,
     GLfloat vertices[][3],
     GLfloat faces[][4],
     GLfloat vertex_colors_right[][4],
     GLfloat vertex_colors_wrong[][4],
     GLfloat vertex_colors[][4])*/
    /*DrawModel(gMarkers[2],
     cube_vertices[8][3],
     cube_faces[6][4],
     cube_vertex_colors_right[8][4],
     cube_vertex_colors_wrong[8][4],
     cube_vertex_colors[8][4]);*/
    
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
    glTranslatef(-1.5f, -0.2f, -2.0f);  // Move right and into the screen
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
    glTranslatef(-0.5f, -0.2f, -2.0f);
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
    glTranslatef(0.0f, -0.2f, -2.0f);
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
    glTranslatef(0.5f, -0.2f, -2.0f);
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
    glTranslatef(1.0f, -0.2f, -2.0f);
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
