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


#define NUM_OF_NUMBER           10

static std::vector<glm::vec3> vertices[NUM_OF_NUMBER];
static std::vector<glm::vec2> uvs[NUM_OF_NUMBER];
static std::vector<glm::vec3> normals[NUM_OF_NUMBER];
static GLuint vertexbuffer[NUM_OF_NUMBER];
static GLuint uvbuffer[NUM_OF_NUMBER];
static GLuint whiteTexture;
static GLuint redTexture;
static GLuint greenTexture;

static std::vector<glm::vec3> cube_vertices;
static std::vector<glm::vec2> cube_uvs;
static std::vector<glm::vec3> cube_normals;
static GLuint cube_vertexbuffer;
static GLuint cube_uvbuffer;

void DrawText(tablePiece piece);
void DrawCube();
void loadText();
void loadCube();
void drawBackground(const float width, const float height, const float x, const float y);


#endif /* drawUtilities_hpp */
