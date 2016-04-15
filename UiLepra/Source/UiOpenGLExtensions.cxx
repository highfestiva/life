
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../Include/UiOpenGLExtensions.h"



namespace UiLepra
{



bool OpenGLExtensions::IsExtensionSupported(const char* pExtension)
{
	str lExtension(pExtension);
	strutil::StripAllWhiteSpaces(lExtension);

	if (lExtension.length() == 0)
		return false;

	str lGLExtensions((char*)glGetString(GL_EXTENSIONS));
	return (lGLExtensions.find(lExtension.c_str(), 0) != str::npos);
}

#if defined(LEPRA_MAC)
#define GET_EXTENSION_POINTER(f) (&::f)
#else
#define GET_EXTENSION_POINTER(f) GetExtensionPointer(#f)
void* OpenGLExtensions::GetExtensionPointer(const char* pFunctionName)
{
#if defined(LEPRA_WINDOWS)
	return (void*)wglGetProcAddress(pFunctionName);
#elif defined(LEPRA_POSIX)
	return ((void*)glXGetProcAddress((const GLubyte*)pFunctionName));
#else // Unkonwn platform
#error "WTF! Waco platform!"
#endif // Win / Posix
}
#endif // Mac / !Mac

void OpenGLExtensions::InitExtensions()
{
	// Check OpenGL version.
	const GLubyte* lVersion = glGetString(GL_VERSION);
	if (lVersion[0] == '1')
	{
		if (lVersion[1] == '.' && 
		   lVersion[2] >= '5' && 
		   lVersion[2] <= '9')
		{
			mIsGLVersion14 = true;
			mIsGLVersion15 = true;
		}
		else if(lVersion[1] == '.' && 
			lVersion[2] >= '4' && 
			lVersion[2] <= '9')
		{
			mIsGLVersion14 = true;
		}
	}
	else if (lVersion[0] >= '2' && lVersion[0] <= '9')
	{
		// This must be an OpenGL version way higher than the versions
		// that existed when this was written. Assume backward compatibility.
		mIsGLVersion14 = true;
		mIsGLVersion15 = true;
	}
	else if (lVersion[0] < '0' || lVersion[0] > '9')
	{
		// NaN hopefully means some extremely well developed system.
		mIsGLVersion14 = true;
		mIsGLVersion15 = true;
	}


#ifndef LEPRA_GL_ES_1
	// Init Frame Buffer Objects...
	if (IsExtensionSupported("GL_EXT_framebuffer_object"))
	{
		mIsFrameBufferObjectsSupported = true;
	}

	if (mIsFrameBufferObjectsSupported)
	{
		glIsRenderbufferEXT                      = (PFNGLISRENDERBUFFEREXTPROC)                      GET_EXTENSION_POINTER(glIsRenderbufferEXT);
		glBindRenderbufferEXT                    = (PFNGLBINDRENDERBUFFEREXTPROC)                    GET_EXTENSION_POINTER(glBindRenderbufferEXT);
		glDeleteRenderbuffersEXT                 = (PFNGLDELETERENDERBUFFERSEXTPROC)                 GET_EXTENSION_POINTER(glDeleteRenderbuffersEXT);
		glGenRenderbuffersEXT                    = (PFNGLGENRENDERBUFFERSEXTPROC)                    GET_EXTENSION_POINTER(glGenRenderbuffersEXT);
		glRenderbufferStorageEXT                 = (PFNGLRENDERBUFFERSTORAGEEXTPROC)                 GET_EXTENSION_POINTER(glRenderbufferStorageEXT);
		glGetRenderbufferParameterivEXT          = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)          GET_EXTENSION_POINTER(glGetRenderbufferParameterivEXT);
		glIsFramebufferEXT                       = (PFNGLISFRAMEBUFFEREXTPROC)                       GET_EXTENSION_POINTER(glIsFramebufferEXT);
		glBindFramebufferEXT                     = (PFNGLBINDFRAMEBUFFEREXTPROC)                     GET_EXTENSION_POINTER(glBindFramebufferEXT);
		glDeleteFramebuffersEXT                  = (PFNGLDELETEFRAMEBUFFERSEXTPROC)                  GET_EXTENSION_POINTER(glDeleteFramebuffersEXT);
		glGenFramebuffersEXT                     = (PFNGLGENFRAMEBUFFERSEXTPROC)                     GET_EXTENSION_POINTER(glGenFramebuffersEXT);
		glCheckFramebufferStatusEXT              = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)              GET_EXTENSION_POINTER(glCheckFramebufferStatusEXT);
		glFramebufferTexture1DEXT                = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)                GET_EXTENSION_POINTER(glFramebufferTexture1DEXT);
		glFramebufferTexture2DEXT                = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)                GET_EXTENSION_POINTER(glFramebufferTexture2DEXT);
		glFramebufferTexture3DEXT                = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)                GET_EXTENSION_POINTER(glFramebufferTexture3DEXT);
		glFramebufferRenderbufferEXT             = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)             GET_EXTENSION_POINTER(glFramebufferRenderbufferEXT);
		glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) GET_EXTENSION_POINTER(glGetFramebufferAttachmentParameterivEXT);
		glGenerateMipmapEXT                      = (PFNGLGENERATEMIPMAPEXTPROC)                      GET_EXTENSION_POINTER(glGenerateMipmapEXT);

		if (!CheckFrameBufferObjectFunctions())
		{
			ClearFrameBufferObjectFunctions();
			mIsFrameBufferObjectsSupported = false;
		}
	}
#endif // !ES1

	// Init buffer objects.
	if (mIsGLVersion15 || IsExtensionSupported("GL_ARB_vertex_buffer_object"))
	{
		mIsBufferObjectsSupported = true;
	}

	if (mIsGLVersion15)
	{
		glBindBuffer    = (PFNGLBINDBUFFERPROC)   GET_EXTENSION_POINTER(glBindBuffer);
		glBufferData    = (PFNGLBUFFERDATAPROC)   GET_EXTENSION_POINTER(glBufferData);
		glBufferSubData = (PFNGLBUFFERSUBDATAPROC)GET_EXTENSION_POINTER(glBufferSubData);
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GET_EXTENSION_POINTER(glDeleteBuffers);
		glGenBuffers    = (PFNGLGENBUFFERSPROC)   GET_EXTENSION_POINTER(glGenBuffers);
#ifdef LEPRA_GL_ES_1
		glMapBuffer     = (PFNGLMAPBUFFERPROC)    GET_EXTENSION_POINTER(glMapBufferOES);
		glUnmapBuffer   = (PFNGLUNMAPBUFFERPROC)  GET_EXTENSION_POINTER(glUnmapBufferOES);
#else // !GLES1
		glMapBuffer     = (PFNGLMAPBUFFERPROC)    GET_EXTENSION_POINTER(glMapBuffer);
		glUnmapBuffer   = (PFNGLUNMAPBUFFERPROC)  GET_EXTENSION_POINTER(glUnmapBuffer);
#endif // GLES1/!GLES1
	}

#ifndef LEPRA_GL_ES_1
	if(mIsBufferObjectsSupported && !CheckBufferObjectFunctions())
	{
		// Retry with the ARB version...
		glBindBuffer    = (PFNGLBINDBUFFERPROC)   GET_EXTENSION_POINTER(glBindBufferARB);
		glBufferData    = (PFNGLBUFFERDATAPROC)   GET_EXTENSION_POINTER(glBufferDataARB);
		glBufferSubData = (PFNGLBUFFERSUBDATAPROC)GET_EXTENSION_POINTER(glBufferSubDataARB);
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GET_EXTENSION_POINTER(glDeleteBuffersARB);
		glGenBuffers    = (PFNGLGENBUFFERSPROC)   GET_EXTENSION_POINTER(glGenBuffersARB);
		glMapBuffer     = (PFNGLMAPBUFFERPROC)    GET_EXTENSION_POINTER(glMapBufferARB);
		glUnmapBuffer   = (PFNGLUNMAPBUFFERPROC)  GET_EXTENSION_POINTER(glUnmapBufferARB);
	}
#endif // !ES1

	if (mIsBufferObjectsSupported)
	{
		if (!CheckBufferObjectFunctions())
		{
			ClearBufferObjectFunctions();
			mIsBufferObjectsSupported = false;
		}
	}

#ifndef LEPRA_GL_ES_1
	// Init anisotropic filtering...
	if (IsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
	{
		mIsAnisotropicFilteringSupported = true;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &mMaxAnisotropy);
	}

	// Init texture compression.
	if (IsExtensionSupported("GL_ARB_texture_compression"))
	{
		mIsCompressedTexturesSupported = true;
		glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
	}
#endif // !ES1

#ifdef LEPRA_WINDOWS
	// Init vsync on/off.
	if (IsExtensionSupported("WGL_EXT_swap"))
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)GET_EXTENSION_POINTER(wglSwapIntervalEXT);
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)GET_EXTENSION_POINTER(wglGetSwapIntervalEXT);
	}
#else
#pragma message("Warning: In OpenGLExtensions::InitExtensions(), VSync is not initialized.")
#endif

#ifndef LEPRA_GL_ES_1
	// Init multi texture support.
	if (IsExtensionSupported("GL_ARB_multitexture"))
	{
		mIsMultiTextureSupported = true;

		glActiveTexture       = (PFNGLACTIVETEXTUREPROC)       GET_EXTENSION_POINTER(glActiveTextureARB);
		glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC) GET_EXTENSION_POINTER(glClientActiveTexture);
		glMultiTexCoord1d     = (PFNGLMULTITEXCOORD1DPROC)     GET_EXTENSION_POINTER(glMultiTexCoord1dARB);
		glMultiTexCoord1dv    = (PFNGLMULTITEXCOORD1DVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord1dvARB);
		glMultiTexCoord1f     = (PFNGLMULTITEXCOORD1FPROC)     GET_EXTENSION_POINTER(glMultiTexCoord1fARB);
		glMultiTexCoord1fv    = (PFNGLMULTITEXCOORD1FVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord1fvARB);
		glMultiTexCoord1i     = (PFNGLMULTITEXCOORD1IPROC)     GET_EXTENSION_POINTER(glMultiTexCoord1iARB);
		glMultiTexCoord1iv    = (PFNGLMULTITEXCOORD1IVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord1ivARB);
		glMultiTexCoord1s     = (PFNGLMULTITEXCOORD1SPROC)     GET_EXTENSION_POINTER(glMultiTexCoord1sARB);
		glMultiTexCoord1sv    = (PFNGLMULTITEXCOORD1SVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord1svARB);
		glMultiTexCoord2d     = (PFNGLMULTITEXCOORD2DPROC)     GET_EXTENSION_POINTER(glMultiTexCoord2dARB);
		glMultiTexCoord2dv    = (PFNGLMULTITEXCOORD2DVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord2dvARB);
		glMultiTexCoord2f     = (PFNGLMULTITEXCOORD2FPROC)     GET_EXTENSION_POINTER(glMultiTexCoord2fARB);
		glMultiTexCoord2fv    = (PFNGLMULTITEXCOORD2FVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord2fvARB);
		glMultiTexCoord2i     = (PFNGLMULTITEXCOORD2IPROC)     GET_EXTENSION_POINTER(glMultiTexCoord2iARB);
		glMultiTexCoord2iv    = (PFNGLMULTITEXCOORD2IVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord2ivARB);
		glMultiTexCoord2s     = (PFNGLMULTITEXCOORD2SPROC)     GET_EXTENSION_POINTER(glMultiTexCoord2sARB);
		glMultiTexCoord2sv    = (PFNGLMULTITEXCOORD2SVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord2svARB);
		glMultiTexCoord3d     = (PFNGLMULTITEXCOORD3DPROC)     GET_EXTENSION_POINTER(glMultiTexCoord3dARB);
		glMultiTexCoord3dv    = (PFNGLMULTITEXCOORD3DVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord3dvARB);
		glMultiTexCoord3f     = (PFNGLMULTITEXCOORD3FPROC)     GET_EXTENSION_POINTER(glMultiTexCoord3fARB);
		glMultiTexCoord3fv    = (PFNGLMULTITEXCOORD3FVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord3fvARB);
		glMultiTexCoord3i     = (PFNGLMULTITEXCOORD3IPROC)     GET_EXTENSION_POINTER(glMultiTexCoord3iARB);
		glMultiTexCoord3iv    = (PFNGLMULTITEXCOORD3IVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord3ivARB);
		glMultiTexCoord3s     = (PFNGLMULTITEXCOORD3SPROC)     GET_EXTENSION_POINTER(glMultiTexCoord3sARB);
		glMultiTexCoord3sv    = (PFNGLMULTITEXCOORD3SVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord3svARB);
		glMultiTexCoord4d     = (PFNGLMULTITEXCOORD4DPROC)     GET_EXTENSION_POINTER(glMultiTexCoord4dARB);
		glMultiTexCoord4dv    = (PFNGLMULTITEXCOORD4DVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord4dvARB);
		glMultiTexCoord4f     = (PFNGLMULTITEXCOORD4FPROC)     GET_EXTENSION_POINTER(glMultiTexCoord4fARB);
		glMultiTexCoord4fv    = (PFNGLMULTITEXCOORD4FVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord4fvARB);
		glMultiTexCoord4i     = (PFNGLMULTITEXCOORD4IPROC)     GET_EXTENSION_POINTER(glMultiTexCoord4iARB);
		glMultiTexCoord4iv    = (PFNGLMULTITEXCOORD4IVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord4ivARB);
		glMultiTexCoord4s     = (PFNGLMULTITEXCOORD4SPROC)     GET_EXTENSION_POINTER(glMultiTexCoord4sARB);
		glMultiTexCoord4sv    = (PFNGLMULTITEXCOORD4SVPROC)    GET_EXTENSION_POINTER(glMultiTexCoord4svARB);

		if (!CheckMultiTextureFunctions())
		{
			mIsMultiTextureSupported = false;
			ClearMultiTextureFunctions();
		}
	}

	// Init vertex and pixel shader support.
	if (IsExtensionSupported("GL_ARB_vertex_program") &&
	   IsExtensionSupported("GL_ARB_fragment_program"))
	{
		mIsShaderAsmProgramsSupported = true;

		glGenProgramsARB                = (PFNGLGENPROGRAMSARBPROC)                GET_EXTENSION_POINTER(glGenProgramsARB);
		glBindProgramARB                = (PFNGLBINDPROGRAMARBPROC)                GET_EXTENSION_POINTER(glBindProgramARB);
		glDeleteProgramsARB             = (PFNGLDELETEPROGRAMSARBPROC)             GET_EXTENSION_POINTER(glDeleteProgramsARB);
		glIsProgramARB                  = (PFNGLISPROGRAMARBPROC)                  GET_EXTENSION_POINTER(glIsProgramARB);
		glProgramStringARB              = (PFNGLPROGRAMSTRINGARBPROC)              GET_EXTENSION_POINTER(glProgramStringARB);
		glGetProgramStringARB           = (PFNGLGETPROGRAMIVARBPROC)               GET_EXTENSION_POINTER(glGetProgramStringARB);
		glEnableVertexAttribArrayARB    = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)    GET_EXTENSION_POINTER(glEnableVertexAttribArrayARB);
		glDisableVertexAttribArrayARB   = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)   GET_EXTENSION_POINTER(glDisableVertexAttribArrayARB);
		glGetProgramEnvParameterdvARB   = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)   GET_EXTENSION_POINTER(glGetProgramEnvParameterdvARB);
		glGetProgramEnvParameterfvARB   = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)   GET_EXTENSION_POINTER(glGetProgramEnvParameterfvARB);
		glProgramEnvParameter4dARB      = (PFNGLPROGRAMENVPARAMETER4DARBPROC)      GET_EXTENSION_POINTER(glProgramEnvParameter4dARB);
		glProgramEnvParameter4dvARB     = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)     GET_EXTENSION_POINTER(glProgramEnvParameter4dvARB);
		glProgramEnvParameter4fARB      = (PFNGLPROGRAMENVPARAMETER4FARBPROC)      GET_EXTENSION_POINTER(glProgramEnvParameter4fARB);
		glProgramEnvParameter4fvARB     = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)     GET_EXTENSION_POINTER(glProgramEnvParameter4fvARB);
		glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) GET_EXTENSION_POINTER(glGetProgramLocalParameterdvARB);
		glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) GET_EXTENSION_POINTER(glGetProgramLocalParameterfvARB);
		glProgramLocalParameter4dARB    = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)    GET_EXTENSION_POINTER(glProgramLocalParameter4dARB);
		glProgramLocalParameter4dvARB   = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)   GET_EXTENSION_POINTER(glProgramLocalParameter4dvARB);
		glProgramLocalParameter4fARB    = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)    GET_EXTENSION_POINTER(glProgramLocalParameter4fARB);
		glProgramLocalParameter4fvARB   = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)   GET_EXTENSION_POINTER(glProgramLocalParameter4fvARB);
		glGetProgramivARB               = (PFNGLGETPROGRAMIVARBPROC)               GET_EXTENSION_POINTER(glGetProgramivARB);
		glGetVertexAttribdvARB          = (PFNGLGETVERTEXATTRIBDVARBPROC)          GET_EXTENSION_POINTER(glGetVertexAttribdvARB);
		glGetVertexAttribfvARB          = (PFNGLGETVERTEXATTRIBFVARBPROC)          GET_EXTENSION_POINTER(glGetVertexAttribfvARB);
		glGetVertexAttribivARB          = (PFNGLGETVERTEXATTRIBIVARBPROC)          GET_EXTENSION_POINTER(glGetVertexAttribivARB);
		glGetVertexAttribPointervARB    = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)    GET_EXTENSION_POINTER(glGetVertexAttribPointervARB);
		glVertexAttribPointerARB        = (PFNGLVERTEXATTRIBPOINTERARBPROC)        GET_EXTENSION_POINTER(glVertexAttribPointerARB);
		glVertexAttrib1dARB             = (PFNGLVERTEXATTRIB1DARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib1dARB);
		glVertexAttrib1dvARB            = (PFNGLVERTEXATTRIB1DVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib1dvARB);
		glVertexAttrib1fARB             = (PFNGLVERTEXATTRIB1FARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib1fARB);
		glVertexAttrib1fvARB            = (PFNGLVERTEXATTRIB1FVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib1fvARB);
		glVertexAttrib1sARB             = (PFNGLVERTEXATTRIB1SARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib1sARB);
		glVertexAttrib1svARB            = (PFNGLVERTEXATTRIB1SVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib1svARB);
		glVertexAttrib2dARB             = (PFNGLVERTEXATTRIB2DARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib2dARB);
		glVertexAttrib2dvARB            = (PFNGLVERTEXATTRIB2DVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib2dvARB);
		glVertexAttrib2fARB             = (PFNGLVERTEXATTRIB2FARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib2fARB);
		glVertexAttrib2fvARB            = (PFNGLVERTEXATTRIB2FVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib2fvARB);
		glVertexAttrib2sARB             = (PFNGLVERTEXATTRIB2SARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib2sARB);
		glVertexAttrib2svARB            = (PFNGLVERTEXATTRIB2SVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib2svARB);
		glVertexAttrib3dARB             = (PFNGLVERTEXATTRIB3DARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib3dARB);
		glVertexAttrib3dvARB            = (PFNGLVERTEXATTRIB3DVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib3dvARB);
		glVertexAttrib3fARB             = (PFNGLVERTEXATTRIB3FARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib3fARB);
		glVertexAttrib3fvARB            = (PFNGLVERTEXATTRIB3FVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib3fvARB);
		glVertexAttrib3sARB             = (PFNGLVERTEXATTRIB3SARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib3sARB);
		glVertexAttrib3svARB            = (PFNGLVERTEXATTRIB3SVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib3svARB);
		glVertexAttrib4NbvARB           = (PFNGLVERTEXATTRIB4NBVARBPROC)           GET_EXTENSION_POINTER(glVertexAttrib4NbvARB);
		glVertexAttrib4NivARB           = (PFNGLVERTEXATTRIB4NIVARBPROC)           GET_EXTENSION_POINTER(glVertexAttrib4NivARB);
		glVertexAttrib4NsvARB           = (PFNGLVERTEXATTRIB4NSVARBPROC)           GET_EXTENSION_POINTER(glVertexAttrib4NsvARB);
		glVertexAttrib4NubARB           = (PFNGLVERTEXATTRIB4NUBARBPROC)           GET_EXTENSION_POINTER(glVertexAttrib4NubARB);
		glVertexAttrib4NubvARB          = (PFNGLVERTEXATTRIB4NUBVARBPROC)          GET_EXTENSION_POINTER(glVertexAttrib4NubvARB);
		glVertexAttrib4NuivARB          = (PFNGLVERTEXATTRIB4NUIVARBPROC)          GET_EXTENSION_POINTER(glVertexAttrib4NuivARB);
		glVertexAttrib4NusvARB          = (PFNGLVERTEXATTRIB4NUSVARBPROC)          GET_EXTENSION_POINTER(glVertexAttrib4NusvARB);
		glVertexAttrib4bvARB            = (PFNGLVERTEXATTRIB4BVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib4bvARB);
		glVertexAttrib4dARB             = (PFNGLVERTEXATTRIB4DARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib4dARB);
		glVertexAttrib4dvARB            = (PFNGLVERTEXATTRIB4DVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib4dvARB);
		glVertexAttrib4fARB             = (PFNGLVERTEXATTRIB4FARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib4fARB);
		glVertexAttrib4fvARB            = (PFNGLVERTEXATTRIB4FVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib4fvARB);
		glVertexAttrib4ivARB            = (PFNGLVERTEXATTRIB4IVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib4ivARB);
		glVertexAttrib4sARB             = (PFNGLVERTEXATTRIB4SARBPROC)             GET_EXTENSION_POINTER(glVertexAttrib4sARB);
		glVertexAttrib4svARB            = (PFNGLVERTEXATTRIB4SVARBPROC)            GET_EXTENSION_POINTER(glVertexAttrib4svARB);
		glVertexAttrib4ubvARB           = (PFNGLVERTEXATTRIB4UBVARBPROC)           GET_EXTENSION_POINTER(glVertexAttrib4ubvARB);
		glVertexAttrib4uivARB           = (PFNGLVERTEXATTRIB4UIVARBPROC)           GET_EXTENSION_POINTER(glVertexAttrib4uivARB);
		glVertexAttrib4usvARB           = (PFNGLVERTEXATTRIB4USVARBPROC)           GET_EXTENSION_POINTER(glVertexAttrib4usvARB);

		if (!CheckShaderProgramFunctions())
		{
			mIsShaderAsmProgramsSupported = false;
			ClearShaderProgramFunctions();
		}
	}

	if (IsExtensionSupported("GL_ARB_vertex_shader")        &&
	   IsExtensionSupported("GL_ARB_fragment_shader")      &&
	   IsExtensionSupported("GL_ARB_shader_objects")       &&
	   IsExtensionSupported("GL_ARB_shading_language_100"))
	{
		mIsShaderCProgramsSupported = true;

		glDeleteObjectARB               = (PFNGLDELETEOBJECTARBPROC)         GET_EXTENSION_POINTER(glDeleteObjectARB);
		glGetHandleARB                  = (PFNGLGETHANDLEARBPROC)            GET_EXTENSION_POINTER(glGetHandleARB);
		glDetachObjectARB               = (PFNGLDETACHOBJECTARBPROC)         GET_EXTENSION_POINTER(glDetachObjectARB);
		glCreateShaderObjectARB         = (PFNGLCREATESHADEROBJECTARBPROC)   GET_EXTENSION_POINTER(glCreateShaderObjectARB);
		glShaderSourceARB               = (PFNGLSHADERSOURCEARBPROC)         GET_EXTENSION_POINTER(glShaderSourceARB);
		glCompileShaderARB              = (PFNGLCOMPILESHADERARBPROC)        GET_EXTENSION_POINTER(glCompileShaderARB);
		glCreateProgramObjectARB        = (PFNGLCREATEPROGRAMOBJECTARBPROC)  GET_EXTENSION_POINTER(glCreateProgramObjectARB);
		glAttachObjectARB               = (PFNGLATTACHOBJECTARBPROC)         GET_EXTENSION_POINTER(glAttachObjectARB);
		glLinkProgramARB                = (PFNGLLINKPROGRAMARBPROC)          GET_EXTENSION_POINTER(glLinkProgramARB);
		glUseProgramObjectARB           = (PFNGLUSEPROGRAMOBJECTARBPROC)     GET_EXTENSION_POINTER(glUseProgramObjectARB);
		glValidateProgramARB            = (PFNGLVALIDATEPROGRAMARBPROC)      GET_EXTENSION_POINTER(glValidateProgramARB);
		glUniform1fARB                  = (PFNGLUNIFORM1FARBPROC)            GET_EXTENSION_POINTER(glUniform1fARB);
		glUniform2fARB                  = (PFNGLUNIFORM2FARBPROC)            GET_EXTENSION_POINTER(glUniform2fARB);
		glUniform3fARB                  = (PFNGLUNIFORM3FARBPROC)            GET_EXTENSION_POINTER(glUniform3fARB);
		glUniform4fARB                  = (PFNGLUNIFORM4FARBPROC)            GET_EXTENSION_POINTER(glUniform4fARB);
		glUniform1iARB                  = (PFNGLUNIFORM1IARBPROC)            GET_EXTENSION_POINTER(glUniform1iARB);
		glUniform2iARB                  = (PFNGLUNIFORM2IARBPROC)            GET_EXTENSION_POINTER(glUniform2iARB);
		glUniform3iARB                  = (PFNGLUNIFORM3IARBPROC)            GET_EXTENSION_POINTER(glUniform3iARB);
		glUniform4iARB                  = (PFNGLUNIFORM4IARBPROC)            GET_EXTENSION_POINTER(glUniform4iARB);
		glUniform1fvARB                 = (PFNGLUNIFORM1FVARBPROC)           GET_EXTENSION_POINTER(glUniform1fvARB);
		glUniform2fvARB                 = (PFNGLUNIFORM2FVARBPROC)           GET_EXTENSION_POINTER(glUniform2fvARB);
		glUniform3fvARB                 = (PFNGLUNIFORM3FVARBPROC)           GET_EXTENSION_POINTER(glUniform3fvARB);
		glUniform4fvARB                 = (PFNGLUNIFORM4FVARBPROC)           GET_EXTENSION_POINTER(glUniform4fvARB);
		glUniform1ivARB                 = (PFNGLUNIFORM1IVARBPROC)           GET_EXTENSION_POINTER(glUniform1ivARB);
		glUniform2ivARB                 = (PFNGLUNIFORM2IVARBPROC)           GET_EXTENSION_POINTER(glUniform2ivARB);
		glUniform3ivARB                 = (PFNGLUNIFORM3IVARBPROC)           GET_EXTENSION_POINTER(glUniform3ivARB);
		glUniform4ivARB                 = (PFNGLUNIFORM4IVARBPROC)           GET_EXTENSION_POINTER(glUniform4ivARB);
		glUniformMatrix2fvARB           = (PFNGLUNIFORMMATRIX2FVARBPROC)     GET_EXTENSION_POINTER(glUniformMatrix2fvARB);
		glUniformMatrix3fvARB           = (PFNGLUNIFORMMATRIX3FVARBPROC)     GET_EXTENSION_POINTER(glUniformMatrix3fvARB);
		glUniformMatrix4fvARB           = (PFNGLUNIFORMMATRIX4FVARBPROC)     GET_EXTENSION_POINTER(glUniformMatrix4fvARB);
		glGetObjectParameterfvARB       = (PFNGLGETOBJECTPARAMETERFVARBPROC) GET_EXTENSION_POINTER(glGetObjectParameterfvARB);
		glGetObjectParameterivARB       = (PFNGLGETOBJECTPARAMETERIVARBPROC) GET_EXTENSION_POINTER(glGetObjectParameterivARB);
		glGetInfoLogARB                 = (PFNGLGETINFOLOGARBPROC)           GET_EXTENSION_POINTER(glGetInfoLogARB);
		glGetAttachedObjectsARB         = (PFNGLGETATTACHEDOBJECTSARBPROC)   GET_EXTENSION_POINTER(glGetAttachedObjectsARB);
		glGetUniformLocationARB         = (PFNGLGETUNIFORMLOCATIONARBPROC)   GET_EXTENSION_POINTER(glGetUniformLocationARB);
		glGetActiveUniformARB           = (PFNGLGETACTIVEUNIFORMARBPROC)     GET_EXTENSION_POINTER(glGetActiveUniformARB);
		glGetUniformfvARB               = (PFNGLGETUNIFORMFVARBPROC)         GET_EXTENSION_POINTER(glGetUniformfvARB);
		glGetUniformivARB               = (PFNGLGETUNIFORMIVARBPROC)         GET_EXTENSION_POINTER(glGetUniformivARB);
		glGetShaderSourceARB            = (PFNGLGETSHADERSOURCEARBPROC)      GET_EXTENSION_POINTER(glGetShaderSourceARB);

		if (!CheckShaderFunctions())
		{
			mIsShaderCProgramsSupported = false;
			ClearShaderFunctions();
		}
	}
#endif // !ES1
}

bool OpenGLExtensions::IsFrameBufferObjectsSupported()
{
	return mIsFrameBufferObjectsSupported;
}

bool OpenGLExtensions::IsBufferObjectsSupported()
{
	return mIsBufferObjectsSupported;
}

bool OpenGLExtensions::IsAnisotropicFilteringSupported()
{
	return mIsAnisotropicFilteringSupported;
}

bool OpenGLExtensions::IsCompressedTexturesSupported()
{
	return mIsCompressedTexturesSupported;
}

bool OpenGLExtensions::IsMultiTextureSupported()
{
	return mIsMultiTextureSupported;
}

bool OpenGLExtensions::IsShadowMapsSupported()
{
	return mIsGLVersion14;
}

bool OpenGLExtensions::IsShaderAsmProgramsSupported()
{
	return mIsShaderAsmProgramsSupported;
}

bool OpenGLExtensions::IsShaderCProgramsSupported()
{
	return mIsShaderCProgramsSupported;
}

float OpenGLExtensions::GetMaxAnisotropy()
{
	return mMaxAnisotropy;
}

void OpenGLExtensions::SetAnisotropy(float pAmountAnisotropy)
{
	if (mIsAnisotropicFilteringSupported)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, pAmountAnisotropy);
	}
}

bool OpenGLExtensions::IsVSyncEnabled()
{
#ifdef LEPRA_WINDOWS
	if (wglGetSwapIntervalEXT != 0)
	{
		return (wglGetSwapIntervalEXT() > 0);
	}
#elif defined(LEPRA_MAC) && !defined(LEPRA_IOS)
	CGLContextObj lContext = CGLGetCurrentContext();
	GLint lSwapInterval = 0;
	CGLGetParameter(lContext, kCGLCPSwapInterval, &lSwapInterval);
	return lSwapInterval != 0;
#else
#pragma message("Warning: OpenGLExtensions::IsVSyncEnabled() is using default behaviour.")
#endif
	return true;
}

bool OpenGLExtensions::SetVSyncEnabled(bool pEnabled)
{
#ifdef LEPRA_WINDOWS
	if (wglSwapIntervalEXT != 0)
	{
		return (wglSwapIntervalEXT(pEnabled ? 1 : 0) != FALSE);
	}
#elif defined(LEPRA_MAC) && !defined(LEPRA_IOS)
	CGLContextObj lContext = CGLGetCurrentContext();
	GLint lSwapInterval = pEnabled? 1 : 0;
	CGLSetParameter(lContext, kCGLCPSwapInterval, &lSwapInterval);
#else
#pragma message("Warning: OpenGLExtensions::SetVSyncEnabled() is not implemented.")
#endif
	return (false);
}



void OpenGLExtensions::ClearFrameBufferObjectFunctions()
{
#ifndef LEPRA_GL_ES_1
	glIsRenderbufferEXT                      = 0;
	glBindRenderbufferEXT                    = 0;
	glDeleteRenderbuffersEXT                 = 0;
	glGenRenderbuffersEXT                    = 0;
	glRenderbufferStorageEXT                 = 0;
	glGetRenderbufferParameterivEXT          = 0;
	glIsFramebufferEXT                       = 0;
	glBindFramebufferEXT                     = 0;
	glDeleteFramebuffersEXT                  = 0;
	glGenFramebuffersEXT                     = 0;
	glCheckFramebufferStatusEXT              = 0;
	glFramebufferTexture1DEXT                = 0;
	glFramebufferTexture2DEXT                = 0;
	glFramebufferTexture3DEXT                = 0;
	glFramebufferRenderbufferEXT             = 0;
	glGetFramebufferAttachmentParameterivEXT = 0;
	glGenerateMipmapEXT                      = 0;
#endif // !ES1
}

void OpenGLExtensions::ClearBufferObjectFunctions()
{
	glBindBuffer    = 0;
	glBufferData    = 0;
	glBufferSubData = 0;
	glDeleteBuffers = 0;
	glGenBuffers    = 0;
	glMapBuffer     = 0;
	glUnmapBuffer   = 0;
}

void OpenGLExtensions::ClearMultiTextureFunctions()
{
#ifndef LEPRA_GL_ES_1
	glActiveTexture       = 0;
	glClientActiveTexture = 0;
	glMultiTexCoord1d     = 0;
	glMultiTexCoord1dv    = 0;
	glMultiTexCoord1f     = 0;
	glMultiTexCoord1fv    = 0;
	glMultiTexCoord1i     = 0;
	glMultiTexCoord1iv    = 0;
	glMultiTexCoord1s     = 0;
	glMultiTexCoord1sv    = 0;
	glMultiTexCoord2d     = 0;
	glMultiTexCoord2dv    = 0;
	glMultiTexCoord2f     = 0;
	glMultiTexCoord2fv    = 0;
	glMultiTexCoord2i     = 0;
	glMultiTexCoord2iv    = 0;
	glMultiTexCoord2s     = 0;
	glMultiTexCoord2sv    = 0;
	glMultiTexCoord3d     = 0;
	glMultiTexCoord3dv    = 0;
	glMultiTexCoord3f     = 0;
	glMultiTexCoord3fv    = 0;
	glMultiTexCoord3i     = 0;
	glMultiTexCoord3iv    = 0;
	glMultiTexCoord3s     = 0;
	glMultiTexCoord3sv    = 0;
	glMultiTexCoord4d     = 0;
	glMultiTexCoord4dv    = 0;
	glMultiTexCoord4f     = 0;
	glMultiTexCoord4fv    = 0;
	glMultiTexCoord4i     = 0;
	glMultiTexCoord4iv    = 0;
	glMultiTexCoord4s     = 0;
	glMultiTexCoord4sv    = 0;
#endif // !ES1
}

void OpenGLExtensions::ClearShaderProgramFunctions()
{
#ifndef LEPRA_GL_ES_1
	glGenProgramsARB                = 0;
	glBindProgramARB                = 0;
	glDeleteProgramsARB             = 0;
	glIsProgramARB                  = 0;
	glProgramStringARB              = 0;
	glGetProgramStringARB           = 0;
	glEnableVertexAttribArrayARB    = 0;
	glDisableVertexAttribArrayARB   = 0;
	glGetProgramEnvParameterdvARB   = 0;
	glGetProgramEnvParameterfvARB   = 0;
	glProgramEnvParameter4dARB      = 0;
	glProgramEnvParameter4dvARB     = 0;
	glProgramEnvParameter4fARB      = 0;
	glProgramEnvParameter4fvARB     = 0;
	glGetProgramLocalParameterdvARB = 0;
	glGetProgramLocalParameterfvARB = 0;
	glProgramLocalParameter4dARB    = 0;
	glProgramLocalParameter4dvARB   = 0;
	glProgramLocalParameter4fARB    = 0;
	glProgramLocalParameter4fvARB   = 0;
	glGetProgramivARB               = 0;
	glGetVertexAttribdvARB          = 0;
	glGetVertexAttribfvARB          = 0;
	glGetVertexAttribivARB          = 0;
	glGetVertexAttribPointervARB    = 0;
	glVertexAttribPointerARB        = 0;
	glVertexAttrib1dARB             = 0;
	glVertexAttrib1dvARB            = 0;
	glVertexAttrib1fARB             = 0;
	glVertexAttrib1fvARB            = 0;
	glVertexAttrib1sARB             = 0;
	glVertexAttrib1svARB            = 0;
	glVertexAttrib2dARB             = 0;
	glVertexAttrib2dvARB            = 0;
	glVertexAttrib2fARB             = 0;
	glVertexAttrib2fvARB            = 0;
	glVertexAttrib2sARB             = 0;
	glVertexAttrib2svARB            = 0;
	glVertexAttrib3dARB             = 0;
	glVertexAttrib3dvARB            = 0;
	glVertexAttrib3fARB             = 0;
	glVertexAttrib3fvARB            = 0;
	glVertexAttrib3sARB             = 0;
	glVertexAttrib3svARB            = 0;
	glVertexAttrib4NbvARB           = 0;
	glVertexAttrib4NivARB           = 0;
	glVertexAttrib4NsvARB           = 0;
	glVertexAttrib4NubARB           = 0;
	glVertexAttrib4NubvARB          = 0;
	glVertexAttrib4NuivARB          = 0;
	glVertexAttrib4NusvARB          = 0;
	glVertexAttrib4bvARB            = 0;
	glVertexAttrib4dARB             = 0;
	glVertexAttrib4dvARB            = 0;
	glVertexAttrib4fARB             = 0;
	glVertexAttrib4fvARB            = 0;
	glVertexAttrib4ivARB            = 0;
	glVertexAttrib4sARB             = 0;
	glVertexAttrib4svARB            = 0;
	glVertexAttrib4ubvARB           = 0;
	glVertexAttrib4uivARB           = 0;
	glVertexAttrib4usvARB           = 0;
#endif // !ES1
}

void OpenGLExtensions::ClearShaderFunctions()
{
#ifndef LEPRA_GL_ES_1
	glDeleteObjectARB               = 0;
	glGetHandleARB                  = 0;
	glDetachObjectARB               = 0;
	glCreateShaderObjectARB         = 0;
	glShaderSourceARB               = 0;
	glCompileShaderARB              = 0;
	glCreateProgramObjectARB        = 0;
	glAttachObjectARB               = 0;
	glLinkProgramARB                = 0;
	glUseProgramObjectARB           = 0;
	glValidateProgramARB            = 0;
	glUniform1fARB                  = 0;
	glUniform2fARB                  = 0;
	glUniform3fARB                  = 0;
	glUniform4fARB                  = 0;
	glUniform1iARB                  = 0;
	glUniform2iARB                  = 0;
	glUniform3iARB                  = 0;
	glUniform4iARB                  = 0;
	glUniform1fvARB                 = 0;
	glUniform2fvARB                 = 0;
	glUniform3fvARB                 = 0;
	glUniform4fvARB                 = 0;
	glUniform1ivARB                 = 0;
	glUniform2ivARB                 = 0;
	glUniform3ivARB                 = 0;
	glUniform4ivARB                 = 0;
	glUniformMatrix2fvARB           = 0;
	glUniformMatrix3fvARB           = 0;
	glUniformMatrix4fvARB           = 0;
	glGetObjectParameterfvARB       = 0;
	glGetObjectParameterivARB       = 0;
	glGetInfoLogARB                 = 0;
	glGetAttachedObjectsARB         = 0;
	glGetUniformLocationARB         = 0;
	glGetActiveUniformARB           = 0;
	glGetUniformfvARB               = 0;
	glGetUniformivARB               = 0;
	glGetShaderSourceARB            = 0;
#endif // !ES1
}

bool OpenGLExtensions::CheckFrameBufferObjectFunctions()
{
#ifndef LEPRA_GL_ES_1
	return (glIsRenderbufferEXT                      != 0 &&
		glBindRenderbufferEXT                    != 0 &&
		glDeleteRenderbuffersEXT                 != 0 &&
		glGenRenderbuffersEXT                    != 0 &&
		glRenderbufferStorageEXT                 != 0 &&
		glGetRenderbufferParameterivEXT          != 0 &&
		glIsFramebufferEXT                       != 0 &&
		glBindFramebufferEXT                     != 0 &&
		glDeleteFramebuffersEXT                  != 0 &&
		glGenFramebuffersEXT                     != 0 &&
		glCheckFramebufferStatusEXT              != 0 &&
		glFramebufferTexture1DEXT                != 0 &&
		glFramebufferTexture2DEXT                != 0 &&
		glFramebufferTexture3DEXT                != 0 &&
		glFramebufferRenderbufferEXT             != 0 &&
		glGetFramebufferAttachmentParameterivEXT != 0 &&
		glGenerateMipmapEXT                      != 0);
#else // !ES1
	return false;
#endif // ES1/!ES1
}

bool OpenGLExtensions::CheckBufferObjectFunctions()
{
	return (glBindBuffer    != 0 &&
		glBufferData    != 0 &&
		glBufferSubData != 0 &&
		glDeleteBuffers != 0 &&
		glGenBuffers    != 0 &&
		glMapBuffer     != 0 &&
		glUnmapBuffer   != 0);
}

bool OpenGLExtensions::CheckMultiTextureFunctions()
{
#ifndef LEPRA_GL_ES_1
	return (glActiveTexture       != 0 &&
		glClientActiveTexture != 0 &&
		glMultiTexCoord1d     != 0 &&
		glMultiTexCoord1dv    != 0 &&
		glMultiTexCoord1f     != 0 &&
		glMultiTexCoord1fv    != 0 &&
		glMultiTexCoord1i     != 0 &&
		glMultiTexCoord1iv    != 0 &&
		glMultiTexCoord1s     != 0 &&
		glMultiTexCoord1sv    != 0 &&
		glMultiTexCoord2d     != 0 &&
		glMultiTexCoord2dv    != 0 &&
		glMultiTexCoord2f     != 0 &&
		glMultiTexCoord2fv    != 0 &&
		glMultiTexCoord2i     != 0 &&
		glMultiTexCoord2iv    != 0 &&
		glMultiTexCoord2s     != 0 &&
		glMultiTexCoord2sv    != 0 &&
		glMultiTexCoord3d     != 0 &&
		glMultiTexCoord3dv    != 0 &&
		glMultiTexCoord3f     != 0 &&
		glMultiTexCoord3fv    != 0 &&
		glMultiTexCoord3i     != 0 &&
		glMultiTexCoord3iv    != 0 &&
		glMultiTexCoord3s     != 0 &&
		glMultiTexCoord3sv    != 0 &&
		glMultiTexCoord4d     != 0 &&
		glMultiTexCoord4dv    != 0 &&
		glMultiTexCoord4f     != 0 &&
		glMultiTexCoord4fv    != 0 &&
		glMultiTexCoord4i     != 0 &&
		glMultiTexCoord4iv    != 0 &&
		glMultiTexCoord4s     != 0 &&
		glMultiTexCoord4sv    != 0);
#else // !ES1
	return false;
#endif // ES1/!ES1
}

bool OpenGLExtensions::CheckShaderProgramFunctions()
{
#ifndef LEPRA_GL_ES_1
	return (glGenProgramsARB                != 0 &&
		glBindProgramARB                != 0 &&
		glDeleteProgramsARB             != 0 &&
		glIsProgramARB                  != 0 &&
		glProgramStringARB              != 0 &&
		glGetProgramStringARB           != 0 &&
		glEnableVertexAttribArrayARB    != 0 &&
		glDisableVertexAttribArrayARB   != 0 &&
		glGetProgramEnvParameterdvARB   != 0 &&
		glGetProgramEnvParameterfvARB   != 0 &&
		glProgramEnvParameter4dARB      != 0 &&
		glProgramEnvParameter4dvARB     != 0 &&
		glProgramEnvParameter4fARB      != 0 &&
		glProgramEnvParameter4fvARB     != 0 &&
		glGetProgramLocalParameterdvARB != 0 &&
		glGetProgramLocalParameterfvARB != 0 &&
		glProgramLocalParameter4dARB    != 0 &&
		glProgramLocalParameter4dvARB   != 0 &&
		glProgramLocalParameter4fARB    != 0 &&
		glProgramLocalParameter4fvARB   != 0 &&
		glGetProgramivARB               != 0 &&
		glGetVertexAttribdvARB          != 0 &&
		glGetVertexAttribfvARB          != 0 &&
		glGetVertexAttribivARB          != 0 &&
		glGetVertexAttribPointervARB    != 0 &&
		glVertexAttribPointerARB        != 0 &&
		glVertexAttrib1dARB             != 0 &&
		glVertexAttrib1dvARB            != 0 &&
		glVertexAttrib1fARB             != 0 &&
		glVertexAttrib1fvARB            != 0 &&
		glVertexAttrib1sARB             != 0 &&
		glVertexAttrib1svARB            != 0 &&
		glVertexAttrib2dARB             != 0 &&
		glVertexAttrib2dvARB            != 0 &&
		glVertexAttrib2fARB             != 0 &&
		glVertexAttrib2fvARB            != 0 &&
		glVertexAttrib2sARB             != 0 &&
		glVertexAttrib2svARB            != 0 &&
		glVertexAttrib3dARB             != 0 &&
		glVertexAttrib3dvARB            != 0 &&
		glVertexAttrib3fARB             != 0 &&
		glVertexAttrib3fvARB            != 0 &&
		glVertexAttrib3sARB             != 0 &&
		glVertexAttrib3svARB            != 0 &&
		glVertexAttrib4NbvARB           != 0 &&
		glVertexAttrib4NivARB           != 0 &&
		glVertexAttrib4NsvARB           != 0 &&
		glVertexAttrib4NubARB           != 0 &&
		glVertexAttrib4NubvARB          != 0 &&
		glVertexAttrib4NuivARB          != 0 &&
		glVertexAttrib4NusvARB          != 0 &&
		glVertexAttrib4bvARB            != 0 &&
		glVertexAttrib4dARB             != 0 &&
		glVertexAttrib4dvARB            != 0 &&
		glVertexAttrib4fARB             != 0 &&
		glVertexAttrib4fvARB            != 0 &&
		glVertexAttrib4ivARB            != 0 &&
		glVertexAttrib4sARB             != 0 &&
		glVertexAttrib4svARB            != 0 &&
		glVertexAttrib4ubvARB           != 0 &&
		glVertexAttrib4uivARB           != 0 &&
		glVertexAttrib4usvARB           != 0);
#else // !ES1
	return false;
#endif // ES1/!ES1
}

bool OpenGLExtensions::CheckShaderFunctions()
{
#ifndef LEPRA_GL_ES_1
	return (glDeleteObjectARB               != 0 &&
		glGetHandleARB                  != 0 &&
		glDetachObjectARB               != 0 &&
		glCreateShaderObjectARB         != 0 &&
		glShaderSourceARB               != 0 &&
		glCompileShaderARB              != 0 &&
		glCreateProgramObjectARB        != 0 &&
		glAttachObjectARB               != 0 &&
		glLinkProgramARB                != 0 &&
		glUseProgramObjectARB           != 0 &&
		glValidateProgramARB            != 0 &&
		glUniform1fARB                  != 0 &&
		glUniform2fARB                  != 0 &&
		glUniform3fARB                  != 0 &&
		glUniform4fARB                  != 0 &&
		glUniform1iARB                  != 0 &&
		glUniform2iARB                  != 0 &&
		glUniform3iARB                  != 0 &&
		glUniform4iARB                  != 0 &&
		glUniform1fvARB                 != 0 &&
		glUniform2fvARB                 != 0 &&
		glUniform3fvARB                 != 0 &&
		glUniform4fvARB                 != 0 &&
		glUniform1ivARB                 != 0 &&
		glUniform2ivARB                 != 0 &&
		glUniform3ivARB                 != 0 &&
		glUniform4ivARB                 != 0 &&
		glUniformMatrix2fvARB           != 0 &&
		glUniformMatrix3fvARB           != 0 &&
		glUniformMatrix4fvARB           != 0 &&
		glGetObjectParameterfvARB       != 0 &&
		glGetObjectParameterivARB       != 0 &&
		glGetInfoLogARB                 != 0 &&
		glGetAttachedObjectsARB         != 0 &&
		glGetUniformLocationARB         != 0 &&
		glGetActiveUniformARB           != 0 &&
		glGetUniformfvARB               != 0 &&
		glGetUniformivARB               != 0 &&
		glGetShaderSourceARB            != 0);
#else // !ES1
	return false;
#endif // ES1/!ES1
}



bool OpenGLExtensions::mIsGLVersion14			= false;
bool OpenGLExtensions::mIsGLVersion15			= false;
bool OpenGLExtensions::mIsFrameBufferObjectsSupported	= false;
bool OpenGLExtensions::mIsBufferObjectsSupported	= false;
bool OpenGLExtensions::mIsAnisotropicFilteringSupported	= false;
bool OpenGLExtensions::mIsCompressedTexturesSupported	= false;
bool OpenGLExtensions::mIsMultiTextureSupported		= false;
bool OpenGLExtensions::mIsShaderAsmProgramsSupported	= false;
bool OpenGLExtensions::mIsShaderCProgramsSupported	= false;
float OpenGLExtensions::mMaxAnisotropy			= 1.0f;

// Declare functions.

#ifdef LEPRA_WINDOWS
PFNWGLSWAPINTERVALEXTPROC    OpenGLExtensions::wglSwapIntervalEXT    = 0;
PFNWGLGETSWAPINTERVALEXTPROC OpenGLExtensions::wglGetSwapIntervalEXT = 0;
#endif

#ifndef LEPRA_GL_ES_1
PFNGLISRENDERBUFFEREXTPROC                      OpenGLExtensions::glIsRenderbufferEXT                      = 0;
PFNGLBINDRENDERBUFFEREXTPROC                    OpenGLExtensions::glBindRenderbufferEXT                    = 0;
PFNGLDELETERENDERBUFFERSEXTPROC                 OpenGLExtensions::glDeleteRenderbuffersEXT                 = 0;
PFNGLGENRENDERBUFFERSEXTPROC                    OpenGLExtensions::glGenRenderbuffersEXT                    = 0;
PFNGLRENDERBUFFERSTORAGEEXTPROC                 OpenGLExtensions::glRenderbufferStorageEXT                 = 0;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC          OpenGLExtensions::glGetRenderbufferParameterivEXT          = 0;
PFNGLISFRAMEBUFFEREXTPROC                       OpenGLExtensions::glIsFramebufferEXT                       = 0;
PFNGLBINDFRAMEBUFFEREXTPROC                     OpenGLExtensions::glBindFramebufferEXT                     = 0;
PFNGLDELETEFRAMEBUFFERSEXTPROC                  OpenGLExtensions::glDeleteFramebuffersEXT                  = 0;
PFNGLGENFRAMEBUFFERSEXTPROC                     OpenGLExtensions::glGenFramebuffersEXT                     = 0;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC              OpenGLExtensions::glCheckFramebufferStatusEXT              = 0;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC                OpenGLExtensions::glFramebufferTexture1DEXT                = 0;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                OpenGLExtensions::glFramebufferTexture2DEXT                = 0;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC                OpenGLExtensions::glFramebufferTexture3DEXT                = 0;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC             OpenGLExtensions::glFramebufferRenderbufferEXT             = 0;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC OpenGLExtensions::glGetFramebufferAttachmentParameterivEXT = 0;
PFNGLGENERATEMIPMAPEXTPROC                      OpenGLExtensions::glGenerateMipmapEXT                      = 0;
#endif // !ES1

PFNGLBINDBUFFERPROC       OpenGLExtensions::glBindBuffer       = 0;
PFNGLBUFFERDATAPROC       OpenGLExtensions::glBufferData       = 0;
PFNGLBUFFERSUBDATAPROC    OpenGLExtensions::glBufferSubData    = 0;
PFNGLDELETEBUFFERSPROC    OpenGLExtensions::glDeleteBuffers    = 0;
PFNGLGENBUFFERSPROC       OpenGLExtensions::glGenBuffers       = 0;
PFNGLMAPBUFFERPROC        OpenGLExtensions::glMapBuffer        = 0;
PFNGLUNMAPBUFFERPROC      OpenGLExtensions::glUnmapBuffer      = 0;

PFNGLACTIVETEXTUREPROC    OpenGLExtensions::glActiveTexture    = 0;
PFNGLCLIENTACTIVETEXTUREPROC OpenGLExtensions::glClientActiveTexture = 0;
#ifndef LEPRA_GL_ES_1
PFNGLMULTITEXCOORD1DPROC  OpenGLExtensions::glMultiTexCoord1d  = 0;
PFNGLMULTITEXCOORD1DVPROC OpenGLExtensions::glMultiTexCoord1dv = 0;
PFNGLMULTITEXCOORD1FPROC  OpenGLExtensions::glMultiTexCoord1f  = 0;
PFNGLMULTITEXCOORD1FVPROC OpenGLExtensions::glMultiTexCoord1fv = 0;
PFNGLMULTITEXCOORD1IPROC  OpenGLExtensions::glMultiTexCoord1i  = 0;
PFNGLMULTITEXCOORD1IVPROC OpenGLExtensions::glMultiTexCoord1iv = 0;
PFNGLMULTITEXCOORD1SPROC  OpenGLExtensions::glMultiTexCoord1s  = 0;
PFNGLMULTITEXCOORD1SVPROC OpenGLExtensions::glMultiTexCoord1sv = 0;
PFNGLMULTITEXCOORD2DPROC  OpenGLExtensions::glMultiTexCoord2d  = 0;
PFNGLMULTITEXCOORD2DVPROC OpenGLExtensions::glMultiTexCoord2dv = 0;
PFNGLMULTITEXCOORD2FPROC  OpenGLExtensions::glMultiTexCoord2f  = 0;
PFNGLMULTITEXCOORD2FVPROC OpenGLExtensions::glMultiTexCoord2fv = 0;
PFNGLMULTITEXCOORD2IPROC  OpenGLExtensions::glMultiTexCoord2i  = 0;
PFNGLMULTITEXCOORD2IVPROC OpenGLExtensions::glMultiTexCoord2iv = 0;
PFNGLMULTITEXCOORD2SPROC  OpenGLExtensions::glMultiTexCoord2s  = 0;
PFNGLMULTITEXCOORD2SVPROC OpenGLExtensions::glMultiTexCoord2sv = 0;
PFNGLMULTITEXCOORD3DPROC  OpenGLExtensions::glMultiTexCoord3d  = 0;
PFNGLMULTITEXCOORD3DVPROC OpenGLExtensions::glMultiTexCoord3dv = 0;
PFNGLMULTITEXCOORD3FPROC  OpenGLExtensions::glMultiTexCoord3f  = 0;
PFNGLMULTITEXCOORD3FVPROC OpenGLExtensions::glMultiTexCoord3fv = 0;
PFNGLMULTITEXCOORD3IPROC  OpenGLExtensions::glMultiTexCoord3i  = 0;
PFNGLMULTITEXCOORD3IVPROC OpenGLExtensions::glMultiTexCoord3iv = 0;
PFNGLMULTITEXCOORD3SPROC  OpenGLExtensions::glMultiTexCoord3s  = 0;
PFNGLMULTITEXCOORD3SVPROC OpenGLExtensions::glMultiTexCoord3sv = 0;
PFNGLMULTITEXCOORD4DPROC  OpenGLExtensions::glMultiTexCoord4d  = 0;
PFNGLMULTITEXCOORD4DVPROC OpenGLExtensions::glMultiTexCoord4dv = 0;
PFNGLMULTITEXCOORD4FPROC  OpenGLExtensions::glMultiTexCoord4f  = 0;
PFNGLMULTITEXCOORD4FVPROC OpenGLExtensions::glMultiTexCoord4fv = 0;
PFNGLMULTITEXCOORD4IPROC  OpenGLExtensions::glMultiTexCoord4i  = 0;
PFNGLMULTITEXCOORD4IVPROC OpenGLExtensions::glMultiTexCoord4iv = 0;
PFNGLMULTITEXCOORD4SPROC  OpenGLExtensions::glMultiTexCoord4s  = 0;
PFNGLMULTITEXCOORD4SVPROC OpenGLExtensions::glMultiTexCoord4sv = 0;

PFNGLGENPROGRAMSARBPROC                OpenGLExtensions::glGenProgramsARB                = 0;
PFNGLBINDPROGRAMARBPROC                OpenGLExtensions::glBindProgramARB                = 0;
PFNGLDELETEPROGRAMSARBPROC             OpenGLExtensions::glDeleteProgramsARB             = 0;
PFNGLISPROGRAMARBPROC                  OpenGLExtensions::glIsProgramARB                  = 0;
PFNGLPROGRAMSTRINGARBPROC              OpenGLExtensions::glProgramStringARB              = 0;
PFNGLGETPROGRAMIVARBPROC               OpenGLExtensions::glGetProgramStringARB           = 0;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC    OpenGLExtensions::glEnableVertexAttribArrayARB    = 0;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC   OpenGLExtensions::glDisableVertexAttribArrayARB   = 0;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC   OpenGLExtensions::glGetProgramEnvParameterdvARB   = 0;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC   OpenGLExtensions::glGetProgramEnvParameterfvARB   = 0;
PFNGLPROGRAMENVPARAMETER4DARBPROC      OpenGLExtensions::glProgramEnvParameter4dARB      = 0;
PFNGLPROGRAMENVPARAMETER4DVARBPROC     OpenGLExtensions::glProgramEnvParameter4dvARB     = 0;
PFNGLPROGRAMENVPARAMETER4FARBPROC      OpenGLExtensions::glProgramEnvParameter4fARB      = 0;
PFNGLPROGRAMENVPARAMETER4FVARBPROC     OpenGLExtensions::glProgramEnvParameter4fvARB     = 0;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC OpenGLExtensions::glGetProgramLocalParameterdvARB = 0;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC OpenGLExtensions::glGetProgramLocalParameterfvARB = 0;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC    OpenGLExtensions::glProgramLocalParameter4dARB    = 0;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC   OpenGLExtensions::glProgramLocalParameter4dvARB   = 0;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC    OpenGLExtensions::glProgramLocalParameter4fARB    = 0;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC   OpenGLExtensions::glProgramLocalParameter4fvARB   = 0;
PFNGLGETPROGRAMIVARBPROC               OpenGLExtensions::glGetProgramivARB               = 0;
PFNGLGETVERTEXATTRIBDVARBPROC          OpenGLExtensions::glGetVertexAttribdvARB          = 0;
PFNGLGETVERTEXATTRIBFVARBPROC          OpenGLExtensions::glGetVertexAttribfvARB          = 0;
PFNGLGETVERTEXATTRIBIVARBPROC          OpenGLExtensions::glGetVertexAttribivARB          = 0;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC    OpenGLExtensions::glGetVertexAttribPointervARB    = 0;
PFNGLVERTEXATTRIBPOINTERARBPROC        OpenGLExtensions::glVertexAttribPointerARB        = 0;
PFNGLVERTEXATTRIB1DARBPROC             OpenGLExtensions::glVertexAttrib1dARB             = 0;
PFNGLVERTEXATTRIB1DVARBPROC            OpenGLExtensions::glVertexAttrib1dvARB            = 0;
PFNGLVERTEXATTRIB1FARBPROC             OpenGLExtensions::glVertexAttrib1fARB             = 0;
PFNGLVERTEXATTRIB1FVARBPROC            OpenGLExtensions::glVertexAttrib1fvARB            = 0;
PFNGLVERTEXATTRIB1SARBPROC             OpenGLExtensions::glVertexAttrib1sARB             = 0;
PFNGLVERTEXATTRIB1SVARBPROC            OpenGLExtensions::glVertexAttrib1svARB            = 0;
PFNGLVERTEXATTRIB2DARBPROC             OpenGLExtensions::glVertexAttrib2dARB             = 0;
PFNGLVERTEXATTRIB2DVARBPROC            OpenGLExtensions::glVertexAttrib2dvARB            = 0;
PFNGLVERTEXATTRIB2FARBPROC             OpenGLExtensions::glVertexAttrib2fARB             = 0;
PFNGLVERTEXATTRIB2FVARBPROC            OpenGLExtensions::glVertexAttrib2fvARB            = 0;
PFNGLVERTEXATTRIB2SARBPROC             OpenGLExtensions::glVertexAttrib2sARB             = 0;
PFNGLVERTEXATTRIB2SVARBPROC            OpenGLExtensions::glVertexAttrib2svARB            = 0;
PFNGLVERTEXATTRIB3DARBPROC             OpenGLExtensions::glVertexAttrib3dARB             = 0;
PFNGLVERTEXATTRIB3DVARBPROC            OpenGLExtensions::glVertexAttrib3dvARB            = 0;
PFNGLVERTEXATTRIB3FARBPROC             OpenGLExtensions::glVertexAttrib3fARB             = 0;
PFNGLVERTEXATTRIB3FVARBPROC            OpenGLExtensions::glVertexAttrib3fvARB            = 0;
PFNGLVERTEXATTRIB3SARBPROC             OpenGLExtensions::glVertexAttrib3sARB             = 0;
PFNGLVERTEXATTRIB3SVARBPROC            OpenGLExtensions::glVertexAttrib3svARB            = 0;
PFNGLVERTEXATTRIB4NBVARBPROC           OpenGLExtensions::glVertexAttrib4NbvARB           = 0;
PFNGLVERTEXATTRIB4NIVARBPROC           OpenGLExtensions::glVertexAttrib4NivARB           = 0;
PFNGLVERTEXATTRIB4NSVARBPROC           OpenGLExtensions::glVertexAttrib4NsvARB           = 0;
PFNGLVERTEXATTRIB4NUBARBPROC           OpenGLExtensions::glVertexAttrib4NubARB           = 0;
PFNGLVERTEXATTRIB4NUBVARBPROC          OpenGLExtensions::glVertexAttrib4NubvARB          = 0;
PFNGLVERTEXATTRIB4NUIVARBPROC          OpenGLExtensions::glVertexAttrib4NuivARB          = 0;
PFNGLVERTEXATTRIB4NUSVARBPROC          OpenGLExtensions::glVertexAttrib4NusvARB          = 0;
PFNGLVERTEXATTRIB4BVARBPROC            OpenGLExtensions::glVertexAttrib4bvARB            = 0;
PFNGLVERTEXATTRIB4DARBPROC             OpenGLExtensions::glVertexAttrib4dARB             = 0;
PFNGLVERTEXATTRIB4DVARBPROC            OpenGLExtensions::glVertexAttrib4dvARB            = 0;
PFNGLVERTEXATTRIB4FARBPROC             OpenGLExtensions::glVertexAttrib4fARB             = 0;
PFNGLVERTEXATTRIB4FVARBPROC            OpenGLExtensions::glVertexAttrib4fvARB            = 0;
PFNGLVERTEXATTRIB4IVARBPROC            OpenGLExtensions::glVertexAttrib4ivARB            = 0;
PFNGLVERTEXATTRIB4SARBPROC             OpenGLExtensions::glVertexAttrib4sARB             = 0;
PFNGLVERTEXATTRIB4SVARBPROC            OpenGLExtensions::glVertexAttrib4svARB            = 0;
PFNGLVERTEXATTRIB4UBVARBPROC           OpenGLExtensions::glVertexAttrib4ubvARB           = 0;
PFNGLVERTEXATTRIB4UIVARBPROC           OpenGLExtensions::glVertexAttrib4uivARB           = 0;
PFNGLVERTEXATTRIB4USVARBPROC           OpenGLExtensions::glVertexAttrib4usvARB           = 0;

PFNGLDELETEOBJECTARBPROC               OpenGLExtensions::glDeleteObjectARB               = 0;
PFNGLGETHANDLEARBPROC                  OpenGLExtensions::glGetHandleARB                  = 0;
PFNGLDETACHOBJECTARBPROC               OpenGLExtensions::glDetachObjectARB               = 0;
PFNGLCREATESHADEROBJECTARBPROC         OpenGLExtensions::glCreateShaderObjectARB         = 0;
PFNGLSHADERSOURCEARBPROC               OpenGLExtensions::glShaderSourceARB               = 0;
PFNGLCOMPILESHADERARBPROC              OpenGLExtensions::glCompileShaderARB              = 0;
PFNGLCREATEPROGRAMOBJECTARBPROC        OpenGLExtensions::glCreateProgramObjectARB        = 0;
PFNGLATTACHOBJECTARBPROC               OpenGLExtensions::glAttachObjectARB               = 0;
PFNGLLINKPROGRAMARBPROC                OpenGLExtensions::glLinkProgramARB                = 0;
PFNGLUSEPROGRAMOBJECTARBPROC           OpenGLExtensions::glUseProgramObjectARB           = 0;
PFNGLVALIDATEPROGRAMARBPROC            OpenGLExtensions::glValidateProgramARB            = 0;
PFNGLUNIFORM1FARBPROC                  OpenGLExtensions::glUniform1fARB                  = 0;
PFNGLUNIFORM2FARBPROC                  OpenGLExtensions::glUniform2fARB                  = 0;
PFNGLUNIFORM3FARBPROC                  OpenGLExtensions::glUniform3fARB                  = 0;
PFNGLUNIFORM4FARBPROC                  OpenGLExtensions::glUniform4fARB                  = 0;
PFNGLUNIFORM1IARBPROC                  OpenGLExtensions::glUniform1iARB                  = 0;
PFNGLUNIFORM2IARBPROC                  OpenGLExtensions::glUniform2iARB                  = 0;
PFNGLUNIFORM3IARBPROC                  OpenGLExtensions::glUniform3iARB                  = 0;
PFNGLUNIFORM4IARBPROC                  OpenGLExtensions::glUniform4iARB                  = 0;
PFNGLUNIFORM1FVARBPROC                 OpenGLExtensions::glUniform1fvARB                 = 0;
PFNGLUNIFORM2FVARBPROC                 OpenGLExtensions::glUniform2fvARB                 = 0;
PFNGLUNIFORM3FVARBPROC                 OpenGLExtensions::glUniform3fvARB                 = 0;
PFNGLUNIFORM4FVARBPROC                 OpenGLExtensions::glUniform4fvARB                 = 0;
PFNGLUNIFORM1IVARBPROC                 OpenGLExtensions::glUniform1ivARB                 = 0;
PFNGLUNIFORM2IVARBPROC                 OpenGLExtensions::glUniform2ivARB                 = 0;
PFNGLUNIFORM3IVARBPROC                 OpenGLExtensions::glUniform3ivARB                 = 0;
PFNGLUNIFORM4IVARBPROC                 OpenGLExtensions::glUniform4ivARB                 = 0;
PFNGLUNIFORMMATRIX2FVARBPROC           OpenGLExtensions::glUniformMatrix2fvARB           = 0;
PFNGLUNIFORMMATRIX3FVARBPROC           OpenGLExtensions::glUniformMatrix3fvARB           = 0;
PFNGLUNIFORMMATRIX4FVARBPROC           OpenGLExtensions::glUniformMatrix4fvARB           = 0;
PFNGLGETOBJECTPARAMETERFVARBPROC       OpenGLExtensions::glGetObjectParameterfvARB       = 0;
PFNGLGETOBJECTPARAMETERIVARBPROC       OpenGLExtensions::glGetObjectParameterivARB       = 0;
PFNGLGETINFOLOGARBPROC                 OpenGLExtensions::glGetInfoLogARB                 = 0;
PFNGLGETATTACHEDOBJECTSARBPROC         OpenGLExtensions::glGetAttachedObjectsARB         = 0;
PFNGLGETUNIFORMLOCATIONARBPROC         OpenGLExtensions::glGetUniformLocationARB         = 0;
PFNGLGETACTIVEUNIFORMARBPROC           OpenGLExtensions::glGetActiveUniformARB           = 0;
PFNGLGETUNIFORMFVARBPROC               OpenGLExtensions::glGetUniformfvARB               = 0;
PFNGLGETUNIFORMIVARBPROC               OpenGLExtensions::glGetUniformivARB               = 0;
PFNGLGETSHADERSOURCEARBPROC            OpenGLExtensions::glGetShaderSourceARB            = 0;
#endif // !ES1



}
