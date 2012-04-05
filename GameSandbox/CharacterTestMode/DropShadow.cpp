// DropShadow.cpp
// Nolan Check
// Created 4/4/2012

#include "DropShadow.hpp"

#include <GLES2/gl2ext.h>

#include "WindowsUtils.hpp"

namespace CharacterTestMode
{
	
static GLuint LoadGLSLShader(GLenum type, const char* src)
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

static GLuint LoadGLSLProgram(const char* vShaderSrc, const char* fShaderSrc)
{
	GLuint vShader = LoadGLSLShader(GL_VERTEX_SHADER, vShaderSrc);
	GLuint fShader = LoadGLSLShader(GL_FRAGMENT_SHADER, fShaderSrc);

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

DropShadowCommon::DropShadowCommon()
	: m_program(0)
{
	static const char DROP_SHADOW_VERTEX_SHADER[] =
		"attribute vec2 a_pos;\n"
		"attribute vec2 a_tex;\n"
		"varying vec2 v_tex;\n"
		"void main()\n"
		"{\n"
			"gl_Position = vec4(a_pos, 0.0, 1.0);\n"
			"v_tex = a_tex;\n"
		"}\n"
		;
	static const char DROP_SHADOW_FRAGMENT_SHADER[] =
		"precision mediump float;\n"
		"varying vec2 v_tex;\n"
		"uniform vec2 u_offset;\n"
		"uniform vec2 u_sampleOffset;\n"
		"uniform sampler2D u_sampler;\n"
		"void main()\n"
		"{\n"
			"float sum = 0.0;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset - 4.0*u_sampleOffset).a * 0.05;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset - 3.0*u_sampleOffset).a * 0.09;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset - 2.0*u_sampleOffset).a * 0.12;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset - u_sampleOffset).a * 0.15;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset).a * 0.16;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset + u_sampleOffset).a * 0.15;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset + 2.0*u_sampleOffset).a * 0.12;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset + 3.0*u_sampleOffset).a * 0.09;\n"
			"sum += texture2D(u_sampler, v_tex + u_offset + 4.0*u_sampleOffset).a * 0.05;\n"
			"gl_FragColor = vec4(0.0, 0.0, 0.0, sum * 0.75);\n"
		"}\n"
		;
	m_program = LoadGLSLProgram(DROP_SHADOW_VERTEX_SHADER, DROP_SHADOW_FRAGMENT_SHADER);
	m_aposLoc = glGetAttribLocation(m_program, "a_pos");
	m_atexLoc = glGetAttribLocation(m_program, "a_tex");
	m_uoffsetLoc = glGetUniformLocation(m_program, "u_offset");
	m_usampleOffsetLoc = glGetUniformLocation(m_program, "u_sampleOffset");
	m_usamplerLoc = glGetUniformLocation(m_program, "u_sampler");
}

DropShadowCommon::~DropShadowCommon()
{
	glDeleteProgram(m_program);
	m_program = 0;
}

DropShadow::DropShadow(DropShadowCommon* common, GLuint srcTexture, int width, int height)
	: m_common(common),
	m_srcTexture(srcTexture),
	m_width(width),
	m_height(height),
	m_tempFramebuffer(0),
	m_tempTexture(0),
	m_dstFramebuffer(0),
	m_dstTexture(0)
{
	glGenFramebuffers(1, &m_tempFramebuffer);
	glGenTextures(1, &m_tempTexture);
	glGenFramebuffers(1, &m_dstFramebuffer);
	glGenTextures(1, &m_dstTexture);

	glBindTexture(GL_TEXTURE_2D, m_tempTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, m_width, m_height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, m_tempFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tempTexture, 0);

	glBindTexture(GL_TEXTURE_2D, m_dstTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, m_width, m_height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, m_dstFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_dstTexture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

DropShadow::~DropShadow()
{
	glDeleteTextures(1, &m_dstTexture);
	m_dstTexture = 0;
	glDeleteFramebuffers(1, &m_dstFramebuffer);
	m_dstFramebuffer = 0;
	glDeleteTextures(1, &m_tempTexture);
	m_tempTexture = 0;
	glDeleteFramebuffers(1, &m_tempFramebuffer);
	m_tempFramebuffer = 0;
}

void DropShadow::generate(const Vector2f& offset, const Vector2f& blurSize)
{
	glUseProgram(m_common->m_program);
	
	// XXX: Pay attention to texture coordinates! They are upside-down from what
	// you expect.
	GLfloat verts[] = { -1.0f, -1.0f, 0.0f, 0.0f, // Lower left
		1.0f, -1.0f, 1.0f, 0.0f, // Lower right
		-1.0f, 1.0f, 0.0f, 1.0f, // Upper left
		1.0f, 1.0f, 1.0f, 1.0f }; // Upper right

	glVertexAttribPointer(m_common->m_aposLoc, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), &verts[0]);
	glVertexAttribPointer(m_common->m_atexLoc, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), &verts[2]);

	glEnableVertexAttribArray(m_common->m_aposLoc);
	glEnableVertexAttribArray(m_common->m_atexLoc);

	// No blending
	glDisable(GL_BLEND);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_tempFramebuffer);
	
	glUniform2f(m_common->m_uoffsetLoc, offset.x, offset.y);
	// Blur along X axis
	glUniform2f(m_common->m_usampleOffsetLoc, blurSize.x, 0.0f);

	glBindTexture(GL_TEXTURE_2D, m_srcTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1i(m_common->m_usamplerLoc, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, m_dstFramebuffer);
	
	glUniform2f(m_common->m_uoffsetLoc, 0.0f, 0.0f);
	// Blur along Y axis
	glUniform2f(m_common->m_usampleOffsetLoc, 0.0f, blurSize.y);

	glBindTexture(GL_TEXTURE_2D, m_tempTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1i(m_common->m_usamplerLoc, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
