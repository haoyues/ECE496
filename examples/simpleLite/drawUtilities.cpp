//
//  drawUtilities.cpp
//  ARToolKit5
//
//  Created by Hao Yue Sheng on 2016-01-07.
//
//

#include "drawUtilities.hpp"
#include "opencvUtilities.hpp"

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
    
    drawObject(redTexture, vertexbuffer[textIdx], vertices[textIdx]);
 }

void drawFurniture(furniturePiece *pieces)
{
    int i;
    
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
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable( GL_POLYGON_OFFSET_FILL );
            glPolygonOffset( 1.0, 1.0 );
        
        
            if(pieces[i].marker != -1 && gMarkers[pieces[i].marker].gPatt_found)
            {
                drawObject(yellowTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx]);
            }
            else if (pieces[i].marker == -1 && opencvUtilities::gScrews[pieces[i].color].objectFound) {
                if (opencvUtilities::gScrews[pieces[i].color].numbersShown <
                    opencvUtilities::gScrews[pieces[i].color].numbersToShow) {
                    drawObject(yellowTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx]);
                    opencvUtilities::gScrews[pieces[i].color].numbersShown ++;
                    printf("numbersToShow: %d, numbersShown: %d\n", opencvUtilities::gScrews[pieces[i].color].numbersToShow, opencvUtilities::gScrews[pieces[i].color].numbersShown);
                } else {
                    drawObject(whiteTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx]);
                }
            }
            else
            {
                drawObject(whiteTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx]);
            }
        
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            drawObject(greenTexture, obj_vertexbuffer[pieces[i].bufferIdx], obj_vertices[pieces[i].bufferIdx]);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable( GL_POLYGON_OFFSET_FILL );
            glPopMatrix();
        }
        
    }
    
}

void drawAnimation(int step)
{
    drawFurniture(instruction[step]);
}

int loadAnimation(char * filename)
{
    FILE *animationFile = NULL;
    char name[MAX_NAME_LEN];
    int idx;
    int pieceIdx = 0;
    int step = 0;
    
    animationFile = fopen(filename, "rb");
    
    if(animationFile == NULL)
    {
        perror("file open error");
        return (FALSE);
    }
    
    fscanf(animationFile, "Total Step=%d\n", &totalNumofStep);
    
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
        if (fscanf(animationFile,"Name=%s ScaleX=%f ScaleY=%f ScaleZ=%f TransX=%f TransY=%f TransZ=%f RotateX=%f RotateY=%f RotateZ=%f Marker=%d Display=%d Color=%d\n",
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
                   &(instruction[step][pieceIdx].marker),
                   &(instruction[step][pieceIdx].display),
                   &(instruction[step][pieceIdx].color)) == 13)
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

void drawObject(GLuint texture, GLuint vertexbuffer, std::vector<glm::vec3> vertices)
{
   // GLint bufferSize;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, sizeof(glm::vec3), 0);
    
    //glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    //printf("buffer size is: %d\n", bufferSize);
    
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_TEXTURE_2D);
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

int loadFurnitureObject(char *filename, furniturePiece **pieces)
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
        if (fscanf(furnitureObjFile,"Name=%s ScaleX=%f ScaleY=%f ScaleZ=%f TransX=%f TransY=%f TransZ=%f RotateX=%f RotateY=%f RotateZ=%f Marker=%d Display=%d Color=%d\n",
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
                   &(*pieces)[pieceIdx].marker,
                   &(*pieces)[pieceIdx].display,
                   &(*pieces)[pieceIdx].color) != 13)
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

