// GLES2Utils.cpp
// Nolan Check
// Created 5/9/2012

#include "GLES2Utils.hpp"

#include <exception>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

GLuint loadGLSLShader(GLenum type, const char* src)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	
	GLint len = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 1)
	{
		char* log = new char[len];

		glGetShaderInfoLog(shader, len, NULL, log);
		OutputDebugStringA("Shader info log:\n");
		OutputDebugStringA(log);

		delete[] log;
	}

	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		glDeleteShader(shader);
		shader = 0;
		throw std::exception("Failed to compile GLSL shader");
	}

	return shader;
}

GLuint loadGLSLProgram(const char* vShaderSrc, const char* fShaderSrc)
{
	GLuint vShader = loadGLSLShader(GL_VERTEX_SHADER, vShaderSrc);
	GLuint fShader = loadGLSLShader(GL_FRAGMENT_SHADER, fShaderSrc);

	GLuint program = glCreateProgram();

	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	glLinkProgram(program);

	// Delete shaders; they are no longer needed
	glDeleteShader(fShader);
	glDeleteShader(vShader);

	GLint len = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	if (len > 1)
	{
		char* log = new char[len];

		glGetProgramInfoLog(program, len, NULL, log);
		OutputDebugStringA("Program info log:\n");
		OutputDebugStringA(log);

		delete[] log;
	}

	GLint linked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		glDeleteProgram(program);
		program = 0;
		throw std::exception("Failed to link GLSL program");
	}

	return program;
}

void glUniformMatrix4x4f(GLint location, const Matrix4x4f& mat)
{
	// GLES requires input in column-major order
	float f[16] = { mat._11, mat._21, mat._31, mat._41,
		mat._12, mat._22, mat._32, mat._42,
		mat._13, mat._23, mat._33, mat._43,
		mat._14, mat._24, mat._34, mat._44 };
	glUniformMatrix4fv(location, 1, GL_FALSE, f);
}
