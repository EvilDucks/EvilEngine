#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

// for now only...
#include "shader.hpp"

// FreeType inc
#include <ft2build.h>
#include FT_FREETYPE_H

namespace FONT {

	GLuint faceVAO, faceVBO;
	SHADER::Shader faceShader {}; // initializes

	struct Character {
		GLuint		textureId;  // ID handle of the glyph texture
		glm::ivec2	size;       // Size of glyph
		glm::ivec2	bearing;    // Offset from baseline to left/top of glyph
		FT_Pos		advance;    // Offset to advance to next glyph
	};

	// 'x' -> x, we def. can do better... soon
	std::map<char, Character> characters;


	void Create (
		const FT_Face& face
	) {
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

}