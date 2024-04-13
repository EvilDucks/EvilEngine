#pragma once
#include "tool/debug.hpp"
#include "global.hpp"
#include "gl.hpp"


namespace SHADER::UNIFORM {

	struct F4 {
		r32 v1, v2, v3, v4;
	};

	struct TX {
		u8 texture;
		u8 at;
	};

	using M4 = glm::mat4;
	using F2 = glm::vec2;
	using I1 = GLuint;

	using SetFunc = void (*const)(const GLint& uniform, const any& values);

	struct Uniform {
		s16 id = 0;
		u8 bufforIndex = 0;
		u8 setIndex = 0;
	};

}


namespace SHADER::UNIFORM::NAMES {
	const char COLOR[] 		{ "color" };
	const char MODEL[] 		{ "model" };
	const char VIEW[] 		{ "view" };
	const char PROJECTION[] { "projection" };
	const char SAMPLER_1[]	{ "sampler1" };
	const char SHIFT[]		{ "shift" };
	const char TILE[]		{ "tile" };
}


namespace SHADER::UNIFORM::BUFFORS { // UNIQUE

	I1 error 		{ 0 };
	M4 projection	= glm::mat4 (1.0f);
	M4 view			= glm::mat4 (1.0f);
	M4 globalSpace	= glm::mat4 (1.0f);
	TX sampler1 	{ 0, 0 };
	F4 color		{ 0 };
	F2 shift		{ 0 };
	I1 tile 		{ 0 };

	any buffors[] {
		&error,
		&projection,
		&view,
		&globalSpace,
		&sampler1,
		&color,
		&shift,
		&tile,
	};

	enum class D: u8 {
		DERROR = 0,
		PROJECTION = 1,
		VIEW = 2,
		GLOBAL_SPACE = 3,
		SAMPLER_1 = 4,
		COLOR = 5,
		SHIFT = 6,
		TILE = 7,
	};
}


namespace SHADER::UNIFORM::SETS {

	// Uniforms apply their data during their rendering. 
	//  To automize that process we set how and with what during initialization like here.
	//  and later we only change the buffor values to apply any changes.
	void ER (const GLint& uniform, const any& values) { 
		DEBUG_RENDER spdlog::error ("Uniform set function not connected!");
	}

	void F4 (const GLint& uniform, const any& values) { 
		auto data = *(SHADER::UNIFORM::F4*)values;
		glUniform4f (uniform, data.v1, data.v2, data.v3, data.v4); 
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 0);
	};

	void M4 (const GLint& uniform, const any& values) { 
		auto data = *(SHADER::UNIFORM::M4*)values;
		glUniformMatrix4fv (uniform, 1, GL_FALSE, &data[0][0]);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 1);
	};

	void F2 (const GLint& uniform, const any& values) {
		auto data = *(SHADER::UNIFORM::F2*)values;
		glUniform2f (uniform, data.x, data.y);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 2);
	};

	void I1 (const GLint& uniform, const any& values) {
		auto data = *(SHADER::UNIFORM::I1*)values;
		glUniform1i (uniform, data);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 3);
	};

	void TX (const GLint& uniform, const any& values) {
		auto data = *(SHADER::UNIFORM::TX*)values;
		glUniform1i (uniform, data.at);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 4);
		glBindTexture (GL_TEXTURE_2D, data.texture);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 5);
	};

	void AT (const GLint& uniform, const any& values) {
		auto data = *(SHADER::UNIFORM::TX*)values;
		glUniform1i (uniform, data.at);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 6);
		glBindTexture (GL_TEXTURE_2D_ARRAY, data.texture);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 7);
	};

	SetFunc sets[] {
		ER, F4, M4, F2, I1, TX, AT,
	};

	enum class D: u8 {
		DER = 0,
		DF4 = 1,
		DM4 = 2,
		DF2 = 3,
		DI1 = 4,
		DTX = 5,
		DAT = 6,
	};
}


namespace SHADER {

	struct Shader {
		s16 id = 0;
		u16 uniformsCount = 0;
		UNIFORM::Uniform* uniforms = nullptr;
	};

}


namespace SHADER::UNIFORM {

	// COPY for each shader.
	Uniform projection	{ 0, (u8)BUFFORS::D::PROJECTION,	(u8)SETS::D::DM4 }; // 1
	Uniform view 		{ 0, (u8)BUFFORS::D::VIEW,			(u8)SETS::D::DM4 }; // 2
	Uniform model 		{ 0, (u8)BUFFORS::D::GLOBAL_SPACE,	(u8)SETS::D::DM4 }; // 3
	Uniform sampler1 	{ 0, (u8)BUFFORS::D::SAMPLER_1,		(u8)SETS::D::DTX }; // 4
	Uniform samplerA1	{ 0, (u8)BUFFORS::D::SAMPLER_1,		(u8)SETS::D::DAT }; // 5
	Uniform color		{ 0, (u8)BUFFORS::D::COLOR,			(u8)SETS::D::DF4 }; // 6
	Uniform shift		{ 0, (u8)BUFFORS::D::SHIFT,			(u8)SETS::D::DF2 }; // 7
	Uniform tile		{ 0, (u8)BUFFORS::D::TILE,			(u8)SETS::D::DI1 }; // 8

}


namespace SHADER::UNIFORM {

	void SetsMaterial (const Shader& program) {
		// Layout<->Uniform structure things.
	}

	void SetsMesh (const Shader& program) {
		for (u64 i = 0; i < program.uniformsCount; ++i) {
			auto& uniform = program.uniforms[i].id;
			// Both are indexed now for storage optimalization.
			auto& buffor = BUFFORS::buffors[program.uniforms[i].bufforIndex];
			auto& set	 = SETS::sets[program.uniforms[i].setIndex];
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

}


namespace SHADER {

	void GetShaderError (const GLuint& identifier, const char* const type);
	void ReadShader (char*& buffor, const char*& filepath);
	void Create (Shader& program, const char*& filepathVertex, const char*& filepathFragment);


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
		const char* const& filepath
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
		/* IN  */  const char* const& filepathVertex, 
		/* IN  */  const char* const& filepathFragment
	) {
		GLuint idFragment = glCreateShader (GL_FRAGMENT_SHADER);
		GLuint idVertex = glCreateShader (GL_VERTEX_SHADER);

		char* buffor = nullptr;

		{ // VERTEX
			ReadShader (buffor, filepathVertex);
			
			glShaderSource (idVertex, 1, &buffor, NULL);
			glCompileShader (idVertex);
			free (buffor);

			DEBUG_SHADER { GetShaderError (idVertex, "Vertex"); }
		}

		{ // FRAGMENT
			ReadShader (buffor, filepathFragment);

			glShaderSource (idFragment, 1, &buffor, NULL);
			glCompileShader (idFragment);
			free (buffor);

			DEBUG_SHADER { GetShaderError (idFragment, "Fragment"); }
		}

		{ // BUNDLED
			program.id = glCreateProgram ();
			glAttachShader (program.id, idVertex);
			glAttachShader (program.id, idFragment);
			glLinkProgram (program.id);

			DEBUG_SHADER {
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