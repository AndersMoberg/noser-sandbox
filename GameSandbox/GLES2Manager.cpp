// GLES2Manager.cpp
// Nolan Check
// Created 3/16/2012

#include "GLES2Manager.hpp"

#include <vector>

#include <wincodec.h>

#include <GLES2/gl2ext.h>

#include "WindowsUtils.hpp"

GLES2Manager::GLES2Manager()
	: m_eglDisplay(0),
	m_eglSurface(0),
	m_eglContext(0)
{ }

GLES2Manager::TexturedQuadProgram::TexturedQuadProgram()
	: program(0)
{ }

GLES2Manager::~GLES2Manager()
{
	glDeleteProgram(m_texturedQuadProgram.program);
	m_texturedQuadProgram.program = 0;

	eglDestroyContext(m_eglDisplay, m_eglContext);
	m_eglContext = 0;

	eglDestroySurface(m_eglDisplay, m_eglSurface);
	m_eglSurface = 0;

	eglTerminate(m_eglDisplay);
	m_eglDisplay = 0;
}

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

GLES2ManagerPtr GLES2Manager::Create(HWND hWnd)
{
	GLES2ManagerPtr p(new GLES2Manager);

	p->m_hWnd = hWnd;

	p->m_eglDisplay = eglGetDisplay(GetDC(hWnd));
	if (p->m_eglDisplay == EGL_NO_DISPLAY) {
		throw std::exception("Failed to get display for EGL");
	}

	if (eglInitialize(p->m_eglDisplay, NULL, NULL) != EGL_TRUE) {
		throw std::exception("Failed to initialize EGL");
	}
	
	EGLint numConfigs;
	if (eglGetConfigs(p->m_eglDisplay, NULL, 0, &numConfigs) != EGL_TRUE) {
		throw std::exception("Failed to get number of EGL configurations");
	}

	static const EGLint chooseConfigAttribs[] = {
		EGL_NONE
	};
	EGLConfig eglConfig;
	if (eglChooseConfig(p->m_eglDisplay, chooseConfigAttribs, &eglConfig, 1, &numConfigs) != EGL_TRUE) {
		throw std::exception("Failed to choose an EGL configuration");
	}

	p->m_eglSurface = eglCreateWindowSurface(p->m_eglDisplay, eglConfig, hWnd, NULL);
	if (p->m_eglSurface == EGL_NO_SURFACE) {
		throw std::exception("Failed to create EGL window surface");
	}

	static const EGLint createContextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	p->m_eglContext = eglCreateContext(p->m_eglDisplay, eglConfig, EGL_NO_CONTEXT, createContextAttribs);
	if (p->m_eglContext == EGL_NO_CONTEXT) {
		throw std::exception("Failed to create EGL context");
	}

	if (eglMakeCurrent(p->m_eglDisplay, p->m_eglSurface, p->m_eglSurface, p->m_eglContext) != EGL_TRUE) {
		throw std::exception("Failed to make EGL context current");
	}

	RECT clientRc;
	GetClientRect(hWnd, &clientRc);
	p->m_width = clientRc.right - clientRc.left;
	p->m_height = clientRc.bottom - clientRc.top;

	static const char TEXTURED_QUAD_VERTEX_SHADER[] =
		"attribute vec2 a_pos;\n"
		"attribute vec2 a_tex;\n"
		"varying vec2 v_tex;\n"
		"uniform mat2 u_mat;\n"
		"uniform vec2 u_add;\n"
		"void main()\n"
		"{\n"
			"gl_Position = vec4((u_mat * a_pos) + u_add, 0, 1);\n"
			"v_tex = a_tex;\n"
		"}\n"
		;
	static const char TEXTURED_QUAD_FRAGMENT_SHADER[] =
		"precision mediump float;\n"
		"varying vec2 v_tex;\n"
		"uniform sampler2D u_sampler;\n"
		"void main()\n"
		"{\n"
			"gl_FragColor = texture2D(u_sampler, v_tex);\n"
		"}\n"
		;
	p->m_texturedQuadProgram.program = LoadGLSLProgram(TEXTURED_QUAD_VERTEX_SHADER,
		TEXTURED_QUAD_FRAGMENT_SHADER);
	p->m_texturedQuadProgram.aposLoc = glGetAttribLocation(
		p->m_texturedQuadProgram.program, "a_pos");
	p->m_texturedQuadProgram.atexLoc = glGetAttribLocation(
		p->m_texturedQuadProgram.program, "a_tex");
	p->m_texturedQuadProgram.umatLoc = glGetUniformLocation(
		p->m_texturedQuadProgram.program, "u_mat");
	p->m_texturedQuadProgram.uaddLoc = glGetUniformLocation(
		p->m_texturedQuadProgram.program, "u_add");
	p->m_texturedQuadProgram.usamplerLoc = glGetUniformLocation(
		p->m_texturedQuadProgram.program, "u_sampler");
	p->SetTexturedQuadMatrix(Matrix3x2f::IDENTITY);

	return p;
}

GLES2Texture* GLES2Manager::CreateTextureFromFile(const std::wstring& path)
{
	GLES2Texture* result = GLES2Texture::Create();

	ComPtr<IWICImagingFactory> wicFactory;
	CHECK_HR(CoCreateInstance(CLSID_WICImagingFactory, NULL,
		CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)wicFactory.Receive()));

	ComPtr<IWICBitmapDecoder> bmpDecoder;
	CHECK_HR(wicFactory->CreateDecoderFromFilename(path.c_str(), NULL,
		GENERIC_READ, WICDecodeMetadataCacheOnDemand, bmpDecoder.Receive()));

	ComPtr<IWICBitmapFrameDecode> frameDecode;
	CHECK_HR(bmpDecoder->GetFrame(0, frameDecode.Receive()));

	UINT width;
	UINT height;
	CHECK_HR(frameDecode->GetSize(&width, &height));

	WICPixelFormatGUID pixelFormat;
	CHECK_HR(frameDecode->GetPixelFormat(&pixelFormat));

	ComPtr<IWICFormatConverter> formatConverter;
	CHECK_HR(wicFactory->CreateFormatConverter(formatConverter.Receive()));

	CHECK_HR(formatConverter->Initialize(frameDecode, 
		GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeErrorDiffusion, NULL, 0.5,
		WICBitmapPaletteTypeMedianCut));

	std::vector<BYTE> data(4*width*height);
	CHECK_HR(formatConverter->CopyPixels(NULL, 4*width, 4*width*height, &data[0]));

	glBindTexture(GL_TEXTURE_2D, result->Get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, &data[0]);

	return result;
}

void GLES2Manager::SetTexturedQuadMatrix(const Matrix3x2f& mat)
{
	glUseProgram(m_texturedQuadProgram.program);

	// Column-major order is required
	float umat[4] = { mat._11, mat._21, mat._21, mat._22 };
	glUniformMatrix2fv(m_texturedQuadProgram.umatLoc, 1, GL_FALSE, umat);
	float uadd[2] = { mat._31, mat._32 };
	glUniform2fv(m_texturedQuadProgram.uaddLoc, 1, uadd);
}

void GLES2Manager::DrawTexturedQuad(const Rectf& rc)
{
	glUseProgram(m_texturedQuadProgram.program);

	GLfloat verts[] = { rc.left, rc.bottom, 0.0f, 1.0f, // Lower left
		rc.right, rc.bottom, 1.0f, 1.0f, // Lower right
		rc.left, rc.top, 0.0f, 0.0f, // Upper left
		rc.right, rc.top, 1.0f, 0.0f }; // Upper right

	glVertexAttribPointer(m_texturedQuadProgram.aposLoc, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), &verts[0]);
	glVertexAttribPointer(m_texturedQuadProgram.atexLoc, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), &verts[2]);

	glEnableVertexAttribArray(m_texturedQuadProgram.aposLoc);
	glEnableVertexAttribArray(m_texturedQuadProgram.atexLoc);

	glUniform1i(m_texturedQuadProgram.usamplerLoc, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLES2Manager::Present()
{
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void GLES2Manager::Resize()
{
	RECT clientRc;
	GetClientRect(m_hWnd, &clientRc);
	m_width = clientRc.right - clientRc.left;
	m_height = clientRc.bottom - clientRc.top;
}
