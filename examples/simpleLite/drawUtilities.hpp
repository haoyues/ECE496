//
//  drawUtilities.hpp
//  ARToolKit5
//
//  Created by Hao Yue Sheng on 2016-01-07.
//
//

#ifndef drawUtilities_hpp
#define drawUtilities_hpp

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#  define snprintf _snprintf
#endif
#include <stdlib.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#include <OpenGl/glext.h>

#include <AR/video.h>

#include "objloader.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define NUM_OF_NUMBER           10
#define ANIMATION_SPEED         0.5
#define MAX_NAME_LEN            100

typedef struct _furniturePiece
{
    GLfloat scale[3];
    GLfloat translate[3];
    GLfloat rotate[3];
    GLuint bufferIdx;
    int marker;
    int display;
    int color;
}furniturePiece;

typedef enum _drawType
{
    INVENTORY,
    MODEL
}drawType;

extern std::vector<glm::vec3> vertices[NUM_OF_NUMBER];
extern std::vector<glm::vec2> uvs[NUM_OF_NUMBER];
extern std::vector<glm::vec3> normals[NUM_OF_NUMBER];
extern GLuint vertexbuffer[NUM_OF_NUMBER];
extern GLuint uvbuffer[NUM_OF_NUMBER];
extern GLuint woodTexture;
extern GLuint redTexture;
extern GLuint yellowTexture;

//label on welcome page
extern std::vector<glm::vec3> label_vertices;
extern std::vector<glm::vec2> label_uvs;
extern std::vector<glm::vec3> label_normals;
extern GLuint label_vertexbuffer;
extern GLuint label_uvbuffer;
extern GLuint labelTexture;

//read a set of objects from file
extern int numofObjects;
extern int numofPieces;
extern int totalNumofStep;
extern std::vector<glm::vec3> *obj_vertices;
extern std::vector<glm::vec2> *obj_uvs;
extern std::vector<glm::vec3> *obj_normals;
extern GLuint *obj_vertexbuffer;
extern GLuint *obj_uvbuffer;
extern furniturePiece *pieces;
extern furniturePiece *model;
extern furniturePiece **instruction;
extern float **translated;
extern bool *startValueStored;

extern std::vector<glm::vec3> screw_vertices;
extern std::vector<glm::vec2> screw_uvs;
extern std::vector<glm::vec3> screw_normals;
extern GLuint screw_vertexbuffer;
extern GLuint screw_uvbuffer;

extern ARMarker *gMarkers;

void DrawText(tablePiece piece);
void DrawCube();
void drawFurniture(furniturePiece *pieces);
void drawAnimation(int step);
int loadAnimation(char * filename);
void loadText();
void loadLabel();
void loadCube();
void loadScrew();
void loadObject(char * obj_file,
                std::vector<glm::vec3> *vertices,
                std::vector<glm::vec2> *uvs,
                std::vector<glm::vec3> *normals,
                GLuint *vertexbuffer,
                GLuint *uvbuffer);
int loadFurnitureObject(char *filename, furniturePiece **pieces);
void drawObject(GLuint texture, GLuint vertexbuffer, GLuint uvbuffer, std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs);
void drawBackground(const float width, const float height, const float x, const float y);


#endif /* drawUtilities_hpp */
