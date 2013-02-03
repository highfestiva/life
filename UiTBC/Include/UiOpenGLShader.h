
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiOpenGLRenderer.h"



namespace UiTbc
{



class UiOpenGLShader
{
public:
    UiOpenGLShader();
    ~UiOpenGLShader();

    bool Build(const str& pVertexShaderSource, const str& pFragmentShaderSource);
    unsigned GetProgram() const;

private:
    void Release();
    static unsigned Compile(GLenum type, const str& pFilename);
    bool Link();

    unsigned mFragmentShader;
    unsigned mVertexShader;
    unsigned mShaderProgram;

    LOG_CLASS_DECLARE();
};



}
