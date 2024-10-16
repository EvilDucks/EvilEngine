#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

// for now only...
#include "shader.hpp"

// FreeType inc
#include <ft2build.h>
#include FT_FREETYPE_H

namespace FONT {

	glm::mat4 model1 = glm::mat4(1.0f);
	glm::mat4 model2 = glm::mat4(1.0f);

	// THE WHOLE THING NEEDS TO BE OPTIMIZED!

	//GLuint faceVAO, faceVBO;
	//SHADER::Shader faceShader {}; // initializes

	struct Character {
		GLuint		textureId;  // ID handle of the glyph texture
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
		FT_Set_Pixel_Sizes (face, 0, 48);
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

		u32 errorCode;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction -> GL_RED connected
		//
		for (unsigned char sign = 0; sign < 128; sign++) {
			// load character glyph 
			errorCode = FT_Load_Char (face, sign, FT_LOAD_RENDER);
			//
			DEBUG if ( errorCode ) {
				spdlog::error ("FREETYTPE: Failed to load Glyph nr: {0}", (u8)(sign));
				continue;
			}
			//
			// generate texture & set texture options
			GLuint texture;
			glGenTextures (1, &texture);
			glBindTexture (GL_TEXTURE_2D, texture);
			glTexImage2D (
				GL_TEXTURE_2D,
				0,
				GL_RED, // 8-bit grayscale bitmap
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,      
				GL_RED, // 8-bit grayscale bitmap
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//
			// Store the final character for use later.
			Character character = {
				texture, 
				glm::ivec2 (face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2 (face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			//
			characters.insert ( std::pair<char, Character> (sign, character) );
		}
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
		/* IN  */ const r32 scaleX, const r32 scaleY,
		//
		const GLuint& vao, 
		const SHADER::Shader& program, 
		const u16& uniformsCount, 
		SHADER::UNIFORM::Uniform*& uniforms
	) {
		PROFILER { ZoneScopedN("Font: RenderText"); }

		const r32 lineHeight = 1.3;

		auto& vbo = buffers[0];
		const r32 dx = x;	// default x

		for (u16 i = 0; i < textCount; ++i) {
			const char sign = text[i];
			const FONT::Character character = FONT::characters[sign];


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
			
			glm::mat4 model = 
				glm::translate (glm::mat4(1.0f), glm::vec3 (xpos, ypos, 0.0f)) * 
				glm::scale (glm::mat4(1.0f), glm::vec3 (character.size.x * scaleX, character.size.y * scaleY, 0.0f))
			;

			SHADER::UNIFORM::BUFFORS::model = model;

			SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

			glBindVertexArray (vao);

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

			glBindTexture (GL_TEXTURE_2D, character.textureId);
			glBindBuffer (GL_ARRAY_BUFFER, vbo);
			//glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
			

			//PROFILER { TracyGpuZone ("Text drawFunc"); }
			const u8 verticesCount = 4;

			glDrawArraysInstanced (GL_TRIANGLE_STRIP, 0, verticesCount, 1);

			x += (character.advance >> 6) * scaleX;
		}
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		DEBUG_RENDER GL::GetError (1236);
	}

}