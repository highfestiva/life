
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games

#include "../Include/UiOpenGLExtensions.h"

namespace UiLepra
{

bool OpenGLExtensions::smGLVersion14                   = false;
bool OpenGLExtensions::smGLVersion15                   = false;
bool OpenGLExtensions::smFrameBufferObjectsSupported   = false;
bool OpenGLExtensions::smBufferObjectsSupported        = false;
bool OpenGLExtensions::smAnisotropicFilteringSupported = false;
bool OpenGLExtensions::smCompressedTexturesSupported   = false;
bool OpenGLExtensions::smMultiTextureSupported         = false;
bool OpenGLExtensions::smShaderProgramsSupported       = false;
bool OpenGLExtensions::smShadersSupported              = false;
bool OpenGLExtensions::smVSyncSupported                = false;

float OpenGLExtensions::smMaxAnisotropy                = 1.0f;

// Declare functions.

#ifdef LEPRA_WINDOWS
PFNWGLSWAPINTERVALEXTPROC    OpenGLExtensions::wglSwapIntervalEXT    = 0;
PFNWGLGETSWAPINTERVALEXTPROC OpenGLExtensions::wglGetSwapIntervalEXT = 0;
#endif

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


PFNGLBINDBUFFERPROC       OpenGLExtensions::glBindBuffer       = 0;
PFNGLBUFFERDATAPROC       OpenGLExtensions::glBufferData       = 0;
PFNGLBUFFERSUBDATAPROC    OpenGLExtensions::glBufferSubData    = 0;
PFNGLDELETEBUFFERSPROC    OpenGLExtensions::glDeleteBuffers    = 0;
PFNGLGENBUFFERSPROC       OpenGLExtensions::glGenBuffers       = 0;
PFNGLMAPBUFFERPROC        OpenGLExtensions::glMapBuffer        = 0;
PFNGLUNMAPBUFFERPROC      OpenGLExtensions::glUnmapBuffer      = 0;

PFNGLACTIVETEXTUREPROC    OpenGLExtensions::glActiveTexture    = 0;
PFNGLCLIENTACTIVETEXTUREPROC OpenGLExtensions::glClientActiveTexture = 0;
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

bool OpenGLExtensions::IsExtensionSupported(const char* pExtension)
{
	astr lExtension(pExtension);
	astrutil::StripWhiteSpaces(lExtension);

	if (lExtension.length() == 0)
		return false;

	astr lGLExtensions((char*)glGetString(GL_EXTENSIONS));
	return (lGLExtensions.find(lExtension.c_str(), 0) >= 0);
}

void* OpenGLExtensions::GetExtensionPointer(const char* pFunctionName)
{
#ifdef LEPRA_WINDOWS
    return (void*)wglGetProcAddress(pFunctionName);
#endif // LEPRA_WINDOWS

#ifdef LEPRA_MACOSX
    // Mac is a bit more tricky.
    // First we need the bundle
    CFBundleRef openGL = 0;
    SInt16      fwVersion = 0;
    SInt32      fwDir = 0;
    
    if(FindFolder(kSystemDomain, kFrameworksFolderType, kDontCreateFolder, &fwVersion, &fwDir) != noErr)
        return NULL;
        
    FSSpec fSpec;
    FSRef  fRef;
    if(FSMakeFSSpec(fwVersion, fwDir, "\pOpenGL.framework", &fSpec) != noErr)
        return NULL;
        
    FSpMakeFSRef(&fSpec, &fRef);
    CFURLRef url = CFURLCreateFromFSRef(kCFAllocatorDefault, &fRef);
    if(!url)
        return NULL;
        
    openGL = CFBundleCreate(kCFAllocatorDefault, url);
    CFRelease(url);
    
    // Then load the function pointer from the bundle
    CFStringRef string = CFStringCreateWithCString(kCFAllocatorDefault, pFunctionName, kCFStringEncodingMacRoman);
    void *pFunc = CFBundleGetFunctionPointerForName(openGL, string);
    
    // Release the bundle and string
    CFRelease(string);
    CFRelease(openGL);
    
    // Return the function ponter
    return pFunc;
#endif // LEPRA_MACOSX
}

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
			smGLVersion14 = true;
			smGLVersion15 = true;
		}
		else if(lVersion[1] == '.' && 
			lVersion[2] >= '4' && 
			lVersion[2] <= '9')
		{
			smGLVersion14 = true;
		}
	}
	else if(lVersion[0] >= '2' && lVersion[0] <= '9')
	{
		// This must be an OpenGL version way higher than the versions
		// that existed when this was written. Assume backward compatibility.
		smGLVersion14 = true;
		smGLVersion15 = true;
	}


	/*
		Init Frame Buffer Objects...
	*/
	if (IsExtensionSupported("GL_EXT_framebuffer_object") == true)
	{
		smFrameBufferObjectsSupported = true;
	}

	if (smFrameBufferObjectsSupported == true)
	{
		glIsRenderbufferEXT                      = (PFNGLISRENDERBUFFEREXTPROC)                      GetExtensionPointer("glIsRenderbufferEXT");
		glBindRenderbufferEXT                    = (PFNGLBINDRENDERBUFFEREXTPROC)                    GetExtensionPointer("glBindRenderbufferEXT");
		glDeleteRenderbuffersEXT                 = (PFNGLDELETERENDERBUFFERSEXTPROC)                 GetExtensionPointer("glDeleteRenderbuffersEXT");
		glGenRenderbuffersEXT                    = (PFNGLGENRENDERBUFFERSEXTPROC)                    GetExtensionPointer("glGenRenderbuffersEXT");
		glRenderbufferStorageEXT                 = (PFNGLRENDERBUFFERSTORAGEEXTPROC)                 GetExtensionPointer("glRenderbufferStorageEXT");
		glGetRenderbufferParameterivEXT          = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)          GetExtensionPointer("glGetRenderbufferParameterivEXT");
		glIsFramebufferEXT                       = (PFNGLISFRAMEBUFFEREXTPROC)                       GetExtensionPointer("glIsFramebufferEXT");
		glBindFramebufferEXT                     = (PFNGLBINDFRAMEBUFFEREXTPROC)                     GetExtensionPointer("glBindFramebufferEXT");
		glDeleteFramebuffersEXT                  = (PFNGLDELETEFRAMEBUFFERSEXTPROC)                  GetExtensionPointer("glDeleteFramebuffersEXT");
		glGenFramebuffersEXT                     = (PFNGLGENFRAMEBUFFERSEXTPROC)                     GetExtensionPointer("glGenFramebuffersEXT");
		glCheckFramebufferStatusEXT              = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)              GetExtensionPointer("glCheckFramebufferStatusEXT");
		glFramebufferTexture1DEXT                = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)                GetExtensionPointer("glFramebufferTexture1DEXT");
		glFramebufferTexture2DEXT                = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)                GetExtensionPointer("glFramebufferTexture2DEXT");
		glFramebufferTexture3DEXT                = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)                GetExtensionPointer("glFramebufferTexture3DEXT");
		glFramebufferRenderbufferEXT             = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)             GetExtensionPointer("glFramebufferRenderbufferEXT");
		glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) GetExtensionPointer("glGetFramebufferAttachmentParameterivEXT");
		glGenerateMipmapEXT                      = (PFNGLGENERATEMIPMAPEXTPROC)                      GetExtensionPointer("glGenerateMipmapEXT");

		if (CheckFrameBufferObjectFunctions() == false)
		{
			ClearFrameBufferObjectFunctions();
			smFrameBufferObjectsSupported = false;
		}
	}

	/*
		Init buffer objects.
	*/
	if (smGLVersion15 == true || IsExtensionSupported("GL_ARB_vertex_buffer_object") == true)
	{
		smBufferObjectsSupported = true;
	}

	if (smGLVersion15 == true)
	{
		glBindBuffer    = (PFNGLBINDBUFFERPROC)   GetExtensionPointer("glBindBuffer");
		glBufferData    = (PFNGLBUFFERDATAPROC)   GetExtensionPointer("glBufferData");
		glBufferSubData = (PFNGLBUFFERSUBDATAPROC)GetExtensionPointer("glBufferSubData");
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GetExtensionPointer("glDeleteBuffers");
		glGenBuffers    = (PFNGLGENBUFFERSPROC)   GetExtensionPointer("glGenBuffers");
		glMapBuffer     = (PFNGLMAPBUFFERPROC)    GetExtensionPointer("glMapBuffer");
		glUnmapBuffer   = (PFNGLUNMAPBUFFERPROC)  GetExtensionPointer("glUnmapBuffer");
	}
	if(smBufferObjectsSupported == true && CheckBufferObjectFunctions() == false)
	{
		// Retry with the ARB version...
		glBindBuffer    = (PFNGLBINDBUFFERPROC)   GetExtensionPointer("glBindBufferARB");
		glBufferData    = (PFNGLBUFFERDATAPROC)   GetExtensionPointer("glBufferDataARB");
		glBufferSubData = (PFNGLBUFFERSUBDATAPROC)GetExtensionPointer("glBufferSubDataARB");
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GetExtensionPointer("glDeleteBuffersARB");
		glGenBuffers    = (PFNGLGENBUFFERSPROC)   GetExtensionPointer("glGenBuffersARB");
		glMapBuffer     = (PFNGLMAPBUFFERPROC)    GetExtensionPointer("glMapBufferARB");
		glUnmapBuffer   = (PFNGLUNMAPBUFFERPROC)  GetExtensionPointer("glUnmapBufferARB");
	}

	if (smBufferObjectsSupported == true)
	{
		if (CheckBufferObjectFunctions() == false)
		{
			ClearBufferObjectFunctions();
			smBufferObjectsSupported = false;
		}
	}

	/*
		Init anisotropic filtering...
	*/

	if (IsExtensionSupported("GL_EXT_texture_filter_anisotropic") == true)
	{
		smAnisotropicFilteringSupported = true;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &smMaxAnisotropy);
	}

	/*
		Init texture compression.
	*/

	if (IsExtensionSupported("GL_ARB_texture_compression") == true)
	{
		smCompressedTexturesSupported = true;
		glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
	}

#ifdef LEPRA_WINDOWS
	// Init vsync on/off.
	if (IsExtensionSupported("WGL_EXT_swap") == true)
	{
		smVSyncSupported = true;

		wglSwapIntervalEXT    = (PFNWGLSWAPINTERVALEXTPROC)    GetExtensionPointer("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) GetExtensionPointer("wglGetSwapIntervalEXT");

		if (wglSwapIntervalEXT    == 0 ||
		   wglGetSwapIntervalEXT == 0)
		{
			smVSyncSupported = false;

			wglSwapIntervalEXT    = 0;
			wglGetSwapIntervalEXT = 0;
		}
	}
#else
#pragma message("Warning: In OpenGLExtensions::InitExtensions(), VSync is not initialized.")
#endif

	/*
		Init multi texture support.
	*/
	if (IsExtensionSupported("GL_ARB_multitexture") == true)
	{
		smMultiTextureSupported = true;

		glActiveTexture       = (PFNGLACTIVETEXTUREPROC)       GetExtensionPointer("glActiveTextureARB");
		glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC) GetExtensionPointer("glClientActiveTexture");
		glMultiTexCoord1d     = (PFNGLMULTITEXCOORD1DPROC)     GetExtensionPointer("glMultiTexCoord1dARB");
		glMultiTexCoord1dv    = (PFNGLMULTITEXCOORD1DVPROC)    GetExtensionPointer("glMultiTexCoord1dvARB");
		glMultiTexCoord1f     = (PFNGLMULTITEXCOORD1FPROC)     GetExtensionPointer("glMultiTexCoord1fARB");
		glMultiTexCoord1fv    = (PFNGLMULTITEXCOORD1FVPROC)    GetExtensionPointer("glMultiTexCoord1fvARB");
		glMultiTexCoord1i     = (PFNGLMULTITEXCOORD1IPROC)     GetExtensionPointer("glMultiTexCoord1iARB");
		glMultiTexCoord1iv    = (PFNGLMULTITEXCOORD1IVPROC)    GetExtensionPointer("glMultiTexCoord1ivARB");
		glMultiTexCoord1s     = (PFNGLMULTITEXCOORD1SPROC)     GetExtensionPointer("glMultiTexCoord1sARB");
		glMultiTexCoord1sv    = (PFNGLMULTITEXCOORD1SVPROC)    GetExtensionPointer("glMultiTexCoord1svARB");
		glMultiTexCoord2d     = (PFNGLMULTITEXCOORD2DPROC)     GetExtensionPointer("glMultiTexCoord2dARB");
		glMultiTexCoord2dv    = (PFNGLMULTITEXCOORD2DVPROC)    GetExtensionPointer("glMultiTexCoord2dvARB");
		glMultiTexCoord2f     = (PFNGLMULTITEXCOORD2FPROC)     GetExtensionPointer("glMultiTexCoord2fARB");
		glMultiTexCoord2fv    = (PFNGLMULTITEXCOORD2FVPROC)    GetExtensionPointer("glMultiTexCoord2fvARB");
		glMultiTexCoord2i     = (PFNGLMULTITEXCOORD2IPROC)     GetExtensionPointer("glMultiTexCoord2iARB");
		glMultiTexCoord2iv    = (PFNGLMULTITEXCOORD2IVPROC)    GetExtensionPointer("glMultiTexCoord2ivARB");
		glMultiTexCoord2s     = (PFNGLMULTITEXCOORD2SPROC)     GetExtensionPointer("glMultiTexCoord2sARB");
		glMultiTexCoord2sv    = (PFNGLMULTITEXCOORD2SVPROC)    GetExtensionPointer("glMultiTexCoord2svARB");
		glMultiTexCoord3d     = (PFNGLMULTITEXCOORD3DPROC)     GetExtensionPointer("glMultiTexCoord3dARB");
		glMultiTexCoord3dv    = (PFNGLMULTITEXCOORD3DVPROC)    GetExtensionPointer("glMultiTexCoord3dvARB");
		glMultiTexCoord3f     = (PFNGLMULTITEXCOORD3FPROC)     GetExtensionPointer("glMultiTexCoord3fARB");
		glMultiTexCoord3fv    = (PFNGLMULTITEXCOORD3FVPROC)    GetExtensionPointer("glMultiTexCoord3fvARB");
		glMultiTexCoord3i     = (PFNGLMULTITEXCOORD3IPROC)     GetExtensionPointer("glMultiTexCoord3iARB");
		glMultiTexCoord3iv    = (PFNGLMULTITEXCOORD3IVPROC)    GetExtensionPointer("glMultiTexCoord3ivARB");
		glMultiTexCoord3s     = (PFNGLMULTITEXCOORD3SPROC)     GetExtensionPointer("glMultiTexCoord3sARB");
		glMultiTexCoord3sv    = (PFNGLMULTITEXCOORD3SVPROC)    GetExtensionPointer("glMultiTexCoord3svARB");
		glMultiTexCoord4d     = (PFNGLMULTITEXCOORD4DPROC)     GetExtensionPointer("glMultiTexCoord4dARB");
		glMultiTexCoord4dv    = (PFNGLMULTITEXCOORD4DVPROC)    GetExtensionPointer("glMultiTexCoord4dvARB");
		glMultiTexCoord4f     = (PFNGLMULTITEXCOORD4FPROC)     GetExtensionPointer("glMultiTexCoord4fARB");
		glMultiTexCoord4fv    = (PFNGLMULTITEXCOORD4FVPROC)    GetExtensionPointer("glMultiTexCoord4fvARB");
		glMultiTexCoord4i     = (PFNGLMULTITEXCOORD4IPROC)     GetExtensionPointer("glMultiTexCoord4iARB");
		glMultiTexCoord4iv    = (PFNGLMULTITEXCOORD4IVPROC)    GetExtensionPointer("glMultiTexCoord4ivARB");
		glMultiTexCoord4s     = (PFNGLMULTITEXCOORD4SPROC)     GetExtensionPointer("glMultiTexCoord4sARB");
		glMultiTexCoord4sv    = (PFNGLMULTITEXCOORD4SVPROC)    GetExtensionPointer("glMultiTexCoord4svARB");

		if (CheckMultiTextureFunctions() == false)
		{
			smMultiTextureSupported = false;
			ClearMultiTextureFunctions();
		}
	}

	/*
		Init vertex and pixel shader support.
	*/
	if (IsExtensionSupported("GL_ARB_vertex_program") == true &&
	   IsExtensionSupported("GL_ARB_fragment_program") == true)
	{
		smShaderProgramsSupported = true;

		glGenProgramsARB                = (PFNGLGENPROGRAMSARBPROC)                GetExtensionPointer("glGenProgramsARB");
		glBindProgramARB                = (PFNGLBINDPROGRAMARBPROC)                GetExtensionPointer("glBindProgramARB");
		glDeleteProgramsARB             = (PFNGLDELETEPROGRAMSARBPROC)             GetExtensionPointer("glDeleteProgramsARB");
		glIsProgramARB                  = (PFNGLISPROGRAMARBPROC)                  GetExtensionPointer("glIsProgramARB");
		glProgramStringARB              = (PFNGLPROGRAMSTRINGARBPROC)              GetExtensionPointer("glProgramStringARB");
		glGetProgramStringARB           = (PFNGLGETPROGRAMIVARBPROC)               GetExtensionPointer("glGetProgramStringARB");
		glEnableVertexAttribArrayARB    = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)    GetExtensionPointer("glEnableVertexAttribArrayARB");
		glDisableVertexAttribArrayARB   = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)   GetExtensionPointer("glDisableVertexAttribArrayARB");
		glGetProgramEnvParameterdvARB   = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)   GetExtensionPointer("glGetProgramEnvParameterdvARB");
		glGetProgramEnvParameterfvARB   = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)   GetExtensionPointer("glGetProgramEnvParameterfvARB");
		glProgramEnvParameter4dARB      = (PFNGLPROGRAMENVPARAMETER4DARBPROC)      GetExtensionPointer("glProgramEnvParameter4dARB");
		glProgramEnvParameter4dvARB     = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)     GetExtensionPointer("glProgramEnvParameter4dvARB");
		glProgramEnvParameter4fARB      = (PFNGLPROGRAMENVPARAMETER4FARBPROC)      GetExtensionPointer("glProgramEnvParameter4fARB");
		glProgramEnvParameter4fvARB     = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)     GetExtensionPointer("glProgramEnvParameter4fvARB");
		glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) GetExtensionPointer("glGetProgramLocalParameterdvARB");
		glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) GetExtensionPointer("glGetProgramLocalParameterfvARB");
		glProgramLocalParameter4dARB    = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)    GetExtensionPointer("glProgramLocalParameter4dARB");
		glProgramLocalParameter4dvARB   = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)   GetExtensionPointer("glProgramLocalParameter4dvARB");
		glProgramLocalParameter4fARB    = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)    GetExtensionPointer("glProgramLocalParameter4fARB");
		glProgramLocalParameter4fvARB   = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)   GetExtensionPointer("glProgramLocalParameter4fvARB");
		glGetProgramivARB               = (PFNGLGETPROGRAMIVARBPROC)               GetExtensionPointer("glGetProgramivARB");
		glGetVertexAttribdvARB          = (PFNGLGETVERTEXATTRIBDVARBPROC)          GetExtensionPointer("glGetVertexAttribdvARB");
		glGetVertexAttribfvARB          = (PFNGLGETVERTEXATTRIBFVARBPROC)          GetExtensionPointer("glGetVertexAttribfvARB");
		glGetVertexAttribivARB          = (PFNGLGETVERTEXATTRIBIVARBPROC)          GetExtensionPointer("glGetVertexAttribivARB");
		glGetVertexAttribPointervARB    = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)    GetExtensionPointer("glGetVertexAttribPointervARB");
		glVertexAttribPointerARB        = (PFNGLVERTEXATTRIBPOINTERARBPROC)        GetExtensionPointer("glVertexAttribPointerARB");
		glVertexAttrib1dARB             = (PFNGLVERTEXATTRIB1DARBPROC)             GetExtensionPointer("glVertexAttrib1dARB");
		glVertexAttrib1dvARB            = (PFNGLVERTEXATTRIB1DVARBPROC)            GetExtensionPointer("glVertexAttrib1dvARB");
		glVertexAttrib1fARB             = (PFNGLVERTEXATTRIB1FARBPROC)             GetExtensionPointer("glVertexAttrib1fARB");
		glVertexAttrib1fvARB            = (PFNGLVERTEXATTRIB1FVARBPROC)            GetExtensionPointer("glVertexAttrib1fvARB");
		glVertexAttrib1sARB             = (PFNGLVERTEXATTRIB1SARBPROC)             GetExtensionPointer("glVertexAttrib1sARB");
		glVertexAttrib1svARB            = (PFNGLVERTEXATTRIB1SVARBPROC)            GetExtensionPointer("glVertexAttrib1svARB");
		glVertexAttrib2dARB             = (PFNGLVERTEXATTRIB2DARBPROC)             GetExtensionPointer("glVertexAttrib2dARB");
		glVertexAttrib2dvARB            = (PFNGLVERTEXATTRIB2DVARBPROC)            GetExtensionPointer("glVertexAttrib2dvARB");
		glVertexAttrib2fARB             = (PFNGLVERTEXATTRIB2FARBPROC)             GetExtensionPointer("glVertexAttrib2fARB");
		glVertexAttrib2fvARB            = (PFNGLVERTEXATTRIB2FVARBPROC)            GetExtensionPointer("glVertexAttrib2fvARB");
		glVertexAttrib2sARB             = (PFNGLVERTEXATTRIB2SARBPROC)             GetExtensionPointer("glVertexAttrib2sARB");
		glVertexAttrib2svARB            = (PFNGLVERTEXATTRIB2SVARBPROC)            GetExtensionPointer("glVertexAttrib2svARB");
		glVertexAttrib3dARB             = (PFNGLVERTEXATTRIB3DARBPROC)             GetExtensionPointer("glVertexAttrib3dARB");
		glVertexAttrib3dvARB            = (PFNGLVERTEXATTRIB3DVARBPROC)            GetExtensionPointer("glVertexAttrib3dvARB");
		glVertexAttrib3fARB             = (PFNGLVERTEXATTRIB3FARBPROC)             GetExtensionPointer("glVertexAttrib3fARB");
		glVertexAttrib3fvARB            = (PFNGLVERTEXATTRIB3FVARBPROC)            GetExtensionPointer("glVertexAttrib3fvARB");
		glVertexAttrib3sARB             = (PFNGLVERTEXATTRIB3SARBPROC)             GetExtensionPointer("glVertexAttrib3sARB");
		glVertexAttrib3svARB            = (PFNGLVERTEXATTRIB3SVARBPROC)            GetExtensionPointer("glVertexAttrib3svARB");
		glVertexAttrib4NbvARB           = (PFNGLVERTEXATTRIB4NBVARBPROC)           GetExtensionPointer("glVertexAttrib4NbvARB");
		glVertexAttrib4NivARB           = (PFNGLVERTEXATTRIB4NIVARBPROC)           GetExtensionPointer("glVertexAttrib4NivARB");
		glVertexAttrib4NsvARB           = (PFNGLVERTEXATTRIB4NSVARBPROC)           GetExtensionPointer("glVertexAttrib4NsvARB");
		glVertexAttrib4NubARB           = (PFNGLVERTEXATTRIB4NUBARBPROC)           GetExtensionPointer("glVertexAttrib4NubARB");
		glVertexAttrib4NubvARB          = (PFNGLVERTEXATTRIB4NUBVARBPROC)          GetExtensionPointer("glVertexAttrib4NubvARB");
		glVertexAttrib4NuivARB          = (PFNGLVERTEXATTRIB4NUIVARBPROC)          GetExtensionPointer("glVertexAttrib4NuivARB");
		glVertexAttrib4NusvARB          = (PFNGLVERTEXATTRIB4NUSVARBPROC)          GetExtensionPointer("glVertexAttrib4NusvARB");
		glVertexAttrib4bvARB            = (PFNGLVERTEXATTRIB4BVARBPROC)            GetExtensionPointer("glVertexAttrib4bvARB");
		glVertexAttrib4dARB             = (PFNGLVERTEXATTRIB4DARBPROC)             GetExtensionPointer("glVertexAttrib4dARB");
		glVertexAttrib4dvARB            = (PFNGLVERTEXATTRIB4DVARBPROC)            GetExtensionPointer("glVertexAttrib4dvARB");
		glVertexAttrib4fARB             = (PFNGLVERTEXATTRIB4FARBPROC)             GetExtensionPointer("glVertexAttrib4fARB");
		glVertexAttrib4fvARB            = (PFNGLVERTEXATTRIB4FVARBPROC)            GetExtensionPointer("glVertexAttrib4fvARB");
		glVertexAttrib4ivARB            = (PFNGLVERTEXATTRIB4IVARBPROC)            GetExtensionPointer("glVertexAttrib4ivARB");
		glVertexAttrib4sARB             = (PFNGLVERTEXATTRIB4SARBPROC)             GetExtensionPointer("glVertexAttrib4sARB");
		glVertexAttrib4svARB            = (PFNGLVERTEXATTRIB4SVARBPROC)            GetExtensionPointer("glVertexAttrib4svARB");
		glVertexAttrib4ubvARB           = (PFNGLVERTEXATTRIB4UBVARBPROC)           GetExtensionPointer("glVertexAttrib4ubvARB");
		glVertexAttrib4uivARB           = (PFNGLVERTEXATTRIB4UIVARBPROC)           GetExtensionPointer("glVertexAttrib4uivARB");
		glVertexAttrib4usvARB           = (PFNGLVERTEXATTRIB4USVARBPROC)           GetExtensionPointer("glVertexAttrib4usvARB");

		if(CheckShaderProgramFunctions() == false)
		{
			smShaderProgramsSupported = false;
			ClearShaderProgramFunctions();
		}
	}

	if (IsExtensionSupported("GL_ARB_vertex_shader")        == true &&
	   IsExtensionSupported("GL_ARB_fragment_shader")      == true &&
	   IsExtensionSupported("GL_ARB_shader_objects")       == true &&
	   IsExtensionSupported("GL_ARB_shading_language_100") == true)
	{
		smShadersSupported = true;

		glDeleteObjectARB               = (PFNGLDELETEOBJECTARBPROC)         GetExtensionPointer("glDeleteObjectARB");
		glGetHandleARB                  = (PFNGLGETHANDLEARBPROC)            GetExtensionPointer("glGetHandleARB");
		glDetachObjectARB               = (PFNGLDETACHOBJECTARBPROC)         GetExtensionPointer("glDetachObjectARB");
		glCreateShaderObjectARB         = (PFNGLCREATESHADEROBJECTARBPROC)   GetExtensionPointer("glCreateShaderObjectARB");
		glShaderSourceARB               = (PFNGLSHADERSOURCEARBPROC)         GetExtensionPointer("glShaderSourceARB");
		glCompileShaderARB              = (PFNGLCOMPILESHADERARBPROC)        GetExtensionPointer("glCompileShaderARB");
		glCreateProgramObjectARB        = (PFNGLCREATEPROGRAMOBJECTARBPROC)  GetExtensionPointer("glCreateProgramObjectARB");
		glAttachObjectARB               = (PFNGLATTACHOBJECTARBPROC)         GetExtensionPointer("glAttachObjectARB");
		glLinkProgramARB                = (PFNGLLINKPROGRAMARBPROC)          GetExtensionPointer("glLinkProgramARB");
		glUseProgramObjectARB           = (PFNGLUSEPROGRAMOBJECTARBPROC)     GetExtensionPointer("glUseProgramObjectARB");
		glValidateProgramARB            = (PFNGLVALIDATEPROGRAMARBPROC)      GetExtensionPointer("glValidateProgramARB");
		glUniform1fARB                  = (PFNGLUNIFORM1FARBPROC)            GetExtensionPointer("glUniform1fARB");
		glUniform2fARB                  = (PFNGLUNIFORM2FARBPROC)            GetExtensionPointer("glUniform2fARB");
		glUniform3fARB                  = (PFNGLUNIFORM3FARBPROC)            GetExtensionPointer("glUniform3fARB");
		glUniform4fARB                  = (PFNGLUNIFORM4FARBPROC)            GetExtensionPointer("glUniform4fARB");
		glUniform1iARB                  = (PFNGLUNIFORM1IARBPROC)            GetExtensionPointer("glUniform1iARB");
		glUniform2iARB                  = (PFNGLUNIFORM2IARBPROC)            GetExtensionPointer("glUniform2iARB");
		glUniform3iARB                  = (PFNGLUNIFORM3IARBPROC)            GetExtensionPointer("glUniform3iARB");
		glUniform4iARB                  = (PFNGLUNIFORM4IARBPROC)            GetExtensionPointer("glUniform4iARB");
		glUniform1fvARB                 = (PFNGLUNIFORM1FVARBPROC)           GetExtensionPointer("glUniform1fvARB");
		glUniform2fvARB                 = (PFNGLUNIFORM2FVARBPROC)           GetExtensionPointer("glUniform2fvARB");
		glUniform3fvARB                 = (PFNGLUNIFORM3FVARBPROC)           GetExtensionPointer("glUniform3fvARB");
		glUniform4fvARB                 = (PFNGLUNIFORM4FVARBPROC)           GetExtensionPointer("glUniform4fvARB");
		glUniform1ivARB                 = (PFNGLUNIFORM1IVARBPROC)           GetExtensionPointer("glUniform1ivARB");
		glUniform2ivARB                 = (PFNGLUNIFORM2IVARBPROC)           GetExtensionPointer("glUniform2ivARB");
		glUniform3ivARB                 = (PFNGLUNIFORM3IVARBPROC)           GetExtensionPointer("glUniform3ivARB");
		glUniform4ivARB                 = (PFNGLUNIFORM4IVARBPROC)           GetExtensionPointer("glUniform4ivARB");
		glUniformMatrix2fvARB           = (PFNGLUNIFORMMATRIX2FVARBPROC)     GetExtensionPointer("glUniformMatrix2fvARB");
		glUniformMatrix3fvARB           = (PFNGLUNIFORMMATRIX3FVARBPROC)     GetExtensionPointer("glUniformMatrix3fvARB");
		glUniformMatrix4fvARB           = (PFNGLUNIFORMMATRIX4FVARBPROC)     GetExtensionPointer("glUniformMatrix4fvARB");
		glGetObjectParameterfvARB       = (PFNGLGETOBJECTPARAMETERFVARBPROC) GetExtensionPointer("glGetObjectParameterfvARB");
		glGetObjectParameterivARB       = (PFNGLGETOBJECTPARAMETERIVARBPROC) GetExtensionPointer("glGetObjectParameterivARB");
		glGetInfoLogARB                 = (PFNGLGETINFOLOGARBPROC)           GetExtensionPointer("glGetInfoLogARB");
		glGetAttachedObjectsARB         = (PFNGLGETATTACHEDOBJECTSARBPROC)   GetExtensionPointer("glGetAttachedObjectsARB");
		glGetUniformLocationARB         = (PFNGLGETUNIFORMLOCATIONARBPROC)   GetExtensionPointer("glGetUniformLocationARB");
		glGetActiveUniformARB           = (PFNGLGETACTIVEUNIFORMARBPROC)     GetExtensionPointer("glGetActiveUniformARB");
		glGetUniformfvARB               = (PFNGLGETUNIFORMFVARBPROC)         GetExtensionPointer("glGetUniformfvARB");
		glGetUniformivARB               = (PFNGLGETUNIFORMIVARBPROC)         GetExtensionPointer("glGetUniformivARB");
		glGetShaderSourceARB            = (PFNGLGETSHADERSOURCEARBPROC)      GetExtensionPointer("glGetShaderSourceARB");

		if (CheckShaderFunctions() == false)
		{
			smShadersSupported = false;
			ClearShaderFunctions();
		}
	}
}

void OpenGLExtensions::ClearFrameBufferObjectFunctions()
{
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
}

void OpenGLExtensions::ClearShaderProgramFunctions()
{
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
}

void OpenGLExtensions::ClearShaderFunctions()
{
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
}

bool OpenGLExtensions::CheckFrameBufferObjectFunctions()
{
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
}

bool OpenGLExtensions::CheckShaderProgramFunctions()
{
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
}

bool OpenGLExtensions::CheckShaderFunctions()
{
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
}

void OpenGLExtensions::SetAnisotropy(float pAmountAnisotropy)
{
	if (smAnisotropicFilteringSupported == true)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, pAmountAnisotropy);
	}
}

bool OpenGLExtensions::SetVSyncEnabled(bool pEnabled)
{
#ifdef LEPRA_WINDOWS
	if (wglSwapIntervalEXT != 0)
	{
		return (wglSwapIntervalEXT(pEnabled ? 1 : 0) != FALSE);
	}
#else
#pragma message("Warning: OpenGLExtensions::SetVSyncEnabled() is not implemented.")
#endif
	return (false);
}

} // End namespace.
