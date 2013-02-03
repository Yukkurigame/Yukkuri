/*
 * gl_shader.cpp
 *
 *  Created on: 24.05.2012
 *
 */
#include "graphics/gl_extensions.h"

#include "graphics/utils/gl_shader.h"
#include <stdio.h>
#include "scripts/LuaConfig.h"
#include <map>

#include "config.h"
#include "safestring.h"
#include "hacks.h"
#include "debug.h"



extern MainConfig conf;


ShaderConfigAttributes::~ShaderConfigAttributes()
{
	free( name );
}

ShaderConfigData::~ShaderConfigData()
{
	if( vertex_name )
		free( vertex_name );
	if( fragment_name )
		free( fragment_name );

	if( output ){
		for( unsigned int i = 0; i < output_count; ++i )
			free( output[i] );
		free( output );
	}

	if( attributes )
		delete[] attributes;
}



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
	std::map< enum GLSFlags, UniformHandlers > uniforms[glpLast];

	// TODO: macro from configs
	#define MACROS_SIZE 3
	const char* macros[MACROS_SIZE] = {
		"_YNORMALS", "_YLIGHT", "_YFIXED"
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
	if( buffer == NULL ){
		Debug::debug( Debug::OS, "Shader file not found: " + std::string(filename) + "\n" );
		return -1;
	}

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
	}
	// Free allocated resources
	free(name);
	free(buffer);

	return shader;
}



GLuint createProgram( enum GLSPass pass, enum GLSFlags glflags )
{
	char* defines = generate_defines( glflags );

	// Get data from config file
	ShaderConfigData config;
	// Push shader config to stack and load config;
	LuaConfig* cfg = new LuaConfig();
	std::string shd_id = "shader_" + citoa(pass);
	cfg->pushSubconfig( shd_id.c_str(), "shader");
	int conf_loaded = cfg->LuaMain::getValue( -1, config );
	// Remove shader config from stack;
	cfg->pop( 1 );
	delete cfg;

	if( !conf_loaded )
		return 0;

	GLint vertex = create_shader( (conf.path.shaders + config.vertex_name).c_str(),
									GL_VERTEX_SHADER, defines );
	GLint fragment = create_shader( (conf.path.shaders + config.fragment_name).c_str(),
									GL_FRAGMENT_SHADER, defines );

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

	/* Bind attribute indexes to locations */
	/* Attribute locations must be setup before calling glLinkProgram. */
	for( unsigned int i = 0; i < config.attributes_count; ++i )
		glBindAttribLocation( shaderprogram, config.attributes[i].index, config.attributes[i].name );
	/* Bind fragment outputs indexes to locations */
	for( unsigned int i = 0; i < config.output_count; ++i )
		glBindFragDataLocation( shaderprogram, i, config.output[i] );

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

	if( !check_shader( shaderprogram, GL_LINK_STATUS, (std::string)"Shader program " + config.name ) ){
		glDeleteProgram( shaderprogram );
		return 0;
	}

	GLint uniforms_count;
	glGetProgramiv( shaderprogram, GL_ACTIVE_UNIFORMS, &uniforms_count );

	UniformHandlers& hdl = uniforms[pass][glflags];
	hdl.count = uniforms_count;
	if( hdl.count )
		hdl.handlers = new UniformHandler[hdl.count];
	for( UINT i = 0; i < hdl.count; ++i ){
		int name_len = 0, num = 0;
		char name[100];
		UniformHandler* h = &hdl.handlers[i];
		glGetActiveUniform( shaderprogram, i, sizeof(name)-1, &name_len, &num, &h->type, name );
		name[name_len] = '\0';
		h->location = glGetUniformLocation( shaderprogram, name );
		h->index = UniformsManager::register_uniform( name, h->type );
	}

	return shaderprogram;
}



GLuint Shaders::getProgram( enum GLSPass pass, enum GLSFlags glflags )
{
	if( pass == glpNone )
		return 0;

	std::map< enum GLSFlags, GLuint >& shaders_map = shaders[pass];
	std::map< enum GLSFlags, GLuint >::iterator fit = shaders_map.find( glflags );
	if( fit != shaders_map.end() )
		return fit->second;

	GLuint prog = createProgram( pass, glflags );
	if( prog ){
		shaders_map[glflags] = prog;
	}
	return prog;
}


UniformHandlers* Shaders::getUniforms( enum GLSPass pass, enum GLSFlags glflags )
{
	if( pass == glpNone )
		return NULL;

	std::map< enum GLSFlags, UniformHandlers >& uniforms_map = uniforms[pass];
	std::map< enum GLSFlags, UniformHandlers >::iterator fit = uniforms_map.find( glflags );
	if( fit != uniforms_map.end() )
		return &fit->second;
	return NULL;
}

