// GLES2Utils.hpp
// Nolan Check
// Created 5/9/2012

#ifndef _GLES2UTILS_HPP
#define _GLES2UTILS_HPP

#include <GLES2/gl2.h>

GLuint loadGLSLShader(GLenum type, const char* src);
GLuint loadGLSLProgram(const char* vShaderSrc, const char* fShaderSrc);

#endif
