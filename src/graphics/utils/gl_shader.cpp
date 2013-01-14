/*
 * gl_shader.cpp
 *
 *  Created on: 24.05.2012
 *
 */
#include "graphics/gl_extensions.h"

#include "graphics/utils/gl_shader.h"
#include <stdio.h>
#include <map>

#include "config.h"
#include "hacks.h"
#include "debug.h"


#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define COLOR_LOCATION 2
#define NORMAL_LOCATION 3


extern MainConfig conf;


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


	std::map< enum GLSFlags, GLuint > shaders[glpLast];


	#define MACROS_SIZE 3
	const char* macros[MACROS_SIZE] = {
		"_YNORMALS", "_YLIGHT", "_YFIXED"
	};
	const char* passes[glpLast] = {
		"main", "geometry",
	};
}


char* generate_defines( enum GLSFlags glflags )
{
	unsigned int size = sizeof(char);
	int string_size = 1;
	char* str = (char*)malloc( size );
	str[0] = '\0';
	const char* def = "#define ";

	unsigned int flag = glsFirst;
	int index = -1;
	while( flag < glsAll ){
		++index;
		flag <<= 1;
		if( ( glflags & flag ) == 0 || index > MACROS_SIZE )
			continue;

		const char* decl = macros[index];
		string_size += strlen(def) + strlen(decl) + 1;
		str = (char*)realloc( str, size * string_size );
		strcat( str, def );
		strcat( str, decl );
		strcat( str, "\n" );
	}

	//string_size++; // Add null terminator
	//str = (char*)realloc( str, size * string_size );
	//strcat( str, "\0" );

	return str;
}



bool check_shader( GLuint object, int status_name, std::string name )
{
	// TODO: rewrite
	GLint status;
	switch( status_name ){
		case GL_COMPILE_STATUS:
			glGetShaderiv( object, status_name, &status );
			break;
		case GL_LINK_STATUS:
			glGetProgramiv( object, status_name, &status );
			break;
		default:
			Debug::debug( Debug::GRAPHICS, "Wrong shader operation");
			return false;
	}

	if( status == GL_FALSE ){
		int log_length;
		char* info_log;

		switch( status_name ){
			case GL_COMPILE_STATUS:
				glGetShaderiv( object, GL_INFO_LOG_LENGTH, &log_length );
				break;
			case GL_LINK_STATUS:
				glGetProgramiv( object, GL_INFO_LOG_LENGTH, &log_length );
				break;
			default:
				Debug::debug( Debug::GRAPHICS, "Wrong shader operation");
				return false;
		}

		/* The maxLength includes the NULL character */
		info_log = (char*)malloc( log_length );

		switch( status_name ){
			case GL_COMPILE_STATUS:
				glGetShaderInfoLog( object, log_length, &log_length, info_log );
				break;
			case GL_LINK_STATUS:
				glGetProgramInfoLog( object, log_length, &log_length, info_log );
				break;
			default:
				Debug::debug( Debug::GRAPHICS, "Wrong shader operation");
				return false;
		}

		Debug::debug( Debug::GRAPHICS, name + ": " + std::string(info_log) );

		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		free(info_log);
		return false;
	}
	return true;
}


GLint create_shader( const char* filename, int type, const char* defines )
{
	/* Pointer will receive the contents of our shader source code files */
	char* buffer = filetobuf( filename );
	int size = 8 + strlen(filename);
	char* name = (char*)malloc( (unsigned)sizeof(char) * size );
	snprintf( name, size, "Shader %s", filename );

	const char* sources[2] = { defines, buffer };

	/* Create an empty shader handle */
	GLuint shader = glCreateShader( type );

	/* Send the shader source code to GL */
	/* Note that the source code is NULL character terminated. */
	/* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */

	glShaderSource( shader, 2, sources, 0 );

	/* Compile the shader */
	glCompileShader( shader );

	if( !check_shader( shader, GL_COMPILE_STATUS, name ) ){
		glDeleteShader(shader);
		shader = -1;
	}else{
		glUseProgram( shader );
		glBindAttribLocation( shader, POSITION_LOCATION, "in_Position" );
		glBindAttribLocation( shader, TEX_COORD_LOCATION, "in_TextCoord" );
		glBindAttribLocation( shader, NORMAL_LOCATION, "in_Normal" );
		glBindAttribLocation( shader, COLOR_LOCATION, "in_Color" );
		GLint cm = glGetUniformLocation( shader, "in_ColorMap" );
		GLint nm = glGetUniformLocation( shader, "in_NormalMap" );
		glUniform1i( cm, gltColor );
		glUniform1i( cm, gltNormal );
		glUseProgram( 0 );
	}

	// Free allocated resources
	free(name);
	free(buffer);

	return shader;
}


GLuint createProgram( std::string filename, enum GLSFlags glflags )
{
	char* defines = generate_defines( glflags );

	GLint vertex = create_shader( (filename + ".vert").c_str(), GL_VERTEX_SHADER, defines );
	GLint fragment = create_shader( (filename + ".frag").c_str(), GL_FRAGMENT_SHADER, defines );

	free( defines );

	if( vertex < 0 || fragment < 0 )
		return 0;


	/* If we reached this point it means the vertex and fragment shaders compiled and are syntax error free. */
	/* We must link them together to make a GL shader program */
	/* GL shader programs are monolithic. It is a single piece made of 1 vertex shader and 1 fragment shader. */
	/* Assign our program handle a "name" */
	GLuint shaderprogram = glCreateProgram();

	/* Attach our shaders to our program */
	glAttachShader( shaderprogram, vertex );
	glAttachShader( shaderprogram, fragment );

	/* Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color */
	/* Attribute locations must be setup before calling glLinkProgram. */
	//glBindAttribLocation( shaderprogram, 0, "in_Position" );
	//glBindAttribLocation( shaderprogram, 1, "in_Color" );

	/* Link our program */
	/* At this stage, the vertex and fragment programs are inspected, optimized and a binary code is generated for the shader. */
	/* The binary code is uploaded to the GPU, if there is no error. */
	glLinkProgram( shaderprogram );

	/* Cleanup all the things we bound and allocated */
#ifndef DEBUG
	glDetachShader( shaderprogram, vertex );
	glDetachShader( shaderprogram, fragment );
	glDeleteShader( vertex );
	glDeleteShader( fragment );
#endif

	/* Again, we must check and make sure that it linked. If it fails, it would mean either there is a mismatch between the vertex */
	/* and fragment shaders. It might be that you have surpassed your GPU's abilities. Perhaps too many ALU operations or */
	/* too many texel fetch instructions or too many interpolators or dynamic loops. */

	if( !check_shader( shaderprogram, GL_LINK_STATUS, "Shader program " + filename ) ){
		glDeleteProgram( shaderprogram );
		return 0;
	}

	return shaderprogram;
}


GLuint Shaders::getProgram( enum GLSPass pass, enum GLSFlags glflags )
{
	std::map< enum GLSFlags, GLuint >& shaders_map = shaders[pass];
	std::map< enum GLSFlags, GLuint >::iterator fit = shaders_map.find( glflags );
	if( fit != shaders_map.end() )
		return fit->second;
	const char* sdrname = passes[pass];
	GLuint prog = createProgram( conf.shadersPath + sdrname, glflags );
	if( prog ){
		shaders_map[glflags] = prog;
		//glUniform1i(glGetUniformLocation(prog, "colorTexture"), 0);
		//if( glflags & glsNormals )
		//	glUniform1i(glGetUniformLocation(prog, "normalTexture"), 1);
	}
	return prog;
}


#define PASS_UNIFORM( type, func )													\
	int pass = 1;																	\
	while( pass < glpLast ){														\
		std::map< enum GLSFlags, GLuint >& shaders_map = shaders[pass];				\
		FOREACHIT(shaders_map){														\
			/* flag in flags or flag is glsAll */									\
			if( glflag != glsAll && !(it->first & glflag ) )						\
				continue;															\
			int location = glGetUniformLocation( it->second, name );				\
			glUseProgram(it->second);												\
			func;																	\
		}																			\
		pass <<= 1;																	\
	}																				\
	glUseProgram(0);

#define PASS_UNIFORM_V( glname, type )												\
void Shaders::passUniform##glname( enum GLSFlags glflag, const char* name,			\
		int count, type data )														\
{																					\
	PASS_UNIFORM(type, glUniform##glname( location, count, data ));					\
}

#define PASS_UNIFORM_MATRIX( glname, type )											\
void Shaders::passUniformMatrix##glname( enum GLSFlags glflag, const char* name,	\
		int count, GLboolean transpose, type data )									\
{																					\
	PASS_UNIFORM(type, glUniformMatrix##glname( location, count, transpose, data ));\
}


PASS_UNIFORM_V( 3fv, float* )

PASS_UNIFORM_MATRIX( 4fv, float* )

#undef PASS_UNIFORM
