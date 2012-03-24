/*
 * gl_extensions.cpp
 *
 *  Created on: 21.03.2012
 */

#include "gl_extensions.h"
#include "debug.h"

bool GLExtensions::load( )
{
#ifdef LOCAL_EXTENSIONS

#ifdef GL_ARB_vertex_buffer_object
		glGenBuffers = (PFNGLGENBUFFERSARBPROC)SDL_GL_GetProcAddress("glGenBuffersARB");
		glBindBuffer = (PFNGLBINDBUFFERARBPROC)SDL_GL_GetProcAddress("glBindBufferARB");
		glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)SDL_GL_GetProcAddress("glDeleteBuffersARB");
		glBufferData = (PFNGLBUFFERDATAARBPROC)SDL_GL_GetProcAddress("glBufferDataARB");
		//glBufferSubData = (PFNGLBUFFERSUBDATAARBPROC)SDL_GL_GetProcAddress("glBufferSubDataARB");
		if( glGenBuffers || glBindBuffer || glDeleteBuffers || glBufferData ){
			debug( dbg_level::GRAPHICS, "One or more GL_ARB_vertex_buffer_object functions were not found.\n" );
			return false;
		}
#else
		debug( dbg_level::GRAPHICS, "No vbo support.\n" );
		return false;
#endif

#ifdef GL_EXT_framebuffer_object
	// Load FramebufferEXT
	//glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)SDL_GL_GetProcAddress("glIsRenderbufferEXT");
	//glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)SDL_GL_GetProcAddress("glBindRenderbufferEXT");
	//glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)SDL_GL_GetProcAddress("glDeleteRenderbuffersEXT");
	//glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)SDL_GL_GetProcAddress("glGenRenderbuffersEXT");
	//glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)SDL_GL_GetProcAddress("glRenderbufferStorageEXT");
	//glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)SDL_GL_GetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)SDL_GL_GetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)SDL_GL_GetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)SDL_GL_GetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
	//glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)SDL_GL_GetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
	//glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)SDL_GL_GetProcAddress("glFramebufferTexture3DEXT");
	//glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)SDL_GL_GetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	//glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)SDL_GL_GetProcAddress("glGenerateMipmapEXT");

	if( //!glIsRenderbufferEXT || !glBindRenderbufferEXT || !glDeleteRenderbuffersEXT ||
		//!glGenRenderbuffersEXT || !glRenderbufferStorageEXT || !glGetRenderbufferParameterivEXT ||
		!glIsFramebufferEXT || !glBindFramebufferEXT || !glDeleteFramebuffersEXT ||
		!glGenFramebuffersEXT || !glCheckFramebufferStatusEXT || // ! glFramebufferTexture1DEXT ||
		!glFramebufferTexture2DEXT || // !glFramebufferTexture3DEXT || !glFramebufferRenderbufferEXT||
		!glGetFramebufferAttachmentParameterivEXT // || !glGenerateMipmapEXT
		){
		debug( dbg_level::GRAPHICS, "One or more EXT_framebuffer_object functions were not found.\n" );
		return false;
	}
#else
	debug( dbg_level::GRAPHICS, "No framebuffer support.\n" );
#endif

#ifdef GL_ARB_shader_objects
	// Load shaders
	/*
	glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glCreateProgramObjectARB");
	glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)SDL_GL_GetProcAddress("glDeleteObjectARB");
	glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glUseProgramObjectARB");
	glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)SDL_GL_GetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)SDL_GL_GetProcAddress("glShaderSourceARB");
	glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)SDL_GL_GetProcAddress("glCompileShaderARB");
	glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)SDL_GL_GetProcAddress("glGetObjectParameterivARB");
	glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)SDL_GL_GetProcAddress("glAttachObjectARB");
	glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)SDL_GL_GetProcAddress("glGetInfoLogARB");
	glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)SDL_GL_GetProcAddress("glLinkProgramARB");
	glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONARBPROC)SDL_GL_GetProcAddress("glGetUniformLocationARB");
	glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)SDL_GL_GetProcAddress("glUniform4fARB");
	glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)SDL_GL_GetProcAddress("glUniform1iARB");
	glUniformMatrix4fvARB	  = (PFNGLUNIFORMMATRIX4FVARBPROC)SDL_GL_GetProcAddress("glUniformMatrix4fvARB");

	if( !glCreateProgramObjectARB || !glDeleteObjectARB || !glUseProgramObjectARB ||
		!glCreateShaderObjectARB || !glCreateShaderObjectARB || !glCompileShaderARB ||
		!glGetObjectParameterivARB || !glAttachObjectARB || !glGetInfoLogARB ||
		!glLinkProgramARB || !glGetUniformLocationARB || !glUniform4fARB ||
		!glUniform1iARB ){
		debug( dbg_level::GRAPHICS, "One or more GL_ARB_shader_objects functions were not found\n" );
		return false;
	}
	*/
#else
	debug( dbg_level::GRAPHICS, "No shaders support.\n" );
#endif

#endif

	return true;
}

