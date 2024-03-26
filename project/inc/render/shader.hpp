#pragma once
#include "tool/debug.hpp"
//#include "global.hpp"
#include "gl.hpp"


namespace SHADER::UNIFORM {

	struct F4 {
		r32 v1, v2, v3, v4;
	};

	using M4 = glm::mat4;

	using SetFunc = void (*const)(const GLint& uniform, const any& values);

	struct Uniform {
		GLint id = 0;
		any data = nullptr;
		SetFunc set = nullptr;
	};

}


namespace SHADER {

	struct Shader {
		GLuint id = 0;
		u64 uniformsCount = 0;
		UNIFORM::Uniform* uniforms = nullptr;
	};

}


namespace SHADER::UNIFORM {

	void SetsMesh (const Shader& program) {
		for (u64 i = 0; i < program.uniformsCount; ++i) {
			auto& uniform = program.uniforms[i].id;
			auto& set = program.uniforms[i].set;
			auto& buffor = program.uniforms[i].data;
			set (uniform, buffor);
		}
	}


	void Create (
		/* OUT */ Shader& program,
		/* IN  */ const u64& uniformsCount,
		/* IN  */ const char** uniformNames,
		/* IN  */ UNIFORM::Uniform* uniforms
	) {
		program.uniforms = uniforms;
		program.uniformsCount = uniformsCount;
	
		for (u64 i = 0; i < program.uniformsCount; ++i) {
			program.uniforms[i].id = glGetUniformLocation (program.id, uniformNames[i]);
		}
	}


	// Uniforms apply their data during their rendering. 
	//  To automize that process we set how and with what during initialization like here.
	//  and later we only change the buffor values to apply any changes.
	SetFunc SetF4 = [](const GLint& uniform, const any& values) { 
		auto data = *(SHADER::UNIFORM::F4*)values;
		glUniform4f (uniform, data.v1, data.v2, data.v3, data.v4); 
	};

	SetFunc SetM4 = [](const GLint& uniform, const any& values) { 
		auto data = *(SHADER::UNIFORM::M4*)values;
		//glUniform4f (uniform, data.v1, data.v2, data.v3, data.v4); 
		glUniformMatrix4fv(uniform, 1, GL_FALSE, &data[0][0]);
	};

}


namespace SHADER {

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


	void Use (const Shader& program) {
		glUseProgram (program.id);
	}


	void Destroy (Shader& program) {
		glDeleteProgram (program.id);
		//delete[] program.uniforms;
		//delete[] program.sets;
	}

}