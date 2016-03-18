//
//  drawUtilities.cpp
//  ARToolKit5
//
//  Created by Hao Yue Sheng on 2016-01-07.
//
//

#include "drawUtilities.hpp"
#include "opencvUtilities.hpp"
#include "texture.h"

void DrawText(tablePiece piece)
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
    printf("textIdx is: %d\n", textIdx);
    printf("vertex buffer is: %d\n", vertexbuffer[textIdx]);
    drawObject(redTexture, vertexbuffer[textIdx], uvbuffer[textIdx], vertices[textIdx], uvs[textIdx]);
 }

void drawFurnitureAnimation(furniturePiece piece_start, furniturePiece piece_end, int pieceIdx)
{
    glPushMatrix();
    
    if((!startValueStored[pieceIdx]) &&
       (piece_start.translate[0] != piece_end.translate[0] ||
       piece_start.translate[1] != piece_end.translate[1] ||
       piece_start.translate[2] != piece_end.translate[2]))
    {
        translated[pieceIdx][0] = piece_start.translate[0];
        translated[pieceIdx][1] = piece_start.translate[1];
        translated[pieceIdx][2] = piece_start.translate[2];
        startValueStored[pieceIdx] = true;
    }
    else if (piece_start.translate[0] == piece_end.translate[0] &&
             piece_start.translate[1] == piece_end.translate[1] &&
             piece_start.translate[2] == piece_end.translate[2])
    {
        translated[pieceIdx][0] = piece_start.translate[0];
        translated[pieceIdx][1] = piece_start.translate[1];
        translated[pieceIdx][2] = piece_start.translate[2];
    }
    
    if(translated[pieceIdx][0] != piece_end.translate[0] ||
       translated[pieceIdx][1] != piece_end.translate[1] ||
       translated[pieceIdx][2] != piece_end.translate[2])
    {
        glTranslatef(translated[pieceIdx][0], translated[pieceIdx][1], translated[pieceIdx][2]);

        if(translated[pieceIdx][0] != piece_end.translate[0] && translated[pieceIdx][0] >= piece_end.translate[0])
        {
            translated[pieceIdx][0] -= ANIMATION_SPEED;
        }
        else if(translated[pieceIdx][0] != piece_end.translate[0] && translated[pieceIdx][0] < piece_end.translate[0])
        {
            translated[pieceIdx][0] += ANIMATION_SPEED;
        }
        if(translated[pieceIdx][1] != piece_end.translate[1] && translated[pieceIdx][1] >= piece_end.translate[1])
        {
            translated[pieceIdx][1] -= ANIMATION_SPEED;
        }
        else if(translated[pieceIdx][1] != piece_end.translate[1] && translated[pieceIdx][1] < piece_end.translate[1])
        {
            translated[pieceIdx][1] += ANIMATION_SPEED;
        }
        if(translated[pieceIdx][2] != piece_end.translate[2] && translated[pieceIdx][2] >= piece_end.translate[2])
        {
            translated[pieceIdx][2] -= ANIMATION_SPEED;
        }
        else if (translated[pieceIdx][2] != piece_end.translate[2] && translated[pieceIdx][2] < piece_end.translate[2])
        {
            translated[pieceIdx][2] += ANIMATION_SPEED;
        }
    }
    
    else
    {
        glTranslatef(translated[pieceIdx][0], translated[pieceIdx][1], translated[pieceIdx][2]);
        startValueStored[pieceIdx] = false;
        memset(translated[pieceIdx], 0, 3 * sizeof(float));
    }
    
    glScalef(piece_end.scale[0], piece_end.scale[1], piece_end.scale[2]);
    if(piece_end.rotate[0] != 0.0)
    {
        glRotatef(piece_end.rotate[0], 1.0, 0.0, 0.0);
    }
    if(piece_end.rotate[1] != 0.0)
    {
        glRotatef(piece_end.rotate[1], 0.0, 1.0, 0.0);
    }
    if(piece_end.rotate[2] != 0.0)
    {
        glRotatef(piece_end.rotate[2], 0.0, 0.0, 1.0);
    }
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glEnable( GL_POLYGON_OFFSET_FILL );
    //glPolygonOffset( 1.0, 1.0 );
    
    if(piece_end.front_marker != -1 && gMarkers[piece_end.front_marker].gPatt_found)
    {
        drawObject(yellowTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx]);
        
        correct_piece = 1;
    }
    else if (piece_end.front_marker == -1 && opencvUtilities::gScrews[piece_end.color].objectFound) {
        if (opencvUtilities::gScrews[piece_end.color].numbersShown <
            opencvUtilities::gScrews[piece_end.color].numbersToShow) {
            drawObject(yellowTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx]);
            opencvUtilities::gScrews[piece_end.color].numbersShown ++;
            printf("numbersToShow: %d, numbersShown: %d\n", opencvUtilities::gScrews[piece_end.color].numbersToShow, opencvUtilities::gScrews[piece_end.color].numbersShown);
            correct_piece = 1;
        } else {
            
            if(piece_end.front_marker != -1)
            {
                drawObject(woodTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx]);
                //correct_piece = 0;
            }
            else
            {
                drawObject(opencvUtilities::silverTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx]);
                //correct_piece = 0;
            }
        }
    }
    else
    {
        if(piece_end.front_marker != -1)
        {
            drawObject(woodTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx]);
            correct_piece = 0;
        }
        else
        {
            drawObject(opencvUtilities::gTextures[piece_end.color], obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx]);
            //correct_piece = 0;
        }
    }
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //drawObject(greenTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx]);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable( GL_POLYGON_OFFSET_FILL );
    glPopMatrix();
}

void drawFurnitureAnimation_shader(furniturePiece piece_start, furniturePiece piece_end, int pieceIdx, glm::mat4 PV)
{
    glm::mat4 Model;
    glPushMatrix();
    
    if((!startValueStored[pieceIdx]) &&
       (piece_start.translate[0] != piece_end.translate[0] ||
        piece_start.translate[1] != piece_end.translate[1] ||
        piece_start.translate[2] != piece_end.translate[2]))
    {
        translated[pieceIdx][0] = piece_start.translate[0];
        translated[pieceIdx][1] = piece_start.translate[1];
        translated[pieceIdx][2] = piece_start.translate[2];
        startValueStored[pieceIdx] = true;
    }
    else if (piece_start.translate[0] == piece_end.translate[0] &&
             piece_start.translate[1] == piece_end.translate[1] &&
             piece_start.translate[2] == piece_end.translate[2])
    {
        translated[pieceIdx][0] = piece_start.translate[0];
        translated[pieceIdx][1] = piece_start.translate[1];
        translated[pieceIdx][2] = piece_start.translate[2];
    }
    
    if(translated[pieceIdx][0] != piece_end.translate[0] ||
       translated[pieceIdx][1] != piece_end.translate[1] ||
       translated[pieceIdx][2] != piece_end.translate[2])
    {
        glTranslatef(translated[pieceIdx][0], translated[pieceIdx][1], translated[pieceIdx][2]);
        
        if(translated[pieceIdx][0] != piece_end.translate[0] && translated[pieceIdx][0] >= piece_end.translate[0])
        {
            translated[pieceIdx][0] -= ANIMATION_SPEED;
        }
        else if(translated[pieceIdx][0] != piece_end.translate[0] && translated[pieceIdx][0] < piece_end.translate[0])
        {
            translated[pieceIdx][0] += ANIMATION_SPEED;
        }
        if(translated[pieceIdx][1] != piece_end.translate[1] && translated[pieceIdx][1] >= piece_end.translate[1])
        {
            translated[pieceIdx][1] -= ANIMATION_SPEED;
        }
        else if(translated[pieceIdx][1] != piece_end.translate[1] && translated[pieceIdx][1] < piece_end.translate[1])
        {
            translated[pieceIdx][1] += ANIMATION_SPEED;
        }
        if(translated[pieceIdx][2] != piece_end.translate[2] && translated[pieceIdx][2] >= piece_end.translate[2])
        {
            translated[pieceIdx][2] -= ANIMATION_SPEED;
        }
        else if (translated[pieceIdx][2] != piece_end.translate[2] && translated[pieceIdx][2] < piece_end.translate[2])
        {
            translated[pieceIdx][2] += ANIMATION_SPEED;
        }
    }
    
    else
    {
        glTranslatef(translated[pieceIdx][0], translated[pieceIdx][1], translated[pieceIdx][2]);
        startValueStored[pieceIdx] = false;
        memset(translated[pieceIdx], 0, 3 * sizeof(float));
    }
    
    glScalef(piece_end.scale[0], piece_end.scale[1], piece_end.scale[2]);
    if(piece_end.rotate[0] != 0.0)
    {
        glRotatef(piece_end.rotate[0], 1.0, 0.0, 0.0);
    }
    if(piece_end.rotate[1] != 0.0)
    {
        glRotatef(piece_end.rotate[1], 0.0, 1.0, 0.0);
    }
    if(piece_end.rotate[2] != 0.0)
    {
        glRotatef(piece_end.rotate[2], 0.0, 0.0, 1.0);
    }
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glEnable( GL_POLYGON_OFFSET_FILL );
    //glPolygonOffset( 1.0, 1.0 );
    
    glGetFloatv(GL_MODELVIEW_MATRIX, &Model[0][0]);
    glm::mat4 MVP = PV * Model;
    
    if(piece_end.front_marker != -1 && gMarkers[piece_end.front_marker].gPatt_found)
    {
        drawObject_shader(yellowTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx], MVP);
        
        correct_piece = 1;
    }
    else if (piece_end.front_marker == -1 && opencvUtilities::gScrews[piece_end.color].objectFound) {
        if (opencvUtilities::gScrews[piece_end.color].numbersShown <
            opencvUtilities::gScrews[piece_end.color].numbersToShow) {
            drawObject_shader(yellowTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx], MVP);
            opencvUtilities::gScrews[piece_end.color].numbersShown ++;
            printf("numbersToShow: %d, numbersShown: %d\n", opencvUtilities::gScrews[piece_end.color].numbersToShow, opencvUtilities::gScrews[piece_end.color].numbersShown);
            correct_piece = 1;
        } else {
            
            if(piece_end.front_marker != -1)
            {
                drawObject_shader(woodTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx], MVP);
                //correct_piece = 0;
            }
            else
            {
                drawObject_shader(opencvUtilities::silverTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx], MVP);
                //correct_piece = 0;
            }
        }
    }
    else
    {
        if(piece_end.front_marker != -1)
        {
            drawObject_shader(woodTexture, obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx], MVP);
            correct_piece = 0;
        }
        else
        {
            drawObject_shader(opencvUtilities::gTextures[piece_end.color], obj_vertexbuffer[piece_end.bufferIdx], obj_uvbuffer[piece_end.bufferIdx], obj_vertices[piece_end.bufferIdx], obj_uvs[piece_end.bufferIdx], MVP);
            //correct_piece = 0;
        }
    }

    glDisable( GL_POLYGON_OFFSET_FILL );
    glPopMatrix();
}

void drawFurniture(furniturePiece *pieces)
{
    int i;
    int count = 0;
    
    for (i = 0; i < MAX_COLOUR_NUM; i++) {
        opencvUtilities::gScrews[i].numbersShown = 0;
    }
    
    for(i = 0; i < numofPieces; i++)
    {
        //if all pieces are not displayed, then inventory is empyt, should move onto next file
        if(pieces[i].display)
        {
            count++;
            glPushMatrix();
        
            glTranslatef(pieces[i].translate[0], pieces[i].translate[1], pieces[i].translate[2]);
            glScalef(pieces[i].scale[0], pieces[i].scale[1], pieces[i].scale[2]);
            if(pieces[i].rotate[0] != 0.0)
            {
                glRotatef(pieces[i].rotate[0], 1.0, 0.0, 0.0);
            }
            if(pieces[i].rotate[1] != 0.0)
            {
                glRotatef(pieces[i].rotate[1], 0.0, 1.0, 0.0);
            }
            if(pieces[i].rotate[2] != 0.0)
            {
                glRotatef(pieces[i].rotate[2], 0.0, 0.0, 1.0);
            }
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            //glEnable( GL_POLYGON_OFFSET_FILL );
            //glPolygonOffset( 1.0, 1.0 );
        
        
            if(pieces[i].front_marker != -1 && gMarkers[pieces[i].front_marker].gPatt_found)
            {
                drawObject(yellowTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx]);
            }
            else if (pieces[i].front_marker == -1 && opencvUtilities::gScrews[pieces[i].color].objectFound) {
                if (opencvUtilities::gScrews[pieces[i].color].numbersShown <
                    opencvUtilities::gScrews[pieces[i].color].numbersToShow) {
                    // Colour detected, need to highlight
                    drawObject(opencvUtilities::silverTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx]);
                    opencvUtilities::gScrews[pieces[i].color].numbersShown ++;
                } else {
                    // No need to highlight, show original colour
                    drawObject(opencvUtilities::gTextures[pieces[i].color], obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx]);
                }
            }
            else
            {
                if(pieces[i].color != -1)
                {
                    drawObject(opencvUtilities::gTextures[pieces[i].color], obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx]);
                }
                else
                {
                    drawObject(woodTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx]);
                }
            }
        
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //drawObject(greenTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx]);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            //glDisable( GL_POLYGON_OFFSET_FILL );
            glPopMatrix();
        }
        
    }
    
}

void drawFurniture_shader(furniturePiece *pieces, glm::mat4 PV)
{
    int i;
    glm::mat4 Model;
    
    for (i = 0; i < MAX_COLOUR_NUM; i++) {
        opencvUtilities::gScrews[i].numbersShown = 0;
    }
    
    for(i = 0; i < numofPieces; i++)
    {
        if(pieces[i].display)
        {
            glPushMatrix();
            
            glTranslatef(pieces[i].translate[0], pieces[i].translate[1], pieces[i].translate[2]);
            glScalef(pieces[i].scale[0], pieces[i].scale[1], pieces[i].scale[2]);
            if(pieces[i].rotate[0] != 0.0)
            {
                glRotatef(pieces[i].rotate[0], 1.0, 0.0, 0.0);
            }
            if(pieces[i].rotate[1] != 0.0)
            {
                glRotatef(pieces[i].rotate[1], 0.0, 1.0, 0.0);
            }
            if(pieces[i].rotate[2] != 0.0)
            {
                glRotatef(pieces[i].rotate[2], 0.0, 0.0, 1.0);
            }
            
            glGetFloatv(GL_MODELVIEW_MATRIX, &Model[0][0]);
            glm::mat4 MVP = PV * Model;
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            //glEnable( GL_POLYGON_OFFSET_FILL );
            //glPolygonOffset( 1.0, 1.0 );
            
            
            if(pieces[i].front_marker != -1 && gMarkers[pieces[i].front_marker].gPatt_found)
            {
                drawObject_shader(yellowTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx], MVP);
            }
            else if (pieces[i].front_marker == -1 && opencvUtilities::gScrews[pieces[i].color].objectFound) {
                if (opencvUtilities::gScrews[pieces[i].color].numbersShown <
                    opencvUtilities::gScrews[pieces[i].color].numbersToShow) {
                    // Colour detected, need to highlight
                    drawObject_shader(opencvUtilities::silverTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx], MVP);
                    opencvUtilities::gScrews[pieces[i].color].numbersShown ++;
                } else {
                    // No need to highlight, show original colour
                    drawObject_shader(opencvUtilities::gTextures[pieces[i].color], obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx], MVP);
                }
            }
            else
            {
                if(pieces[i].color != -1)
                {
                    drawObject_shader(opencvUtilities::gTextures[pieces[i].color], obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx], MVP);
                }
                else
                {
                    drawObject_shader(woodTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_uvbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx], obj_uvs[pieces[i].bufferIdx], MVP);
                }
            }
            
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //drawObject(greenTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx]);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            //glDisable( GL_POLYGON_OFFSET_FILL );
            glPopMatrix();
        }
        
    }
    
}

static bool arrayCompare(float * array1, float * array2, int size)
{
    int idx;
    for(idx = 0; idx < size; idx++)
    {
        if(array1[idx] != array2[idx])
        {
            return false;
        }
    }
    
    return true;
}

void drawAnimation_shader(int step, glm::mat4 PV)
{
    //count the number of displayed pieces for previous step and current step
    int prev_step_piece = 0;
    int curr_step_piece = 0;
    int idx;
    
    if(step > 0)
    {
        for(idx= 0; idx < numofPieces; idx++)
        {
            if(instruction[step - 1][idx].display == 1)
            {
                prev_step_piece++;
                pieces[idx].display = 0;
                
            }
            if(instruction[step][idx].display == 1)
            {
                curr_step_piece++;
                
            }
        }
        
        if(prev_step_piece == curr_step_piece)
        {
            for(idx = 0; idx < numofPieces; idx++)
            {
                if(instruction[step][idx].display == 1)
                {
                    drawFurnitureAnimation_shader(instruction[step-1][idx], instruction[step][idx], idx, PV);
                }
            }
        }
        else
        {
            memset(startValueStored, 0, numofPieces * sizeof(bool));
            drawFurniture_shader(instruction[step], PV);
        }
    }
    else
    {
        drawFurniture_shader(instruction[step], PV);
    }
}

void drawAnimation(int step)
{
    //count the number of displayed pieces for previous step and current step
    int prev_step_piece = 0;
    int curr_step_piece = 0;
    int idx;
    
    /************ SPECIAL CASE: PREVIOUS STEP *************/
    if(step == 0 &&  loadInventoryFile )
    {
        for(idx= 0; idx < numofPieces; idx++)
        {
            if(instruction[step][idx].display == 1)
            {
                pieces[idx].display = 1;
            }
        }
        loadInventoryFile = 0;
    }
    
    if(step > 0)
    {
        for(idx= 0; idx < numofPieces; idx++)
        {
            if(!isSubtract)
            {
                pieces[idx].display = 1;
            }
            
            if(instruction[step - 1][idx].display == 1)
            {
                prev_step_piece++;
                
                if(!isSubtract)
                {
                    pieces[idx].display = 0;
                }
            }
            if(instruction[step][idx].display == 1)
            {
                curr_step_piece++;
            }
            
            /************ SPECIAL CASE: SUBTRACT PARTS *************/
            if(isSubtract && instruction[step][idx].display == 1 && instruction[step+1][idx].display == 0)
            {
                pieces[idx].color = 2;

            }
            /************ SPECIAL CASE: PREVIOUS STEP *************/
            /*if(loadInventoryFile && instruction[step -1][idx].display == 1 && instruction[step][idx].display == 1)
            {
                pieces[idx].display = 1;
            }*/
        }
        
        if(prev_step_piece == curr_step_piece)
        {
            for(idx = 0; idx < numofPieces; idx++)
            {
                if(instruction[step][idx].display == 1)
                {
                    /*if(arrayCompare(instruction[step][idx].rotate, instruction[step-1][idx].rotate, 3) &&
                       arrayCompare(instruction[step][idx].translate, instruction[step-1][idx].translate, 3) &&
                       arrayCompare(instruction[step][idx].scale, instruction[step-1][idx].scale, 3))
                    {
                        continue;
                    }
                    else
                    {
                        drawFurnitureAnimation(furniturePiece piece_start, furniturePiece piece_end);
                    }*/
                    
                    drawFurnitureAnimation(instruction[step-1][idx], instruction[step][idx], idx);
                }
            }
        }
        else
        {
            memset(startValueStored, 0, numofPieces * sizeof(bool));
            drawFurniture(instruction[step]);
        }
    }
    else
    {
        drawFurniture(instruction[step]);
    }
 }

int loadAnimation(const char * filename)
{
    FILE *animationFile = NULL;
    char name[MAX_NAME_LEN];
    int idx;
    int pieceIdx = 0;
    int step = 0;
    
    translated = (float **)malloc(numofPieces * sizeof(float *));
    for(idx = 0; idx < numofPieces; idx++)
    {
        translated[idx] = (float *)malloc(3 * sizeof(float));

    }
    startValueStored = (bool *)malloc(numofPieces * sizeof(bool));

    animationFile = fopen(filename, "rb");
    
    if(animationFile == NULL)
    {
        perror("file open error");
        return (FALSE);
    }
    
    fscanf(animationFile, "Total Step=%d Subtract=%d\n", &totalNumofStep, &isSubtract);
    
    printf("total number of step is: %d\n", totalNumofStep);
    instruction = (furniturePiece **)malloc(totalNumofStep * sizeof(furniturePiece*));
    for(idx = 0; idx < totalNumofStep; idx++)
    {
        instruction[idx] = (furniturePiece *)malloc(numofPieces * sizeof(furniturePiece));
        memset((instruction[idx]), 0x0, numofPieces * sizeof(furniturePiece));
        for(pieceIdx = 0; pieceIdx < numofPieces; pieceIdx++)
        {
            instruction[idx][pieceIdx].bufferIdx = model[pieceIdx].bufferIdx;
        }
    }
    
    printf("instruction 0: %x, instruction 1: %x\n", instruction[0], instruction[1]);
    printf("piece 0: %x\n",instruction[0][0]);
    
    while (!feof(animationFile))
    {
        if (fscanf(animationFile,"Name=%s ScaleX=%f ScaleY=%f ScaleZ=%f TransX=%f TransY=%f TransZ=%f RotateX=%f RotateY=%f RotateZ=%f FrontMarker=%d BackMarker=%d Display=%d Color=%d\n",
                   name,
                   &(instruction[step][pieceIdx].scale[0]),
                   &(instruction[step][pieceIdx].scale[1]),
                   &(instruction[step][pieceIdx].scale[2]),
                   &(instruction[step][pieceIdx].translate[0]),
                   &(instruction[step][pieceIdx].translate[1]),
                   &(instruction[step][pieceIdx].translate[2]),
                   &(instruction[step][pieceIdx].rotate[0]),
                   &(instruction[step][pieceIdx].rotate[1]),
                   &(instruction[step][pieceIdx].rotate[2]),
                   &(instruction[step][pieceIdx].front_marker),
                   &(instruction[step][pieceIdx].back_marker),
                   &(instruction[step][pieceIdx].display),
                   &(instruction[step][pieceIdx].color)) == 14)
        {
            pieceIdx++;
        }

        else if(fscanf(animationFile, "Step=%d\n", &step) == 1)
        {
           pieceIdx = 0;
        }

        else
        {
            perror("error");
            return (FALSE);
        }


    }
    
    return (TRUE);
}

void drawObject(GLuint texture, GLuint vertexbuffer, GLuint uvbuffer, std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs)
{
   // GLint bufferSize;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uvbuffer);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, sizeof(glm::vec3), 0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(glm::vec2), 0);
    
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_TEXTURE_2D);
}

void drawObject_shader(GLuint texture, GLuint vertexbuffer, GLuint uvbuffer, std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, glm::mat4 MVP)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    //glUniform1i(TextureID, 0);
    
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(vertexPosition_modelspaceID,  // The attribute we want to configure
                          3,                            // size
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          0,                            // stride
                          (void*)0                      // array buffer offset
                          );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(vertexUVID);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(vertexUVID,                   // The attribute we want to configure
                          2,                            // size : U+V => 2
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          0,                            // stride
                          (void*)0                      // array buffer offset
                          );
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // 12*3 indices starting at 0 -> 12 triangles
    
    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexUVID);
}


void loadText()
{
    int textIdx;
    for(textIdx = 0; textIdx < NUM_OF_NUMBER; textIdx++)
    {
        char *number = (char*)malloc(sizeof(char));
        sprintf(number, "%d", textIdx);
        printf("textIdx is: %d\n", textIdx);
        
        char file_extension[] = ".obj";
        char file_dir[] = "Data/mesh/";
        
        char *file_name = strcat(number, file_extension);
        char *obj_file = strcat(file_dir, file_name);
        
        printf("obj file name is: %s\n", obj_file);
        
        loadObject(obj_file,
                   &vertices[textIdx],
                   &uvs[textIdx],
                   &normals[textIdx],
                   &vertexbuffer[textIdx],
                   &uvbuffer[textIdx]);
        
        printf("vertexbuffer is: %d, uvbuffer is: %d\n", vertexbuffer[textIdx], uvbuffer[textIdx]);
    }
}

void loadLabel()
{
    //load the texture
    labelTexture = loadBMP_custom("Data/mesh/label.bmp");
    loadObject("Data/mesh/plane.obj",
               &label_vertices,
               &label_uvs,
               &label_normals,
               &label_vertexbuffer,
               &label_uvbuffer);

}

int loadFurnitureObject(const char *filename, furniturePiece **pieces)
{
    
    FILE *furnitureObjFile = NULL;
    char prev_name[MAX_NAME_LEN];
    char name[MAX_NAME_LEN];
    int pieceIdx = 0;
    int objIdx = 0;
    
    furnitureObjFile = fopen(filename, "rb");
    
    if(furnitureObjFile == NULL)
    {
        perror("file open error");
        return (FALSE);
    }
    
    fscanf(furnitureObjFile, "Count=%d Piece Count=%d\n", &numofObjects, &numofPieces);
    obj_vertices = (std::vector<glm::vec3> *)malloc(numofObjects * sizeof(std::vector<glm::vec3>));
    obj_uvs = (std::vector<glm::vec2> *)malloc(numofObjects * sizeof(std::vector<glm::vec2>));
    obj_normals = (std::vector<glm::vec3> *)malloc(numofObjects * sizeof(std::vector<glm::vec3>));
    obj_vertexbuffer = (GLuint *)malloc(numofObjects * sizeof(GLuint));
    obj_uvbuffer = (GLuint *)malloc(numofObjects * sizeof(GLuint));
    (*pieces) = (furniturePiece *)malloc(numofPieces * sizeof(furniturePiece));
    
    memset(obj_vertices, 0x0, numofObjects * sizeof(std::vector<glm::vec3>));
    memset(obj_uvs, 0x0, numofObjects * sizeof(std::vector<glm::vec2>));
    memset(obj_normals, 0x0, numofObjects * sizeof(std::vector<glm::vec3>));
    memset(obj_vertexbuffer, 0x0, numofObjects * sizeof(GLuint));
    memset(obj_uvbuffer, 0x0, numofObjects * sizeof(GLuint));
    memset((*pieces), 0x0, numofPieces * sizeof(furniturePiece));
    
    while (!feof(furnitureObjFile))
    {
        if (fscanf(furnitureObjFile,"Name=%s ScaleX=%f ScaleY=%f ScaleZ=%f TransX=%f TransY=%f TransZ=%f RotateX=%f RotateY=%f RotateZ=%f FrontMarker=%d BackMarker=%d Display=%d Color=%d\n",
                   name,
                   &(*pieces)[pieceIdx].scale[0],
                   &(*pieces)[pieceIdx].scale[1],
                   &(*pieces)[pieceIdx].scale[2],
                   &(*pieces)[pieceIdx].translate[0],
                   &(*pieces)[pieceIdx].translate[1],
                   &(*pieces)[pieceIdx].translate[2],
                   &(*pieces)[pieceIdx].rotate[0],
                   &(*pieces)[pieceIdx].rotate[1],
                   &(*pieces)[pieceIdx].rotate[2],
                   &(*pieces)[pieceIdx].front_marker,
                   &(*pieces)[pieceIdx].back_marker,
                   &(*pieces)[pieceIdx].display,
                   &(*pieces)[pieceIdx].color) != 14)
        {
            // found a line that does match this pattern: try again later, the file might be currently written
            perror("error");
            return (FALSE);
        }
        
        if(strcmp(prev_name, name) == 0)
        {
            (*pieces)[pieceIdx].bufferIdx = objIdx - 1;
            pieceIdx++;
            continue;
        }
        strcpy(prev_name, name);
        (*pieces)[pieceIdx].bufferIdx = objIdx;
        
        loadObject(name,
                   &obj_vertices[objIdx],
                   &obj_uvs[objIdx],
                   &obj_normals[objIdx],
                   &obj_vertexbuffer[objIdx],
                   &obj_uvbuffer[objIdx]);
        
        pieceIdx++;
        objIdx++;
    }
    return (TRUE);
}

void loadObject(char * obj_file,
                std::vector<glm::vec3> *vertices,
                std::vector<glm::vec2> *uvs,
                std::vector<glm::vec3> *normals,
                GLuint *vertexbuffer,
                GLuint *uvbuffer)
{
    bool objloaded  = loadOBJ(obj_file, *vertices, *uvs, *normals);
    
    if(!objloaded)
    {
        printf("object is not loaded!!");
    }
    
    glGenBuffers(1, vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, (*vertices).size() * sizeof(glm::vec3), &(*vertices)[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, (*uvs).size() * sizeof(glm::vec2), &(*uvs)[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void drawBackground(const float width, const float height, const float x, const float y)
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

