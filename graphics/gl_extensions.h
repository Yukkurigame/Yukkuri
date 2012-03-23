/*
 * extenisons.h
 *
 *  Created on: 21.03.2012
 */

#ifndef GL_EXTENISONS_H_
#define GL_EXTENISONS_H_

#include "SDL/SDL_opengl.h"

#ifdef GL_ARB_vertex_buffer_object
	extern PFNGLGENBUFFERSARBPROC glGenBuffers;
	extern PFNGLBINDBUFFERARBPROC glBindBuffer;
	extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;
	extern PFNGLBUFFERDATAARBPROC glBufferData;
	extern PFNGLBUFFERSUBDATAARBPROC glBufferSubData;
#endif

#ifdef GL_EXT_framebuffer_object
	// EXT_framebuffer_object - http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
	extern PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT;
	extern PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
	extern PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
	extern PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
	extern PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
	extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT;
	extern PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT;
	extern PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
	extern PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
	extern PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
	extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
	extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT;
	extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
	extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;
	extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
	extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
	extern PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT;
#endif

#ifdef GL_ARB_shader_objects
	// Saders, ftgg.
	extern PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB;
	extern PFNGLDELETEOBJECTARBPROC			glDeleteObjectARB;
	extern PFNGLUSEPROGRAMOBJECTARBPROC		glUseProgramObjectARB;
	extern PFNGLCREATESHADEROBJECTARBPROC	glCreateShaderObjectARB;
	extern PFNGLSHADERSOURCEARBPROC			glShaderSourceARB;
	extern PFNGLCOMPILESHADERARBPROC		glCompileShaderARB;
	extern PFNGLGETOBJECTPARAMETERIVARBPROC	glGetObjectParameterivARB;
	extern PFNGLATTACHOBJECTARBPROC			glAttachObjectARB;
	extern PFNGLGETINFOLOGARBPROC			glGetInfoLogARB;
	extern PFNGLLINKPROGRAMARBPROC			glLinkProgramARB;
	extern PFNGLGETUNIFORMLOCATIONARBPROC	glGetUniformLocationARB;
	extern PFNGLUNIFORM4FARBPROC			glUniform4fARB;
	extern PFNGLUNIFORM1IARBPROC			glUniform1fARB;
	extern PFNGLUNIFORMMATRIX4FVARBPROC		glUniformMatrix4fvARB;
#endif


namespace GLExtensions
{
	bool load();
}

#endif /* GL_EXTENISONS_H_ */
