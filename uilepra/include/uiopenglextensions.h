
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

// Unreachable code warning below (MSVC8). For some reason just this file happens to temper with some shitty template.
#include "../../lepra/include/lepratarget.h"
#ifdef LEPRA_MSVC
#pragma warning(push)
#pragma warning(disable: 4702)
#endif // LEPRA_MSVC
#include "uilepra.h"
#ifdef LEPRA_MSVC
#pragma warning(pop)
#endif // LEPRA_MSVC
#include "../../lepra/include/lepraos.h"


#if defined(LEPRA_WINDOWS)

#define LEPRA_GL_INDEX_TYPE GL_UNSIGNED_INT
#include <gl/gl.h>
#include <gl/glu.h>
#include "win32/WGLEXT.h"
#include "glext.h"

#elif defined(LEPRA_IOS)

#define LEPRA_GL_ES
#define LEPRA_GL_ES_1
#define LEPRA_GL_INDEX_TYPE GL_UNSIGNED_SHORT
#ifdef __OBJC__
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <sys/time.h>
#include "iosglext.h"
#endif // Objective-C

#elif defined(LEPRA_MAC)

#define LEPRA_GL_INDEX_TYPE GL_UNSIGNED_INT
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <sys/time.h>
#include "macglext.h"

#elif defined(LEPRA_POSIX)

#define LEPRA_GL_INDEX_TYPE GL_UNSIGNED_INT
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glxext.h>
#include "linuxglext.h"

#else // Unkonwn platform.

#error "Bad GL platform!"

#endif // Win / Mac / Posix



namespace uilepra {



class OpenGLExtensions {
public:

	static bool IsExtensionSupported(const char* extension);
#if !defined(LEPRA_MAC)
	static void* GetExtensionPointer(const char* function_name);
#endif // !Mac

	// OpenGL extension function pointers...
	// TODO: Implement more.

	// Frame buffer objects.
	static PFNGLISRENDERBUFFEREXTPROC                      glIsRenderbufferEXT;
	static PFNGLBINDRENDERBUFFEREXTPROC                    glBindRenderbufferEXT;
	static PFNGLDELETERENDERBUFFERSEXTPROC                 glDeleteRenderbuffersEXT;
	static PFNGLGENRENDERBUFFERSEXTPROC                    glGenRenderbuffersEXT;
	static PFNGLRENDERBUFFERSTORAGEEXTPROC                 glRenderbufferStorageEXT;
	static PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC          glGetRenderbufferParameterivEXT;
	static PFNGLISFRAMEBUFFEREXTPROC                       glIsFramebufferEXT;
	static PFNGLBINDFRAMEBUFFEREXTPROC                     glBindFramebufferEXT;
	static PFNGLDELETEFRAMEBUFFERSEXTPROC                  glDeleteFramebuffersEXT;
	static PFNGLGENFRAMEBUFFERSEXTPROC                     glGenFramebuffersEXT;
	static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC              glCheckFramebufferStatusEXT;
	static PFNGLFRAMEBUFFERTEXTURE1DEXTPROC                glFramebufferTexture1DEXT;
	static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                glFramebufferTexture2DEXT;
	static PFNGLFRAMEBUFFERTEXTURE3DEXTPROC                glFramebufferTexture3DEXT;
	static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC             glFramebufferRenderbufferEXT;
	static PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
	static PFNGLGENERATEMIPMAPEXTPROC                      glGenerateMipmapEXT;

	// Buffer objects.
	static PFNGLBINDBUFFERPROC    glBindBuffer;
	static PFNGLBUFFERDATAPROC    glBufferData;
	static PFNGLBUFFERSUBDATAPROC glBufferSubData;
	static PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	static PFNGLGENBUFFERSPROC    glGenBuffers;
	static PFNGLMAPBUFFERPROC     glMapBuffer;
	static PFNGLUNMAPBUFFERPROC   glUnmapBuffer;

	// Multi texture.
	static PFNGLACTIVETEXTUREPROC       glActiveTexture;
	static PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
#ifndef LEPRA_GL_ES_1
	static PFNGLMULTITEXCOORD1DPROC     glMultiTexCoord1d;
	static PFNGLMULTITEXCOORD1DVPROC    glMultiTexCoord1dv;
	static PFNGLMULTITEXCOORD1FPROC     glMultiTexCoord1f;
	static PFNGLMULTITEXCOORD1FVPROC    glMultiTexCoord1fv;
	static PFNGLMULTITEXCOORD1IPROC     glMultiTexCoord1i;
	static PFNGLMULTITEXCOORD1IVPROC    glMultiTexCoord1iv;
	static PFNGLMULTITEXCOORD1SPROC     glMultiTexCoord1s;
	static PFNGLMULTITEXCOORD1SVPROC    glMultiTexCoord1sv;
	static PFNGLMULTITEXCOORD2DPROC     glMultiTexCoord2d;
	static PFNGLMULTITEXCOORD2DVPROC    glMultiTexCoord2dv;
	static PFNGLMULTITEXCOORD2FPROC     glMultiTexCoord2f;
	static PFNGLMULTITEXCOORD2FVPROC    glMultiTexCoord2fv;
	static PFNGLMULTITEXCOORD2IPROC     glMultiTexCoord2i;
	static PFNGLMULTITEXCOORD2IVPROC    glMultiTexCoord2iv;
	static PFNGLMULTITEXCOORD2SPROC     glMultiTexCoord2s;
	static PFNGLMULTITEXCOORD2SVPROC    glMultiTexCoord2sv;
	static PFNGLMULTITEXCOORD3DPROC     glMultiTexCoord3d;
	static PFNGLMULTITEXCOORD3DVPROC    glMultiTexCoord3dv;
	static PFNGLMULTITEXCOORD3FPROC     glMultiTexCoord3f;
	static PFNGLMULTITEXCOORD3FVPROC    glMultiTexCoord3fv;
	static PFNGLMULTITEXCOORD3IPROC     glMultiTexCoord3i;
	static PFNGLMULTITEXCOORD3IVPROC    glMultiTexCoord3iv;
	static PFNGLMULTITEXCOORD3SPROC     glMultiTexCoord3s;
	static PFNGLMULTITEXCOORD3SVPROC    glMultiTexCoord3sv;
	static PFNGLMULTITEXCOORD4DPROC     glMultiTexCoord4d;
	static PFNGLMULTITEXCOORD4DVPROC    glMultiTexCoord4dv;
	static PFNGLMULTITEXCOORD4FPROC     glMultiTexCoord4f;
	static PFNGLMULTITEXCOORD4FVPROC    glMultiTexCoord4fv;
	static PFNGLMULTITEXCOORD4IPROC     glMultiTexCoord4i;
	static PFNGLMULTITEXCOORD4IVPROC    glMultiTexCoord4iv;
	static PFNGLMULTITEXCOORD4SPROC     glMultiTexCoord4s;
	static PFNGLMULTITEXCOORD4SVPROC    glMultiTexCoord4sv;

	// Shader programs (using the old style assebly language shaders).
	static PFNGLGENPROGRAMSARBPROC                glGenProgramsARB;
	static PFNGLBINDPROGRAMARBPROC                glBindProgramARB;
	static PFNGLDELETEPROGRAMSARBPROC             glDeleteProgramsARB;
	static PFNGLISPROGRAMARBPROC                  glIsProgramARB;

	static PFNGLPROGRAMSTRINGARBPROC              glProgramStringARB;
	static PFNGLGETPROGRAMIVARBPROC               glGetProgramStringARB;

	static PFNGLENABLEVERTEXATTRIBARRAYARBPROC    glEnableVertexAttribArrayARB;
	static PFNGLDISABLEVERTEXATTRIBARRAYARBPROC   glDisableVertexAttribArrayARB;

	static PFNGLGETPROGRAMENVPARAMETERDVARBPROC   glGetProgramEnvParameterdvARB;
	static PFNGLGETPROGRAMENVPARAMETERFVARBPROC   glGetProgramEnvParameterfvARB;
	static PFNGLPROGRAMENVPARAMETER4DARBPROC      glProgramEnvParameter4dARB;
	static PFNGLPROGRAMENVPARAMETER4DVARBPROC     glProgramEnvParameter4dvARB;
	static PFNGLPROGRAMENVPARAMETER4FARBPROC      glProgramEnvParameter4fARB;
	static PFNGLPROGRAMENVPARAMETER4FVARBPROC     glProgramEnvParameter4fvARB;

	static PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB;
	static PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB;
	static PFNGLPROGRAMLOCALPARAMETER4DARBPROC    glProgramLocalParameter4dARB;
	static PFNGLPROGRAMLOCALPARAMETER4DVARBPROC   glProgramLocalParameter4dvARB;
	static PFNGLPROGRAMLOCALPARAMETER4FARBPROC    glProgramLocalParameter4fARB;
	static PFNGLPROGRAMLOCALPARAMETER4FVARBPROC   glProgramLocalParameter4fvARB;

	static PFNGLGETPROGRAMIVARBPROC               glGetProgramivARB;
	static PFNGLGETVERTEXATTRIBDVARBPROC          glGetVertexAttribdvARB;
	static PFNGLGETVERTEXATTRIBFVARBPROC          glGetVertexAttribfvARB;
	static PFNGLGETVERTEXATTRIBIVARBPROC          glGetVertexAttribivARB;

	static PFNGLGETVERTEXATTRIBPOINTERVARBPROC    glGetVertexAttribPointervARB;
	static PFNGLVERTEXATTRIBPOINTERARBPROC        glVertexAttribPointerARB;

	static PFNGLVERTEXATTRIB1DARBPROC             glVertexAttrib1dARB;
	static PFNGLVERTEXATTRIB1DVARBPROC            glVertexAttrib1dvARB;
	static PFNGLVERTEXATTRIB1FARBPROC             glVertexAttrib1fARB;
	static PFNGLVERTEXATTRIB1FVARBPROC            glVertexAttrib1fvARB;
	static PFNGLVERTEXATTRIB1SARBPROC             glVertexAttrib1sARB;
	static PFNGLVERTEXATTRIB1SVARBPROC            glVertexAttrib1svARB;
	static PFNGLVERTEXATTRIB2DARBPROC             glVertexAttrib2dARB;
	static PFNGLVERTEXATTRIB2DVARBPROC            glVertexAttrib2dvARB;
	static PFNGLVERTEXATTRIB2FARBPROC             glVertexAttrib2fARB;
	static PFNGLVERTEXATTRIB2FVARBPROC            glVertexAttrib2fvARB;
	static PFNGLVERTEXATTRIB2SARBPROC             glVertexAttrib2sARB;
	static PFNGLVERTEXATTRIB2SVARBPROC            glVertexAttrib2svARB;
	static PFNGLVERTEXATTRIB3DARBPROC             glVertexAttrib3dARB;
	static PFNGLVERTEXATTRIB3DVARBPROC            glVertexAttrib3dvARB;
	static PFNGLVERTEXATTRIB3FARBPROC             glVertexAttrib3fARB;
	static PFNGLVERTEXATTRIB3FVARBPROC            glVertexAttrib3fvARB;
	static PFNGLVERTEXATTRIB3SARBPROC             glVertexAttrib3sARB;
	static PFNGLVERTEXATTRIB3SVARBPROC            glVertexAttrib3svARB;
	static PFNGLVERTEXATTRIB4NBVARBPROC           glVertexAttrib4NbvARB;
	static PFNGLVERTEXATTRIB4NIVARBPROC           glVertexAttrib4NivARB;
	static PFNGLVERTEXATTRIB4NSVARBPROC           glVertexAttrib4NsvARB;
	static PFNGLVERTEXATTRIB4NUBARBPROC           glVertexAttrib4NubARB;
	static PFNGLVERTEXATTRIB4NUBVARBPROC          glVertexAttrib4NubvARB;
	static PFNGLVERTEXATTRIB4NUIVARBPROC          glVertexAttrib4NuivARB;
	static PFNGLVERTEXATTRIB4NUSVARBPROC          glVertexAttrib4NusvARB;
	static PFNGLVERTEXATTRIB4BVARBPROC            glVertexAttrib4bvARB;
	static PFNGLVERTEXATTRIB4DARBPROC             glVertexAttrib4dARB;
	static PFNGLVERTEXATTRIB4DVARBPROC            glVertexAttrib4dvARB;
	static PFNGLVERTEXATTRIB4FARBPROC             glVertexAttrib4fARB;
	static PFNGLVERTEXATTRIB4FVARBPROC            glVertexAttrib4fvARB;
	static PFNGLVERTEXATTRIB4IVARBPROC            glVertexAttrib4ivARB;
	static PFNGLVERTEXATTRIB4SARBPROC             glVertexAttrib4sARB;
	static PFNGLVERTEXATTRIB4SVARBPROC            glVertexAttrib4svARB;
	static PFNGLVERTEXATTRIB4UBVARBPROC           glVertexAttrib4ubvARB;
	static PFNGLVERTEXATTRIB4UIVARBPROC           glVertexAttrib4uivARB;
	static PFNGLVERTEXATTRIB4USVARBPROC           glVertexAttrib4usvARB;

	// Shaders (using GLSL - the real slim shader).
	static PFNGLDELETEOBJECTARBPROC               glDeleteObjectARB;
	static PFNGLGETHANDLEARBPROC                  glGetHandleARB;
	static PFNGLDETACHOBJECTARBPROC               glDetachObjectARB;
	static PFNGLCREATESHADEROBJECTARBPROC         glCreateShaderObjectARB;
	static PFNGLSHADERSOURCEARBPROC               glShaderSourceARB;
	static PFNGLCOMPILESHADERARBPROC              glCompileShaderARB;
	static PFNGLCREATEPROGRAMOBJECTARBPROC        glCreateProgramObjectARB;
	static PFNGLATTACHOBJECTARBPROC               glAttachObjectARB;
	static PFNGLLINKPROGRAMARBPROC                glLinkProgramARB;
	static PFNGLUSEPROGRAMOBJECTARBPROC           glUseProgramObjectARB;
	static PFNGLVALIDATEPROGRAMARBPROC            glValidateProgramARB;
	static PFNGLUNIFORM1FARBPROC                  glUniform1fARB;
	static PFNGLUNIFORM2FARBPROC                  glUniform2fARB;
	static PFNGLUNIFORM3FARBPROC                  glUniform3fARB;
	static PFNGLUNIFORM4FARBPROC                  glUniform4fARB;
	static PFNGLUNIFORM1IARBPROC                  glUniform1iARB;
	static PFNGLUNIFORM2IARBPROC                  glUniform2iARB;
	static PFNGLUNIFORM3IARBPROC                  glUniform3iARB;
	static PFNGLUNIFORM4IARBPROC                  glUniform4iARB;
	static PFNGLUNIFORM1FVARBPROC                 glUniform1fvARB;
	static PFNGLUNIFORM2FVARBPROC                 glUniform2fvARB;
	static PFNGLUNIFORM3FVARBPROC                 glUniform3fvARB;
	static PFNGLUNIFORM4FVARBPROC                 glUniform4fvARB;
	static PFNGLUNIFORM1IVARBPROC                 glUniform1ivARB;
	static PFNGLUNIFORM2IVARBPROC                 glUniform2ivARB;
	static PFNGLUNIFORM3IVARBPROC                 glUniform3ivARB;
	static PFNGLUNIFORM4IVARBPROC                 glUniform4ivARB;
	static PFNGLUNIFORMMATRIX2FVARBPROC           glUniformMatrix2fvARB;
	static PFNGLUNIFORMMATRIX3FVARBPROC           glUniformMatrix3fvARB;
	static PFNGLUNIFORMMATRIX4FVARBPROC           glUniformMatrix4fvARB;
	static PFNGLGETOBJECTPARAMETERFVARBPROC       glGetObjectParameterfvARB;
	static PFNGLGETOBJECTPARAMETERIVARBPROC       glGetObjectParameterivARB;
	static PFNGLGETINFOLOGARBPROC                 glGetInfoLogARB;
	static PFNGLGETATTACHEDOBJECTSARBPROC         glGetAttachedObjectsARB;
	static PFNGLGETUNIFORMLOCATIONARBPROC         glGetUniformLocationARB;
	static PFNGLGETACTIVEUNIFORMARBPROC           glGetActiveUniformARB;
	static PFNGLGETUNIFORMFVARBPROC               glGetUniformfvARB;
	static PFNGLGETUNIFORMIVARBPROC               glGetUniformivARB;
	static PFNGLGETSHADERSOURCEARBPROC            glGetShaderSourceARB;
#endif // !ES1

	static void InitExtensions();

	static bool IsFrameBufferObjectsSupported();
	static bool IsBufferObjectsSupported();	// Returns true if the OpenGL version is at least 1.5.
	static bool IsAnisotropicFilteringSupported();
	static bool IsCompressedTexturesSupported();
	static bool IsMultiTextureSupported();
	static bool IsShadowMapsSupported();	// Returns true if the OpenGL version is at least 1.4.
	static bool IsShaderAsmProgramsSupported();
	static bool IsShaderCProgramsSupported();

	static float GetMaxAnisotropy();
	static void SetAnisotropy(float amount_anisotropy);

	static bool IsVSyncEnabled();
	static bool SetVSyncEnabled(bool enabled);

protected:
private:
	// Sets the functions pointers to null for the corresponding extension.
	static void ClearFrameBufferObjectFunctions();
	static void ClearBufferObjectFunctions();
	static void ClearMultiTextureFunctions();
	static void ClearShaderProgramFunctions();
	static void ClearShaderFunctions();

	static bool CheckFrameBufferObjectFunctions();
	static bool CheckBufferObjectFunctions();
	static bool CheckMultiTextureFunctions();
	static bool CheckShaderProgramFunctions();
	static bool CheckShaderFunctions();

	static bool is_gl_version14_;
	static bool is_gl_version15_;
	static bool is_frame_buffer_objects_supported_;
	static bool is_buffer_objects_supported_;
	static bool is_anisotropic_filtering_supported_;
	static bool is_compressed_textures_supported_;
	static bool is_multi_texture_supported_;
	static bool is_shader_asm_programs_supported_;
	static bool is_shader_c_programs_supported_;

	static float max_anisotropy_;

#ifdef LEPRA_WINDOWS
	// Vsync on/off.
	static PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT;
	static PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
#endif
};



}
