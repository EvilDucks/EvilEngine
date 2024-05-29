#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

// for now only...
#include "shader.hpp"

// FreeType inc
#include <ft2build.h>
#include FT_FREETYPE_H

namespace FONT {

	// How many model's we can have inside a frag call.
	const u16 MODELS_MAX = 384;
	GLuint textureArray;

	int letterMap[MODELS_MAX] { 0 };
	glm::mat4 models[MODELS_MAX] { glm::mat4(1.0f) }; // Are all keys initialized this way??? 46.27. HACK

	// THE WHOLE THING NEEDS TO BE OPTIMIZED!

	//GLuint faceVAO, faceVBO;
	//SHADER::Shader faceShader {}; // initializes

	struct Character {
		GLint		letter;  // ID handle of the glyph texture	// why is that an int now?
		glm::ivec2	size;       // Size of glyph
		glm::ivec2	bearing;    // Offset from baseline to left/top of glyph
		FT_Pos		advance;    // Offset to advance to next glyph
	};

	// 'x' -> x, we def. can do better... soon
	std::map<char, Character> characters;


	void Load (
		FT_Face& face,
		FT_Library& freeType,
		const char* filePath
	) {
		u32 errorCode = FT_New_Face (freeType, filePath, 0, &face);

		DEBUG if ( errorCode ) {
			spdlog::error ("FREETYPE: Failed to load font");
			exit (1);
		}

		// Once we've loaded the face, we should define the pixel font size we'd like to extract from this face:
		// The function sets the font's width and height parameters. Setting the width to 0 lets the face dynamically calculate the width based on the given height.
		FT_Set_Pixel_Sizes (face, 256, 256); // We standirize it now.
		errorCode = FT_Load_Char (face, 'X', FT_LOAD_RENDER);

		DEBUG if ( errorCode ){
			spdlog::error ("FREETYTPE: Failed to load Glyph");
			exit (1);
		}
	}


	void CreateTexture (
		const FT_Face& face
	) {
		PROFILER { ZoneScopedN("Font: Create"); }
		DEBUG_RENDER GL::GetError (1000 + 7);

		u32 errorCode;
		glPixelStorei (GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction -> GL_RED connected
		DEBUG_RENDER GL::GetError (1000 + 6);
		// SETUP 3D TEXTURE
		glGenTextures (1, &textureArray);
		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D_ARRAY, textureArray);
		DEBUG_RENDER GL::GetError (1000 + 5);

		const u16 WIDTH = 256 + 128;
		const u16 HEIGHT = 256;

		glTexImage3D (
			GL_TEXTURE_2D_ARRAY, 0, GL_R8, 
			256 + 128, 256, 128, 0, GL_RED, // Holy F. Motherf. 'W' letter.
			GL_UNSIGNED_BYTE, 0
		);
		DEBUG_RENDER GL::GetError (1000 + 4);

		// LOAD INTO THAT 3D TEXTURE


		for (unsigned char sign = 0; sign < 128; ++sign) {
			// load character glyph 
			errorCode = FT_Load_Char (face, sign, FT_LOAD_RENDER);
			//
			DEBUG if ( errorCode ) {
				spdlog::error ("FREETYTPE: Failed to load Glyph nr: {0}", (u8)(sign));
				continue;
			}

			DEBUG_RENDER if (
				face->glyph->bitmap.width > WIDTH ||
				face->glyph->bitmap.rows > HEIGHT
			) {
				spdlog::error (
					"c: {0} - {1}, {2}", sign, 
					face->glyph->bitmap.width, 
					face->glyph->bitmap.rows
				);

				exit (1);
			}

			glTexSubImage3D (
				GL_TEXTURE_2D_ARRAY, 0, 
				0, 0, int(sign), 
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				1, 
				GL_RED, GL_UNSIGNED_BYTE, 
				face->glyph->bitmap.buffer
			);
			DEBUG_RENDER GL::GetError (1000 + 3);

			glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			DEBUG_RENDER GL::GetError (1000 + 2);
			//
			// Store the final character for use later.
			Character character = {
				int(sign), 
				glm::ivec2 (face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2 (face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			//
			characters.insert ( std::pair<char, Character> (sign, character) );
		}
		DEBUG_RENDER GL::GetError (1000 + 1);
		glBindTexture (GL_TEXTURE_2D_ARRAY, 0);
		DEBUG_RENDER GL::GetError (1000 + 0);
	}


	void CreateMesh (
		/* OUT */ GLuint& vao,
		/* OUT */ GLuint* buffers
	) {
		const r32 verticesCount = 4; // Now we're doing a triangle strip
		const r32 vertices[] {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
		};

		const u8 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		auto& vbo = buffers[0];

		glGenVertexArrays (1, &vao);
		glGenBuffers (1, buffers);
		glBindVertexArray (vao);

		/*  v  */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/*  v  */ glBufferData (GL_ARRAY_BUFFER, sizeof(float) * verticesCount * 2, vertices, GL_STATIC_DRAW);
		/*  v  */ DEBUG_RENDER GL::GetError (10);

		/*  v  */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		/*  v  */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/*  v  */ DEBUG_RENDER GL::GetError (11);

		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);
	}


	void RenderText (
		/* IN  */ const GLuint* buffers,
		/* IN  */ const u16& textCount,
		/* IN  */ const char* const text,
		/* IN  */ r32 x, r32 y, 
		/* IN  */ r32 scaleX, r32 scaleY,
		//
		const GLuint& vao, 
		const SHADER::Shader& program, 
		const u16& uniformsCount, 
		SHADER::UNIFORM::Uniform*& uniforms,
		//
		const glm::mat4& projection,
		const SHADER::UNIFORM::F4& color
	) {
		PROFILER { ZoneScopedN("Font: RenderText"); }

		// Adjusting the scale
		scaleX = scaleX * 48.0f / 256.0f;
		scaleY = scaleY * 48.0f / 256.0f;

		const r32 lineHeight = 1.3;

		auto& vbo = buffers[0];
		const r32 dx = x;	// default x

		

		int workingIndex = 0;

		for (u16 i = 0; i < textCount; ++i) {
			const char sign = text[i];
			const FONT::Character character = FONT::characters[sign];

			if (workingIndex == MODELS_MAX - 1) {
				break;
			}

			switch (sign) {

				case '\n': { // Don't render new lines
					y -= character.size.y * lineHeight * scaleX;
					x = dx;
					continue;
				};

				case '\t': { // Don't render tabs
					x += (character.advance >> 6) * scaleX * 4;
					continue;
				};

				case ' ': { // Don't render spaces
					x += (character.advance >> 6) * scaleX;
					continue;
				};

			}
			
			const float xpos = x + character.bearing.x * scaleX;
			const float ypos = y - (character.size.y - character.bearing.y) * scaleY;

			//glm::mat4 model = glm::mat4(1.0f);
			// MAYBE THIS IS WRONG? 21:48t
			//model = glm::translate	(model, glm::vec3 (xpos, ypos, 0.0f));
			//model = glm::scale		(model, glm::vec3 (character.size.x * scaleX, character.size.y * scaleY, 1.0f));
			
			models[workingIndex] = 
				glm::translate (glm::mat4(1.0f), glm::vec3 (xpos, ypos, 0.0f)) * 
				glm::scale (glm::mat4(1.0f), glm::vec3 (256 * scaleX, 256 * scaleY, 0.0f))
			;

			letterMap[workingIndex] = character.letter;


			//const float w = character.size.x * scaleX;
			//const float h = character.size.y * scaleY;
			//
			//const float vertices[6][4] = {
			//	{ xpos,     ypos + h,   0.0f, 0.0f },            
			//	{ xpos,     ypos,       0.0f, 1.0f },
			//	{ xpos + w, ypos,       1.0f, 1.0f },
			//	//
			//	{ xpos,     ypos + h,   0.0f, 0.0f },
			//	{ xpos + w, ypos,       1.0f, 1.0f },
			//	{ xpos + w, ypos + h,   1.0f, 0.0f },       
			//};

			
			
			//glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
			

			//PROFILER { TracyGpuZone ("Text drawFunc"); }
			

			x += (character.advance >> 6) * scaleX;
			++workingIndex;
		}

		DEBUG_RENDER GL::GetError (1242);
		glUniformMatrix4fv (glGetUniformLocation (program.id, "projection"), 1, GL_FALSE, &projection[0][0]);
		DEBUG_RENDER GL::GetError (1241);
		glUniformMatrix4fv (glGetUniformLocation (program.id, "models"), workingIndex, GL_FALSE, &models[0][0][0]);
		DEBUG_RENDER GL::GetError (1240);
		glUniform1iv (glGetUniformLocation (program.id, "letterMap"), workingIndex, &letterMap[0]);
		DEBUG_RENDER GL::GetError (1239);
		glUniform4f (glGetUniformLocation (program.id, "color"), color.v1, color.v2, color.v3, color.v4); 
		DEBUG_RENDER GL::GetError (1238);

		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D_ARRAY, textureArray);
		glBindVertexArray (vao);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);

		// 52:48 HACK THis might go so much sideways..
		//SHADER::UNIFORM::BUFFORS::model = models[0][0][0];
		SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

		const u8 verticesCount = 4;
		glDrawArraysInstanced (GL_TRIANGLE_STRIP, 0, verticesCount, workingIndex);
		DEBUG_RENDER GL::GetError (1237);

		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		DEBUG_RENDER GL::GetError (1238);
	}

}