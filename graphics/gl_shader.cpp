/*
 * gl_shader.cpp
 *
 *  Created on: 24.05.2012
 *
 */
#include "graphics/gl_extensions.h"

#include "graphics/gl_shader.h"
#include <stdio.h>
#include <map>

#include "config.h"
#include "hacks.h"
#include "debug.h"


namespace {

	char* filetobuf( const char* file )
	{
		FILE *fptr;
		long length;
		char *buf;

		fptr = fopen(file, "rb"); /* Open file for reading */
		if (!fptr) /* Return NULL on failure */
			return NULL;
		fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
		length = ftell(fptr); /* Find out how many bytes into the file we are */
		buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
		fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
		fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
		fclose(fptr); /* Close the file */
		buf[length] = 0; /* Null terminator */

		return buf; /* Return the buffer */
	}


	GLuint createProgram( std::string filename )
	{
	    GLint IsCompiled_VS, IsCompiled_FS;
	    int IsLinked;
	    int maxLength;
	    char* vertexInfoLog;
	    char* fragmentInfoLog;
	    char* shaderProgramInfoLog;

	    /* These pointers will receive the contents of our shader source code files */
	    GLchar* vertexsource;
	    GLchar* fragmentsource;

	    /* These are handles used to reference the shaders */
	    GLuint vertexshader, fragmentshader;

	    /* This is a handle to the shader program */
	    GLuint shaderprogram;

	    /* Read our shaders into the appropriate buffers */
	    vertexsource = (GLchar*)filetobuf( ( filename + ".vert").c_str() );
	    fragmentsource = (GLchar*)filetobuf( ( filename + ".frag").c_str() );

	    /* Create an empty vertex shader handle */
	    vertexshader = glCreateShader( GL_VERTEX_SHADER );

	    /* Send the vertex shader source code to GL */
	    /* Note that the source code is NULL character terminated. */
	    /* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
	    glShaderSource( vertexshader, 1, (const GLchar**)&vertexsource, 0 );

	    /* Compile the vertex shader */
	    glCompileShader( vertexshader );

	    glGetShaderiv( vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS );
	    if( IsCompiled_VS == GL_FALSE ){
	       glGetShaderiv( vertexshader, GL_INFO_LOG_LENGTH, &maxLength );

	       /* The maxLength includes the NULL character */
	       vertexInfoLog = (char *)malloc( maxLength );

	       glGetShaderInfoLog( vertexshader, maxLength, &maxLength, vertexInfoLog );
	       Debug::debug( Debug::GRAPHICS, "Vertex shader " + filename + ": " + std::string(vertexInfoLog) );

	       /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
	       /* In this simple program, we'll just leave */
	       free(vertexInfoLog);
	       return 0;
	    }

	    /* Create an empty fragment shader handle */
	    fragmentshader = glCreateShader( GL_FRAGMENT_SHADER );

	    /* Send the fragment shader source code to GL */
	    /* Note that the source code is NULL character terminated. */
	    /* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
	    glShaderSource( fragmentshader, 1, (const GLchar**)&fragmentsource, 0 );

	    /* Compile the fragment shader */
	    glCompileShader( fragmentshader );

	    glGetShaderiv( fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS );
	    if( IsCompiled_FS == GL_FALSE ){
	       glGetShaderiv( fragmentshader, GL_INFO_LOG_LENGTH, &maxLength );

	       /* The maxLength includes the NULL character */
	       fragmentInfoLog = (char *)malloc(maxLength);

	       glGetShaderInfoLog( fragmentshader, maxLength, &maxLength, fragmentInfoLog );
	       Debug::debug( Debug::GRAPHICS, "Fragment shader " + filename + ": " + std::string(fragmentInfoLog) );

	       /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
	       /* In this simple program, we'll just leave */
	       free( fragmentInfoLog );
	       return 0;
	    }

	    /* If we reached this point it means the vertex and fragment shaders compiled and are syntax error free. */
	    /* We must link them together to make a GL shader program */
	    /* GL shader programs are monolithic. It is a single piece made of 1 vertex shader and 1 fragment shader. */
	    /* Assign our program handle a "name" */
	    shaderprogram = glCreateProgram();

	    /* Attach our shaders to our program */
	    glAttachShader( shaderprogram, vertexshader );
	    glAttachShader( shaderprogram, fragmentshader );

	    /* Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color */
	    /* Attribute locations must be setup before calling glLinkProgram. */
	    glBindAttribLocation( shaderprogram, 0, "in_Position" );
	    glBindAttribLocation( shaderprogram, 1, "in_Color" );

	    /* Link our program */
	    /* At this stage, the vertex and fragment programs are inspected, optimized and a binary code is generated for the shader. */
	    /* The binary code is uploaded to the GPU, if there is no error. */
	    glLinkProgram( shaderprogram );

	    /* Again, we must check and make sure that it linked. If it fails, it would mean either there is a mismatch between the vertex */
	    /* and fragment shaders. It might be that you have surpassed your GPU's abilities. Perhaps too many ALU operations or */
	    /* too many texel fetch instructions or too many interpolators or dynamic loops. */

	    glGetProgramiv( shaderprogram, GL_LINK_STATUS, (int *)&IsLinked );
	    if( IsLinked == GL_FALSE ){
	       /* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
	       glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);

	       /* The maxLength includes the NULL character */
	       shaderProgramInfoLog = (char *)malloc(maxLength);

	       /* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
	       glGetProgramInfoLog( shaderprogram, maxLength, &maxLength, shaderProgramInfoLog );
	       Debug::debug( Debug::GRAPHICS, "Shader program " + filename + ": " + std::string(shaderProgramInfoLog) );

	       /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
	       /* In this simple program, we'll just leave */
	       free( shaderProgramInfoLog );
	       return 0;
	    }

	    /* Cleanup all the things we bound and allocated */

	    //glDetachShader(shaderprogram, vertexshader);
	    //glDetachShader(shaderprogram, fragmentshader);
	    //glDeleteProgram(shaderprogram);
	    //glDeleteShader(vertexshader);
	    //glDeleteShader(fragmentshader);
	    free(vertexsource);
	    free(fragmentsource);

	    return shaderprogram;
	}


	std::map< std::string, GLuint > shaders;
}


GLuint Shaders::getProgram( std::string filename )
{
	std::map< std::string, GLuint >::iterator fit = shaders.find( filename );
	if( fit != shaders.end() )
		return fit->second;
	extern MainConfig conf;
	GLuint prog = createProgram( conf.shadersPath + filename );
	if( prog )
		shaders[filename] = prog;
	return prog;
}



