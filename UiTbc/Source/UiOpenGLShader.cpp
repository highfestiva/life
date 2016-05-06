
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#define GL_GLEXT_PROTOTYPES
#include "pch.h"
#include "../include/uiopenglshader.h"



namespace uitbc {



UiOpenGLShader::UiOpenGLShader():
	fragment_shader_(0),
	vertex_shader_(0),
	shader_program_(0) {
}

UiOpenGLShader::~UiOpenGLShader() {
	Release();
}

void UiOpenGLShader::Release() {
	if (vertex_shader_) {
		::glDeleteShader(vertex_shader_);
		vertex_shader_ = 0;
	}
	if (fragment_shader_) {
		::glDeleteShader(fragment_shader_);
		fragment_shader_ = 0;
	}
	if (shader_program_) {
		::glDeleteProgram(shader_program_);
		shader_program_ = 0;
	}
}

unsigned UiOpenGLShader::GetProgram() const {
	return shader_program_;
}

bool UiOpenGLShader::Build(const str& vertex_shader_source, const str& fragment_shader_source) {
	shader_program_ = ::glCreateProgram();

	vertex_shader_ = Compile(GL_VERTEX_SHADER, vertex_shader_source);
	fragment_shader_ = Compile(GL_FRAGMENT_SHADER, fragment_shader_source);
	if (vertex_shader_ == 0 || fragment_shader_ == 0) {
		log_.Error("Error while compiling shaders");
		Release();
		return false;
	}

	::glAttachShader(shader_program_, vertex_shader_);
	::glAttachShader(shader_program_, fragment_shader_);

	if (!Link()) {
		Release();
		return false;
	}

	return true;
}

unsigned UiOpenGLShader::Compile(GLenum type, const str& source) {
	if (source.empty()) {
		log_.Errorf("Shader source '%s' is empty!", source.c_str());
		return 0;
	}

	GLuint shader = ::glCreateShader(type);
	str ansi_source = source;
	const char* raw_source = ansi_source.c_str();
	::glShaderSource(shader, 1, &raw_source, 0);
	::glCompileShader(shader);
	GLint status;
	::glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		log_.Errorf("Failed to compile shader with contents: '%s'", source.c_str());
		GLint log_length;
		::glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		if (log_length > 0) {
			unsigned char* raw_log = new unsigned char[log_length];
			::glGetShaderInfoLog(shader, log_length, &log_length, (GLchar*)&raw_log[0]);
			str char_log((char*)raw_log);
			str log = char_log;
			log_.Error(log);
			delete raw_log;
		}
		return 0;
	}
	return shader;
}

bool UiOpenGLShader::Link() {
	::glLinkProgram(shader_program_);
	GLint status;
	::glGetProgramiv(shader_program_, GL_LINK_STATUS, &status);
	if (!status) {
		log_.Error("Failed to link shader program");
	}
	return status? true : false;
}



loginstance(kUiGfx3D, UiOpenGLShader);



}
