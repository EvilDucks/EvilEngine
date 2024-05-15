#pragma once
#include "tool/debug.hpp"
#include "global.hpp"
#include "gl.hpp"

// How it works.
//


namespace SHADER::UNIFORM {

	struct F4 {
		r32 v1, v2, v3, v4;
	};

	struct TX {
		u8 texture;
		u8 at;
	};

	using M4 = glm::mat4;
	using F3 = glm::vec3;
	using F2 = glm::vec2;
	using F1 = float;
	using I1 = GLuint;

	using SetFunc = void (*const)(const GLint& uniform, const any& values);

	struct Uniform {
		s16 id = 0; // Every next uniform in shader is simply prevNumber + 1 starting from 0.
		u8 bufforIndex = 0;
		u8 setIndex = 0;
	};

	const u8 UNIFORM_BYTES = sizeof (SHADER::UNIFORM::Uniform);

}


namespace SHADER::UNIFORM::NAMES {

	const char PROJECTION[] 				{ "projection" };
	const char VIEW[] 						{ "view" };
	const char MODEL[] 						{ "model" };

	const char SAMPLER_1[]					{ "sampler1" };
	const char SAMPLER_1A[]					{ "sampler1a" };

	const char COLOR[] 						{ "color" };
	const char SHIFT[]						{ "shift" };
	const char TILE[]						{ "tile" };

	const char LIGHT_POSITION[]				{ "lightPosition" };
	const char LIGHT_CONSTANT[]				{ "lightConstant" };
	const char LIGHT_LINEAR[]				{ "lightLinear" };
	const char LIGHT_QUADRATIC[]			{ "lightQuadratic" };
	const char LIGHT_AMBIENT[]				{ "lightAmbient" };
	const char LIGHT_AMBIENT_INTENSITY[]	{ "lightAmbientIntensity" };
	const char LIGHT_DIFFUSE[]				{ "lightDiffuse" };
	const char LIGHT_DIFFUSE_INTENSITY[]	{ "lightDiffuseIntensity" };

	const char VIEW_POSITION[]				{ "viewPosition" };

	u8 namesCount = 17;
	const char* const names[] {
		PROJECTION, VIEW, MODEL, SAMPLER_1, SAMPLER_1A, COLOR, SHIFT, TILE,
		LIGHT_POSITION, LIGHT_CONSTANT, LIGHT_LINEAR, LIGHT_QUADRATIC, 
		LIGHT_AMBIENT, LIGHT_AMBIENT_INTENSITY, LIGHT_DIFFUSE, LIGHT_DIFFUSE_INTENSITY,
		VIEW_POSITION
	};

}


namespace SHADER::UNIFORM::BUFFORS { // UNIQUE

	I1 error 		{ 0 };
	M4 projection	= glm::mat4 (1.0f);
	M4 view			= glm::mat4 (1.0f);
	M4 model		= glm::mat4 (1.0f);
	TX sampler1 	{ 0, 0 };
	F4 color		{ 0 };
	F2 shift		{ 0 };
	I1 tile 		{ 0 };

	F3 lightPosition			{ 0 };
	F1 lightConstant			{ 0 };
	F1 lightQuadratic			{ 0 };
	F1 lightLinear				{ 0 };
	F3 lightAmbient				{ 0 };
	F1 lightAmbientIntensity	{ 0 };
	F3 lightDiffuse				{ 0 };
	F1 lightDiffuseIntensity	{ 0 };

	F3 viewPosition 			{ 0 };

	any buffors[] {
		&error,
		&projection,
		&view,
		&model,
		&sampler1,
		&color,
		&shift,
		&tile,

		&lightPosition,
		&lightConstant,
		&lightQuadratic,
		&lightLinear,
		&lightAmbient,
		&lightAmbientIntensity,
		&lightDiffuse,
		&lightDiffuseIntensity,

		&viewPosition,
	};

	enum class D: u8 {
		DERROR = 0,
		PROJECTION = 1,
		VIEW = 2,
		MODEL = 3,
		SAMPLER_1 = 4,
		COLOR = 5,
		SHIFT = 6,
		TILE = 7,
		LIGHT_POSITION = 8,
		LIGHT_CONSTANT = 9,
		LIGHT_LINEAR = 10,
		LIGHT_QUADRATIC = 11,
		LIGHT_AMBIENT = 12,
		LIGHT_AMBIENT_INTENSITY = 13,
		LIGHT_DIFFUSE = 14,
		LIGHT_DIFFUSE_INTENSITY = 15,
		VIEW_POSITION = 16,
	};
}


namespace SHADER::UNIFORM::SETS {

	// Uniforms apply their data during their rendering. 
	//  To automize that process we set how and with what during initialization like here.
	//  and later we only change the buffor values to apply any changes.
	void ER (const GLint& uniform, const any& values) { 
		DEBUG_RENDER spdlog::error ("Uniform set function not connected!");
	}

	void M4 (const GLint& uniform, const any& values) { 
		auto data = *(SHADER::UNIFORM::M4*)values;
		glUniformMatrix4fv (uniform, 1, GL_FALSE, &data[0][0]);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 1);
	};

	void F4 (const GLint& uniform, const any& values) { 
		auto data = *(SHADER::UNIFORM::F4*)values;
		glUniform4f (uniform, data.v1, data.v2, data.v3, data.v4); 
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 0);
	};

	void F3 (const GLint& uniform, const any& values) {
		auto data = *(SHADER::UNIFORM::F3*)values;
		glUniform3f (uniform, data.x, data.y, data.z);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 2);
	};

	void F2 (const GLint& uniform, const any& values) {
		auto data = *(SHADER::UNIFORM::F2*)values;
		glUniform2f (uniform, data.x, data.y);
		DEBUG_RENDER GL::GetError (GL::UNIFORM_SET + 2);
	};

	void F1 (const GLint& uniform, const any& values) {
		auto data = *(SHADER::UNIFORM::F1*)values;
		glUniform1f (uniform, data);
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
		ER, M4, F4, F3, F2, F1, I1, TX, AT,
	};

	enum class D: u8 {
		DER = 0,
		DM4 = 1,
		DF4 = 2,
		DF3 = 3,
		DF2 = 4,
		DF1 = 5,
		DI1 = 6,
		DTX = 7,
		DAT = 8,
	};
}


namespace SHADER {

	struct Shader {
		s16 id = 0;
	};

}


namespace SHADER::UNIFORM {

	// COPY for each shader.
	Uniform projection	{ 0, (u8)BUFFORS::D::PROJECTION,	(u8)SETS::D::DM4 }; // 01
	Uniform view 		{ 0, (u8)BUFFORS::D::VIEW,			(u8)SETS::D::DM4 }; // 02
	Uniform model 		{ 0, (u8)BUFFORS::D::MODEL,			(u8)SETS::D::DM4 }; // 03
	Uniform sampler1 	{ 0, (u8)BUFFORS::D::SAMPLER_1,		(u8)SETS::D::DTX }; // 04
	Uniform samplerA1	{ 0, (u8)BUFFORS::D::SAMPLER_1,		(u8)SETS::D::DAT }; // 05
	Uniform color		{ 0, (u8)BUFFORS::D::COLOR,			(u8)SETS::D::DF4 }; // 06
	Uniform shift		{ 0, (u8)BUFFORS::D::SHIFT,			(u8)SETS::D::DF2 }; // 07
	Uniform tile		{ 0, (u8)BUFFORS::D::TILE,			(u8)SETS::D::DI1 }; // 08

	Uniform lightPosition			{ 0, (u8)BUFFORS::D::LIGHT_POSITION,			(u8)SETS::D::DF3 }; // 09
	Uniform lightConstant 			{ 0, (u8)BUFFORS::D::LIGHT_CONSTANT,			(u8)SETS::D::DF1 }; // 10
	Uniform lightLinear 			{ 0, (u8)BUFFORS::D::LIGHT_LINEAR,				(u8)SETS::D::DF1 }; // 11
	Uniform lightQuadratic 			{ 0, (u8)BUFFORS::D::LIGHT_QUADRATIC,			(u8)SETS::D::DF1 }; // 12
	Uniform lightAmbient			{ 0, (u8)BUFFORS::D::LIGHT_AMBIENT,				(u8)SETS::D::DF3 }; // 13
	Uniform lightAmbientIntensity	{ 0, (u8)BUFFORS::D::LIGHT_AMBIENT_INTENSITY,	(u8)SETS::D::DF1 }; // 14
	Uniform lightDiffuse			{ 0, (u8)BUFFORS::D::LIGHT_DIFFUSE,				(u8)SETS::D::DF3 }; // 15
	Uniform lightDiffuseIntensity	{ 0, (u8)BUFFORS::D::LIGHT_DIFFUSE_INTENSITY,	(u8)SETS::D::DF1 }; // 16

	Uniform viewPosition			{ 0, (u8)BUFFORS::D::VIEW_POSITION,				(u8)SETS::D::DF3 }; // 17

	u32 uniformsCount = 17;
	Uniform uniforms[] {
		projection, view, model, sampler1, samplerA1, color, shift, tile,
		lightPosition, lightConstant, lightLinear, lightQuadratic,
		lightAmbient, lightAmbientIntensity, lightDiffuse, lightDiffuseIntensity,
		viewPosition
	};

}


namespace SHADER::UNIFORM {

	void SetsMaterial (const Shader& program) {
		// Layout<->Uniform structure things.
	}

	void SetsMesh (const Shader& program, const u8& uniformsCount, UNIFORM::Uniform*& uniforms) {
        ZoneScopedN("Shader::UNIFORM: SetsMesh");

		for (u8 i = 0; i < uniformsCount; ++i) {
			auto& uniform = uniforms[i].id;
			auto& buffor = BUFFORS::buffors[uniforms[i].bufforIndex];
			auto& set	 = SETS::sets[uniforms[i].setIndex];
			set (uniform, buffor);
		}

	}


	void CreateAll (
		/* OUT */ Shader& program,
		/* IN  */ const u8& uniformsTableCount,
		/* OUT */ UNIFORM::Uniform*& uniformsTable,
		/* IN  */ const char** uniformNames
	) {
        ZoneScopedN("Shader::UNIFORM: Create");
		for (u8 i = 0; i < uniformsTableCount; ++i)
			uniformsTable[i].id = glGetUniformLocation (program.id, uniformNames[i]);
	}

	void Create (
		/* OUT */ Shader& program,
		/* OUT */ UNIFORM::Uniform& uniform,
		/* IN  */ const char* uniformName
	) {
		ZoneScopedN("Shader::UNIFORM: Create");
		uniform.id = glGetUniformLocation (program.id, uniformName);
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
        ZoneScopedN("Shader: ReadShader");

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
        ZoneScopedN("Shader: Create");

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
	}

}