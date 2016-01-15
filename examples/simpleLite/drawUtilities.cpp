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
    
    drawObject(redTexture, vertexbuffer[textIdx], vertices[textIdx]);
 }

/*void DrawCube()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 1.0, 1.0 );
    
    drawObject(redTexture, obj_vertexbuffer, obj_vertices);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    drawObject(greenTexture, obj_vertexbuffer, obj_vertices);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}*/
void drawFurniture()
{
    int i;
    for(i = 0; i < numofObjects; i++)
    {
        drawObject(redTexture, obj_vertexbuffer[i], obj_vertices[i]);
    }
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

int loadFurnitureObject(char *filename)
{
    
    FILE *furnitureObjFile = NULL;
    char name[MAX_PATTERN_NAME_LEN];
    int pieceIdx = 0;
    
    furnitureObjFile = fopen(filename, "rb");
    
    if(furnitureObjFile == NULL)
    {
        perror("file open error");
        return (FALSE);
    }
    
    fscanf(furnitureObjFile, "Count=%d\n", &numofObjects);
    obj_vertices = (std::vector<glm::vec3> *)malloc(numofObjects * sizeof(std::vector<glm::vec3>));
    obj_uvs = (std::vector<glm::vec2> *)malloc(numofObjects * sizeof(std::vector<glm::vec2>));
    obj_normals = (std::vector<glm::vec3> *)malloc(numofObjects * sizeof(std::vector<glm::vec3>));
    obj_vertexbuffer = (GLuint *)malloc(numofObjects * sizeof(GLuint));
    obj_uvbuffer = (GLuint *)malloc(numofObjects * sizeof(GLuint));
    memset(obj_vertices, 0x0, numofObjects * sizeof(std::vector<glm::vec3>));
    memset(obj_uvs, 0x0, numofObjects * sizeof(std::vector<glm::vec2>));
    memset(obj_normals, 0x0, numofObjects * sizeof(std::vector<glm::vec3>));
    memset(obj_vertexbuffer, 0x0, numofObjects * sizeof(GLuint));
    memset(obj_uvbuffer, 0x0, numofObjects * sizeof(GLuint));
    
    while (!feof(furnitureObjFile))
    {
        if (fscanf(furnitureObjFile,"Name=%s\n",name) != 1)
        {
            // found a line that does match this pattern: try again later, the file might be currently written
            perror("error");
            return (FALSE);
        }
        
        loadObject(name,
                   &obj_vertices[pieceIdx],
                   &obj_uvs[pieceIdx],
                   &obj_normals[pieceIdx],
                   &obj_vertexbuffer[pieceIdx],
                   &obj_uvbuffer[pieceIdx]);
        
        pieceIdx++;
    }
    
    return (TRUE);
}

/*void loadCube()
{
    loadObject("Data/mesh/table_top.obj",
               &obj_vertices,
               &obj_uvs,
               &obj_normals,
               &obj_vertexbuffer,
               &obj_uvbuffer);
    
    printf("vertexbuffer is: %d, uvbuffer is: %d\n", obj_vertexbuffer, obj_uvbuffer);
}*/

void loadScrew() {
    loadObject("Data/mesh/screw1.obj",
               &screw_vertices,
               &screw_uvs,
               &screw_normals,
               &screw_vertexbuffer,
               &screw_uvbuffer);
    
    printf("vertexbuffer is: %d, uvbuffer is: %d\n", screw_vertexbuffer, screw_uvbuffer);
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

