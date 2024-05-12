#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TEXTURE {

	// TODO
	// 1. Put formatSource into TextureHolder a simple converter is needed.

	// Why differensing TextureAtlas and ArrayTexture?
	// - http://gaarlicbread.com/post/gl_2d_array

	// To temporary store CPU only needed data.
	struct Holder {
		u8* data;
		GLint channelsCount;
		GLint width;
		GLint height;
	};

	const u8 CUBE_FACES_COUNT = 6;
	using HolderCube = Holder[6];

	// Helper for atlas & array textures.
	struct Atlas {
		u8 elementsCount;
		u8 cols;
		u8 rows;
		u8 tileSizeX;
		u8 tileSizeY;
	};

}

namespace TEXTURE::PROPERTIES {

	// To temporary store GPU only needed data.
	struct Properties {
		GLint format;		// How is it stored after loading when in GPU memory.
		GLint mipmapLevels;	// We can specify manually how many we want.
		GLint wrapX;		// Define how do we treat UV values lower then 0 higher then 1.
		GLint wrapY;		// ...
		GLint filterMin;	// Define what happends when the rendered texture is smaller/bigger.
		GLint filterMax;	// ...
	};

	const Properties defaultRGBA 		{ GL_RGBA8, 0, GL_REPEAT, 		 GL_REPEAT, 	   GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR  };
	const Properties defaultRGB 		{ GL_RGB8,  0, GL_REPEAT, 		 GL_REPEAT, 	   GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST };
	const Properties alphaPixelNoMipmap { GL_RGBA8, 1, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, 				 GL_NEAREST };

}

namespace TEXTURE {

	// TEXTURE FORMAT TYPES !
	// GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA,
	// GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, GL_BGRA_INTEGER,
	// GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL

	void ChannelsCountToSourceFormat (
		/* IN  */ const GLint& channelsCount,
		/* OUT */ GLint& sourceFormat
	) {
		switch (channelsCount) {
			case 1:
				sourceFormat = GL_RED;
				break;
			case 2:
				sourceFormat = GL_RG;
				break;
			case 3:
				sourceFormat = GL_RGB;
				break;
			case 4:
				sourceFormat = GL_RGBA;
				break;
			case 0:
			default:
				DEBUG spdlog::info ("Silent ERROR while converting from ChannelsCount to FormatSource on TextureLoading");
				sourceFormat = 0;
				break;
		}
	}

	// Loads a Texture from a file to binary form that yet needs to be send to the GPU memory.
	//  To do so call the Create function. 
	void Load ( Holder& textureHolder, const char* filepath ) {
        ZoneScopedN("TEXTURE: Load");

		textureHolder.data = stbi_load (filepath, &textureHolder.width, &textureHolder.height, &textureHolder.channelsCount, 0);
		DEBUG if (textureHolder.data == nullptr) {
			spdlog::error ("Could not find the texture under specified filepath!");
			exit (1);
		}
	}

}

namespace TEXTURE::SINGLE {

	void Create (
		/* OUT */ GLuint& textureId,
		/* IN  */ Holder& textureHolder,
		/* IN  */ const PROPERTIES::Properties& properties
	) {
        ZoneScopedN("TEXTURE::SINGLE: Create");

		// It is formatted in bytes. The way color in source is being safed. 
		const GLenum SOURCE_TYPE = GL_UNSIGNED_BYTE;
		
		glGenTextures (1, &textureId);
		glBindTexture (GL_TEXTURE_2D, textureId);  

		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties.wrapX);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties.wrapY);

		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, properties.filterMin);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, properties.filterMax);

		GLint formatSource;
		ChannelsCountToSourceFormat (textureHolder.channelsCount, formatSource);

		// Generate GPU texture.
		glTexImage2D (
			GL_TEXTURE_2D, properties.mipmapLevels, properties.format, 									// GPU
			textureHolder.width, textureHolder.height, 0, formatSource, SOURCE_TYPE, textureHolder.data // CPU
		); DEBUG_RENDER GL::GetError (1101);

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
		/* IN  */ const PROPERTIES::Properties& properties,
		/* IN  */ const Atlas& atlas
	) {
        ZoneScopedN("TEXTURE::ARRAY: 2");

		// It is formatted in bytes. The way color in source is being safed. 
		const GLenum SOURCE_TYPE = GL_UNSIGNED_BYTE;

		glGenTextures (1, &textureId);
		glBindTexture (GL_TEXTURE_2D_ARRAY, textureId);

		glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, properties.wrapX);
		glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, properties.wrapY);

		glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, properties.filterMin);
		glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, properties.filterMax);

		// glTexStorage3D does not know how big the image is and therefore we need to inform it?.
		glPixelStorei (GL_UNPACK_ROW_LENGTH, textureHolder.width);
		DEBUG_RENDER GL::GetError (1111);

		GLint formatSource;
		ChannelsCountToSourceFormat (textureHolder.channelsCount, formatSource);

		// COPY
		GLsizei height = atlas.tileSizeY;
		GLsizei width = atlas.tileSizeX;

		assert(properties.mipmapLevels == 1);

		for (GLint level = 0; level < properties.mipmapLevels; level++) {

        	glTexImage3D (GL_TEXTURE_2D_ARRAY, 
				level, properties.format, 
				height, height, atlas.elementsCount, 
				0, formatSource, GL_UNSIGNED_BYTE, NULL
			);

			DEBUG_RENDER GL::GetError (1112);

        	height = fmax (1, (height / 2));
			width = fmax (1, (width / 2));
    	}

		// When chaning on Y axis we need to jump by that amount of bytes.
		const u64 wholeRow = textureHolder.channelsCount * atlas.tileSizeX * atlas.tileSizeY * atlas.cols;
		u8 row = 0, col = 0;

		for (u8 element = 0; element < atlas.elementsCount; ++element) {
			const u8 index = (atlas.cols * row) + col;
			const u64 offsetX = textureHolder.channelsCount * atlas.tileSizeX * col;
			const u64 offsetY = wholeRow * row;
			//
			glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 
				0, 0, 0, index, atlas.tileSizeX, atlas.tileSizeY, 1, 
				formatSource, GL_UNSIGNED_BYTE, textureHolder.data + offsetX + offsetY
			); DEBUG_RENDER  GL::GetError (1113);
			//
			++col;
			u8 condition = col == atlas.cols;
			col *= (!condition); // Resets columnsCounter when counter equal max.
			row += (condition);  // Increments rowsCounter by one when counter equal max.
		}

		// We need to restore it to the default state...
		glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
		
	}

}

namespace TEXTURE::CUBEMAP {

	void Create (
		/* OUT */ GLuint& textureId,
		/* IN  */ HolderCube& textureHolders,
		/* IN  */ const PROPERTIES::Properties& properties
	) {
		ZoneScopedN ("TEXTURE::CUBEMAP: Create");

		glGenTextures (1, &textureId);
		glBindTexture (GL_TEXTURE_CUBE_MAP, textureId);

		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		GLint formatSource;

		for (u8 face = 0; face < CUBE_FACES_COUNT; ++face) {
			auto& height = textureHolders[face].height;
			auto& width = textureHolders[face].width;
			auto&& data = textureHolders[face].data;

			ChannelsCountToSourceFormat (textureHolders[face].channelsCount, formatSource);

			glTexImage2D ( 
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 
				GL_RGB, width, height, 0,
				formatSource, GL_UNSIGNED_BYTE, data
			); DEBUG_RENDER GL::GetError (333);
		}
	}

}

namespace TEXTURE::MODEL {
    struct Texture {
        GLuint ID;
        const char* type;
        GLuint unit;
    };

    TEXTURE::MODEL::Texture Create(const char* image, const char* texType, GLuint slot)
    {
        TEXTURE::MODEL::Texture texture{};

        // Assigns the type of the texture ot the texture object
        texture.type = texType;

        // Stores the width, height, and the number of color channels of the image
        int widthImg, heightImg, numColCh;
        // Flips the image so it appears right side up
        stbi_set_flip_vertically_on_load(true);
        // Reads the image from a file and stores it in bytes
        unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

        // Generates an OpenGL texture object
        glGenTextures(1, &texture.ID);
        // Assigns the texture to a Texture Unit
        glActiveTexture(GL_TEXTURE0 + slot);
        texture.unit = slot;
        glBindTexture(GL_TEXTURE_2D, texture.ID);

        // Configures the type of algorithm that is used to make the image smaller or bigger
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Configures the way the texture repeats (if it does at all)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Extra lines in case you choose to use GL_CLAMP_TO_BORDER
        // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

        // Check what type of color channels the texture has and load it accordingly
        if (numColCh == 4)
            glTexImage2D
                    (
                            GL_TEXTURE_2D,
                            0,
                            GL_RGBA,
                            widthImg,
                            heightImg,
                            0,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            bytes
                    );
        else if (numColCh == 3)
            glTexImage2D
                    (
                            GL_TEXTURE_2D,
                            0,
                            GL_RGBA,
                            widthImg,
                            heightImg,
                            0,
                            GL_RGB,
                            GL_UNSIGNED_BYTE,
                            bytes
                    );
        else if (numColCh == 1)
            glTexImage2D
                    (
                            GL_TEXTURE_2D,
                            0,
                            GL_RGBA,
                            widthImg,
                            heightImg,
                            0,
                            GL_RED,
                            GL_UNSIGNED_BYTE,
                            bytes
                    );
        else
            throw std::invalid_argument("Automatic Texture type recognition failed");

        // Generates MipMaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Deletes the image data as it is already in the OpenGL Texture object
        stbi_image_free(bytes);

        // Unbinds the OpenGL Texture object so that it can't accidentally be modified
        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }

}
