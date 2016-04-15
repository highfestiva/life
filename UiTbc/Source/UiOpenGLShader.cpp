
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#define GL_GLEXT_PROTOTYPES
#include "pch.h"
#include "../Include/UiOpenGLShader.h"



namespace UiTbc
{



UiOpenGLShader::UiOpenGLShader():
	mFragmentShader(0),
	mVertexShader(0),
	mShaderProgram(0)
{
}

UiOpenGLShader::~UiOpenGLShader()
{
	Release();
}

void UiOpenGLShader::Release()
{
	if (mVertexShader)
	{
		::glDeleteShader(mVertexShader);
		mVertexShader = 0;
	}
	if (mFragmentShader)
	{
		::glDeleteShader(mFragmentShader);
		mFragmentShader = 0;
	}
	if (mShaderProgram)
	{
		::glDeleteProgram(mShaderProgram);
		mShaderProgram = 0;
	}
}

unsigned UiOpenGLShader::GetProgram() const
{
	return mShaderProgram;
}

bool UiOpenGLShader::Build(const str& pVertexShaderSource, const str& pFragmentShaderSource)
{
	mShaderProgram = ::glCreateProgram();

	mVertexShader = Compile(GL_VERTEX_SHADER, pVertexShaderSource);
	mFragmentShader = Compile(GL_FRAGMENT_SHADER, pFragmentShaderSource);
	if (mVertexShader == 0 || mFragmentShader == 0)
	{
		mLog.Error("Error while compiling shaders");
		Release();
		return false;
	}

	::glAttachShader(mShaderProgram, mVertexShader);
	::glAttachShader(mShaderProgram, mFragmentShader);

	if (!Link())
	{
		Release();
		return false;
	}

	return true;
}

unsigned UiOpenGLShader::Compile(GLenum type, const str& pSource)
{
	if (pSource.empty())
	{
		mLog.Errorf("Shader source '%s' is empty!", pSource.c_str());
		return 0;
	}

	GLuint lShader = ::glCreateShader(type);
	str lAnsiSource = pSource;
	const char* lRawSource = lAnsiSource.c_str();
	::glShaderSource(lShader, 1, &lRawSource, 0);
	::glCompileShader(lShader);
	GLint lStatus;
	::glGetShaderiv(lShader, GL_COMPILE_STATUS, &lStatus);
	if (!lStatus)
	{
		mLog.Errorf("Failed to compile shader with contents: '%s'", pSource.c_str());
		GLint lLogLength;
		::glGetShaderiv(lShader, GL_INFO_LOG_LENGTH, &lLogLength);
		if (lLogLength > 0)
		{
			unsigned char* lRawLog = new unsigned char[lLogLength];
			::glGetShaderInfoLog(lShader, lLogLength, &lLogLength, (GLchar*)&lRawLog[0]);
			str lCharLog((char*)lRawLog);
			str lLog = lCharLog;
			mLog.Error(lLog);
			delete lRawLog;
		}
		return 0;
	}
	return lShader;
}

bool UiOpenGLShader::Link()
{
	::glLinkProgram(mShaderProgram);
	GLint lStatus;
	::glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &lStatus);
	if (!lStatus)
	{
		mLog.Error("Failed to link shader program");
	}
	return lStatus? true : false;
}



loginstance(UI_GFX_3D, UiOpenGLShader);



}
