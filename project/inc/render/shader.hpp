#pragma once
#include "tool/debug.hpp"
#include "global.hpp"
#include "gl.hpp"

namespace SHADER {

	//using Value = const r64&;
	struct Values {
		r32 v1;
		r32 v2;
		r32 v3;
		r32 v4;
	};

	using SetFunc = void (*const)(const GLint& uniform, const Values& values);

	struct Shader {
		GLuint id = 0;
		u64 uniformsCount = 0;
		GLint* uniforms = nullptr;
		SetFunc* sets = nullptr;
	};
	

	void GetShaderError (const GLuint& identifier, const char* const type);
	void ReadShader (char*& buffor, const char*& filepath);
	void Create (Shader& program, const char*& filepathVertex, const char*& filepathFragment);
	void Use ();


	void GetShaderError (
		const GLuint& identifier, 
		const char* const type
	) {
		char infoLog[512];
		GLint isSuccess;
	
		glGetShaderiv (identifier, GL_COMPILE_STATUS, &isSuccess);

		if (!isSuccess) {
			glGetShaderInfoLog (identifier, 512, NULL, infoLog);
			spdlog::error ("{0} Shader Compilation Failure!\n{1}", type, infoLog);
		} else {
			spdlog::info ("Succesfull {0} Shader Compilation", type);
		}
	}



	void ReadShader (
		char*& buffor, 
		const char*& filepath
	) {
		FILE *file = fopen (filepath, "rb");

		DEBUG {
			u64 length;

			if (file) {
				fseek (file, 0, SEEK_END);
				length = ftell (file);

				fseek (file, 0, SEEK_SET);
				buffor = (char*) malloc (length + 1);
			} else {
				spdlog::error ("Shader: Script not found!");
				exit (1);
			}

			if (buffor) {
				fread (buffor, 1, length, file);
				buffor[length] = '\0';
			} else {
				spdlog::error ("Shader: Buffor could not be allocated!");
				exit (1);
			}
		} else {
			fseek (file, 0, SEEK_END);
			u64 length = ftell (file);

			fseek (file, 0, SEEK_SET);
			buffor = (char*) malloc (length + 1);

			fread (buffor, 1, length, file);
			buffor[length] = '\0';
		}

		fclose (file);
	}


	void Create (
		/* OUT */  Shader& program, 
		/* IN  */  const char*& filepathVertex, 
		/* IN  */  const char*& filepathFragment
	) {
		GLuint idFragment = glCreateShader (GL_FRAGMENT_SHADER);
		GLuint idVertex = glCreateShader (GL_VERTEX_SHADER);

		char* buffor = nullptr;

		{ // VERTEX
			ReadShader (buffor, filepathVertex);
			
			glShaderSource (idVertex, 1, &buffor, NULL);
			glCompileShader (idVertex);
			free (buffor);

			DEBUG { GetShaderError (idVertex, "Vertex"); }
		}

		{ // FRAGMENT
			ReadShader (buffor, filepathFragment);

			glShaderSource (idFragment, 1, &buffor, NULL);
			glCompileShader (idFragment);
			free (buffor);

			DEBUG { GetShaderError (idFragment, "Fragment"); }
		}

		{ // BUNDLED
			program.id = glCreateProgram ();
			glAttachShader (program.id, idVertex);
			glAttachShader (program.id, idFragment);
			glLinkProgram (program.id);

			DEBUG {
				char infoLog[512];
				GLint isSuccess;

				glGetProgramiv (program.id, GL_LINK_STATUS, &isSuccess);
				
				if (!isSuccess) {
					glGetProgramInfoLog (program.id, 512, NULL, infoLog);
					spdlog::error ("Program Shader Compilation Failure!\n{0}", infoLog);
				} else {
					spdlog::info ("Succesfull Program Shader Compilation");
				}
			}

			// Once objects are linked they're no longer needed.
			glDeleteShader (idVertex);
			glDeleteShader (idFragment);  

		}
		
	}


	void CreateUniforms (
		/* OUT */ Shader& program, 
		/* IN  */ const u64& attributesCount,
		/* IN  */ const char** attributes,
		/* IN  */ SetFunc* sets
	) { 
		program.uniforms = new GLint[attributesCount];
		//program.sets = new SetFunc[attributesCount];

		program.uniformsCount = attributesCount;
		for (u64 i = 0; i < program.uniformsCount; ++i) {
			program.uniforms[i] = glGetUniformLocation (program.id, attributes[i]);
		}

		program.sets = sets;
	}

	void SetColor (
		const GLint& uniform,
		const Values& values
	) {
		glUniform4f (uniform, values.v1, values.v2, values.v3, values.v4);
	}


	void Use (const Shader& program) {
		glUseProgram (program.id);
	}

	void Set (const Shader& program, const u64& uniformId, const Values& values) {
		auto& uniform = program.uniforms[uniformId];
		auto& set = program.sets[uniformId];
		set (uniform, values);
	}

	void Destroy (Shader& program) {
		glDeleteProgram (program.id);
		delete[] program.uniforms;
		//delete[] program.sets;
	}

}