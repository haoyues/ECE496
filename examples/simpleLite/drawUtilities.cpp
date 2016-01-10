//
//  drawUtilities.cpp
//  ARToolKit5
//
//  Created by Hao Yue Sheng on 2016-01-07.
//
//

#include "drawUtilities.hpp"

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
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, redTexture);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[textIdx]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(glm::vec3), 0);
    
    glDrawArrays(GL_TRIANGLES, 0, vertices[textIdx].size() * 3);
    
    glDisable(GL_TEXTURE_2D);
}

void DrawCube()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 1.0, 1.0 );
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, redTexture);
    
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertexbuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, sizeof(glm::vec3), 0);
    
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size() * 3);
    
    glDisable(GL_TEXTURE_2D);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, greenTexture);
    
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertexbuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, sizeof(glm::vec3), 0);
    
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size() * 3);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
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
        
        bool objloaded  = loadOBJ(obj_file, vertices[textIdx], uvs[textIdx], normals[textIdx]);
        
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

void loadCube()
{
    char *obj_file = "Data/mesh/cube.obj";
    bool objloaded  = loadOBJ(obj_file, cube_vertices, cube_uvs, cube_normals);
    
    if(!objloaded)
    {
        printf("cube is not loaded!!");
    }
    
    glGenBuffers(1, &cube_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, cube_vertices.size() * sizeof(glm::vec3), &cube_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &cube_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, cube_uvs.size() * sizeof(glm::vec2), &cube_uvs[0], GL_STATIC_DRAW);
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

