#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

// for now only...
#include "shader.hpp"

// FreeType inc
#include <ft2build.h>
#include FT_FREETYPE_H

namespace FONT {

	// THE WHOLE THING NEEDS TO BE OPTIMIZED!

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


	void RenderText (
		const u16& textCount,
		const char* const text,
		float x, float y, 
		float scale
	) {
		auto& VAO = FONT::faceVAO;
		auto& VBO = FONT::faceVBO;
		
    	glBindVertexArray (VAO);

		for (u16 i = 0; i < textCount; ++i) {
			const char sign = text[i];
			const FONT::Character character = FONT::characters[sign];
			//
			const float xpos = x + character.bearing.x * scale;
			const float ypos = y - (character.size.y - character.bearing.y) * scale;
			//
			const float w = character.size.x * scale;
			const float h = character.size.y * scale;
			//
			const float vertices[6][4] = {
    	        { xpos,     ypos + h,   0.0f, 0.0f },            
    	        { xpos,     ypos,       0.0f, 1.0f },
    	        { xpos + w, ypos,       1.0f, 1.0f },
				//
    	        { xpos,     ypos + h,   0.0f, 0.0f },
    	        { xpos + w, ypos,       1.0f, 1.0f },
    	        { xpos + w, ypos + h,   1.0f, 0.0f },       
    	    };
    	    glBindTexture (GL_TEXTURE_2D, character.textureId);
    	    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    	    glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
    	    glBindBuffer (GL_ARRAY_BUFFER, 0);
    	    glDrawArrays (GL_TRIANGLES, 0, 6);
    	    x += (character.advance >> 6) * scale;
		}
		glBindVertexArray(0);
    	glBindTexture(GL_TEXTURE_2D, 0);
	}

}