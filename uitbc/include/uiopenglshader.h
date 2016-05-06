
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uiopenglrenderer.h"



namespace uitbc {



class UiOpenGLShader {
public:
    UiOpenGLShader();
    ~UiOpenGLShader();

    bool Build(const str& vertex_shader_source, const str& fragment_shader_source);
    unsigned GetProgram() const;

private:
    void Release();
    static unsigned Compile(GLenum type, const str& filename);
    bool Link();

    unsigned fragment_shader_;
    unsigned vertex_shader_;
    unsigned shader_program_;

	logclass();
};



}
