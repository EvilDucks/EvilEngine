#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TEXTURE {

	// Why differensing TextureAtlas and ArrayTexture?
	// - http://gaarlicbread.com/post/gl_2d_array

	// To temporary store CPU only needed data.
	struct Holder {
		u8* data;
		GLint channelsCount;
		GLint width;
		GLint height;
	};

	// To temporary store GPU only needed data.
	struct Properties {
		GLint format;		// How is it stored after loading when in GPU memory.
		GLint mipmapLevels;	// We can specify manually how many we want.
		GLint wrapX;		// Define how do we treat UV values lower then 0 higher then 1.
		GLint wrapY;		// ...
		GLint filterMin;	// Define what happends when the rendered texture is smaller/bigger.
		GLint filterMax;	// ...
	};

	// Helper for atlas & array textures.
	struct Atlas {
		u8 elementsCount;
		u8 tileSizeX;
		u8 tileSizeY;
	};

}

namespace TEXTURE::SINGLE {

	void Create (
		/* OUT */ GLuint& textureId,
		/* IN  */ Holder& textureHolder,
		/* IN  */ const GLint& formatSource,    // Source might be serialized as with "Alpha" channel or "monocolor" or other.
		/* IN  */ const Properties& properties
	) {		
		// It is formatted in bytes. The way color in source is being safed. 
		const GLenum SOURCE_TYPE = GL_UNSIGNED_BYTE;
		
		glGenTextures (1, &textureId);
		glBindTexture (GL_TEXTURE_2D, textureId);  

		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties.wrapX);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties.wrapY);

		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, properties.filterMin);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, properties.filterMax);

		// Generate GPU texture.
		glTexImage2D (
			GL_TEXTURE_2D, properties.mipmapLevels, properties.format, 									// GPU
			textureHolder.width, textureHolder.height, 0, formatSource, SOURCE_TYPE, textureHolder.data // CPU
		);

		// Generate mipmap textures.
		glGenerateMipmap (GL_TEXTURE_2D);

		// It's in GPU memory so clear the CPU memory now.
		stbi_image_free (textureHolder.data);
	}

}

namespace TEXTURE::ARRAY {

	void Create (
		/* OUT */ GLuint& textureId,
		/* IN  */ Holder& textureHolder,
		/* IN  */ const GLint& formatSource,
		/* IN  */ const Properties& properties,
		/* IN  */ const Atlas& atlas
	) {
		// It is formatted in bytes. The way color in source is being safed. 
		const GLenum SOURCE_TYPE = GL_UNSIGNED_BYTE;

		GLint& height = textureHolder.height;
		GLint& width = textureHolder.width;
		GLuint elementsCount = 2;

		glGenTextures (1, &textureId);
		glBindTexture (GL_TEXTURE_2D_ARRAY, textureId);

		glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, properties.wrapX);
		glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, properties.wrapY);

		glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, properties.filterMin);
		glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, properties.filterMax);

		glTexStorage3D (GL_TEXTURE_2D_ARRAY, 
			properties.mipmapLevels, properties.format, 
			atlas.tileSizeX, atlas.tileSizeY, atlas.elementsCount
		);
		DEBUG_RENDER GL::GetError (1111);

		// glTexStorage3D does not know how big the image is and therefore we need to inform it?.
		glPixelStorei (GL_UNPACK_ROW_LENGTH, textureHolder.width);
		DEBUG_RENDER GL::GetError (1112);

		//glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
		//	0, 0, 0, 0, atlas.tileSizeX, atlas.tileSizeY, atlas.elementsCount, 
		//	GL_RGBA, GL_UNSIGNED_BYTE, textureHolder.data
		//);
		//DEBUG_RENDER GL::GetError (2222);

		//glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
		//	0, 0, 0, 0, atlas.tileSizeX, atlas.tileSizeY, 1, 
		//	GL_RGBA, GL_UNSIGNED_BYTE, textureHolder.data + (4 * 16) * 0
		//);
		////
		//glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
		//	0, 0, 0, 1, atlas.tileSizeX, atlas.tileSizeY, 1, 
		//	GL_RGBA, GL_UNSIGNED_BYTE, textureHolder.data + (4 * 16) * 1
		//);
		////
		//glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
		//	0, 0, 0, 2, atlas.tileSizeX, atlas.tileSizeY, 1, 
		//	GL_RGBA, GL_UNSIGNED_BYTE, textureHolder.data + (4 * 16) * 2
		//);
		////
		//glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
		//	0, 0, 0, 3, atlas.tileSizeX, atlas.tileSizeY, 1, 
		//	GL_RGBA, GL_UNSIGNED_BYTE, textureHolder.data + (4 * 16) * 3
		//);
		////
		//glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
		//	0, 0, 0, 4, atlas.tileSizeX, atlas.tileSizeY, 1, 
		//	GL_RGBA, GL_UNSIGNED_BYTE, textureHolder.data + (4 * 16) * 4
		//);
		////
		//glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
		//	0, 0, 0, 5, atlas.tileSizeX, atlas.tileSizeY, 1, 
		//	GL_RGBA, GL_UNSIGNED_BYTE, textureHolder.data + (4 * 16) * 5
		//);

		const u8 columns = 6;
		const u8 rows = 1;
		for (u8 row = 0; row < rows; ++row) {
    		for (u8 col = 0; col < columns; ++col) {
				const u8 index = (columns * row) + col;
				const u64 offset = 4 * 16 * index; //((row * atlas.tileSizeY * textureHolder.width) + (col * atlas.tileSizeX)) * rows;
				spdlog::info (offset);
				glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
					0, 0, 0, index, atlas.tileSizeX, atlas.tileSizeY, 1, 
					GL_RGBA, GL_UNSIGNED_BYTE, textureHolder.data + offset
				);
			}
		}


		// We need to restore it to the default state...
		glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	}

}

namespace TEXTURE {

	// Loads a Texture from a file to binary form that yet needs to be send to the GPU memory.
	//  To do so call the Create function. 
	void Load ( Holder& textureHolder, const char* filepath ) {
		textureHolder.data = stbi_load (filepath, &textureHolder.width, &textureHolder.height, &textureHolder.channelsCount, 0);
		DEBUG if (textureHolder.data == nullptr) {
			spdlog::error ("Could not find the texture under specified filepath!");
			exit (1);
		}
	}

}