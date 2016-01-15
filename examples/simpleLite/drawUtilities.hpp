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

extern std::vector<glm::vec3> vertices[NUM_OF_NUMBER];
extern std::vector<glm::vec2> uvs[NUM_OF_NUMBER];
extern std::vector<glm::vec3> normals[NUM_OF_NUMBER];
extern GLuint vertexbuffer[NUM_OF_NUMBER];
extern GLuint uvbuffer[NUM_OF_NUMBER];
extern GLuint whiteTexture;
extern GLuint redTexture;
extern GLuint greenTexture;

extern std::vector<glm::vec3> obj_vertices;
extern std::vector<glm::vec2> obj_uvs;
extern std::vector<glm::vec3> obj_normals;
extern GLuint obj_vertexbuffer;
extern GLuint obj_uvbuffer;

void DrawText(tablePiece piece);
void DrawCube();
void loadText();
void loadCube();
void loadObject(char * obj_file,
                std::vector<glm::vec3> *vertices,
                std::vector<glm::vec2> *uvs,
                std::vector<glm::vec3> *normals,
                GLuint *vertexbuffer,
                GLuint *uvbuffer);
void drawObject(GLuint texture, GLuint vertexbuffer, std::vector<glm::vec3> vertices);
void drawBackground(const float width, const float height, const float x, const float y);


#endif /* drawUtilities_hpp */
