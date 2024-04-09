#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"


// VBO - Vertex Buffer Objects.
// EBO - Element Buffer Objects.
// VAO - Vertex Array Object.

// - VBO
// Here we store vertex data within memory on the graphics card.

// - GL_STATIC_DRAW -> The position data of the triangle does not 
//  change, is used a lot, and stays the same.

// - VAO
// Stores information about many VERTEX_ATTRIBUTE_LOCATIONs where 
//  each is defined by a different VBO.

namespace MESH {

	const u8 UNIT_SIZE = 4; // -> GLfloat
	const u8 VERTEX = 3;	// -> x, y, z directions
	const u8 UV_SIZE = 2;

	enum class TYPE : u8 {
		V 		= 1, /* VERTEX */
		VC 		= 2, /* VERTEX_COLOR */
		VI		= 2, /* VERTEX_INDEX */
		VCI 	= 3, /* VERTEX_INDEX_COLOR */
		VIT 	= 4, /* VERTEX_INDEX_COLOR_TEXTURE */
	};

	using DrawFunc = void (*)(GLenum mode, GLsizei count);

	struct Base {
		GLuint vao = 0;
		GLsizei verticiesCount = 0;
		GLsizei buffersCount = 0;
		GLuint buffers[2] { 0 };
		DrawFunc drawFunc = nullptr;
	};


	struct Mesh {
		GameObjectID id = 0;
		Base base { 0 };
	};


	GLuint texture1 = 0; GLuint texture1UniformLocation = 0;
	GLuint texture2 = 0; GLuint texture2UniformLocation = 0;

}


namespace MESH::DD::TRIANGLE {

	const u8 VERTICES_COUNT = 3;

	const GLfloat VERTICES[] {
		-0.5f, -0.5f, 0.0f,
	 	 0.5f, -0.5f, 0.0f,
	 	 0.0f,  0.5f, 0.0f,
	};

	const GLfloat UV[] {
		0.0f, 0.0f,  // lower-left corner  
    	1.0f, 0.0f,  // lower-right corner
    	0.5f, 1.0f 	 // upper corner
	};

}

namespace MESH::DD::SAMPLE1_SQUARE {

	// corner_position
	const float RCP = 0.75f;
	const float TCP = 0.75f;
	const float LCP = 0.50f;
	const float BCP = 0.50f;

	const GLfloat VERTICES[] {
		TCP, RCP, 0.0f,			// top,    right
		RCP, BCP, 0.0f,			// right,  bottom
		BCP, LCP, 0.0f,			// bottom, left
		LCP, TCP, 0.0f,			// left,   top  
	};

	const GLfloat VERTICES_UV[] {
		TCP, RCP, 0.0f, 1.0f, 1.0f,		// top,    right
		RCP, BCP, 0.0f,	1.0f, 0.0f,		// bottom, right
		BCP, LCP, 0.0f,	0.0f, 0.0f,		// bottom, left
		LCP, TCP, 0.0f,	0.0f, 1.0f,		// top,    left 
	};

}

namespace MESH::DD::SAMPLE2_SQUARE {

	// corner_position
	const float RCP = -0.50f;
	const float TCP = -0.50f;
	const float LCP = -0.75f;
	const float BCP = -0.75f;

	const GLfloat VERTICES[] {
		TCP, RCP, 0.0f,			// top,    right
		RCP, BCP, 0.0f,			// right,  bottom
		BCP, LCP, 0.0f,			// bottom, left
		LCP, TCP, 0.0f,			// left,   top  
	};

	const GLfloat VERTICES_UV[] {
		TCP, RCP, 0.0f, 1.0f, 1.0f,		// top,    right
		RCP, BCP, 0.0f,	1.0f, 0.0f,		// bottom, right
		BCP, LCP, 0.0f,	0.0f, 0.0f,		// bottom, left
		LCP, TCP, 0.0f,	0.0f, 1.0f,		// top,    left 
	};

}

namespace MESH::DD::FULL_SQUARE {

	// corner_position
	const float RCP =  1.0f;
	const float TCP =  1.0f;
	const float LCP = -1.0f;
	const float BCP = -1.0f;

	const GLfloat VERTICES[] {
		TCP, RCP, 0.0f,			// top,    right
		RCP, BCP, 0.0f,			// right,  bottom
		BCP, LCP, 0.0f,			// bottom, left
		LCP, TCP, 0.0f,			// left,   top  
	};

	const GLfloat VERTICES_UV[] {
		TCP, RCP, 0.0f, 1.0f, 1.0f,		// top,    right
		RCP, BCP, 0.0f,	1.0f, 0.0f,		// bottom, right
		BCP, LCP, 0.0f,	0.0f, 0.0f,		// bottom, left
		LCP, TCP, 0.0f,	0.0f, 1.0f,		// top,    left 
	};

}


namespace MESH::DD::SQUARE {

	const u8 VERTICES_COUNT = 4;

	const GLfloat VERTICES[] {
		 0.5f,  0.5f, 0.0f,			// top,    right
		 0.5f, -0.5f, 0.0f,			// bottom, right
		-0.5f, -0.5f, 0.0f,			// bottom, left
		-0.5f,  0.5f, 0.0f,			// top,    left 
	};


	const u8 INDICES_COUNT = 6;

	const GLuint INDICES[] {
		0, 1, 3,					// first Triangle
		1, 2, 3,					// second Triangle
	};

	const GLfloat VERTICES_UV[] {
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,		// top,    right
		 0.5f, -0.5f, 0.0f,	1.0f, 0.0f,		// bottom, right
		-0.5f, -0.5f, 0.0f,	0.0f, 0.0f,		// bottom, left
		-0.5f,  0.5f, 0.0f,	0.0f, 1.0f,		// top,    left 
	};

}


namespace MESH::DDD::CUBE {

	const u8 VERTICES_COUNT = 36;

	const GLfloat VERTICES[] {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};

}


namespace MESH::V {

	void CreateVAO (
		/*OUT*/	GLuint& vao,
		/*OUT*/ GLuint* buffers,
		/*IN */	const u64& verticesSize,
		/*IN */	const GLfloat* vertices
	) {
		auto& vbo = buffers[0];

		glGenVertexArrays (1, &vao);
		DEBUG_RENDER GL::GetError (0);
		glGenBuffers (1, &vbo);
		DEBUG_RENDER GL::GetError (1);

		/* i */ glBindVertexArray (vao);
		/* i */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* i */ glBufferData (GL_ARRAY_BUFFER, verticesSize * VERTEX * UNIT_SIZE, vertices, GL_STATIC_DRAW);
		/* i */ DEBUG_RENDER GL::GetError (2);

		// Tell GL how to treat vertices pointer.
		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		// In this line VBO gets connected to VAO based on previous glBindVertexArray call.
		/* i */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 3 * UNIT_SIZE, (void*)0);
		/* i */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/* i */ DEBUG_RENDER GL::GetError (3);

		// Not needed -> Unbind!
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count) {
		const u8 OFFSET = 0;

		//glBindTexture(GL_TEXTURE_2D, texture);
		//glBindTexture (GL_TEXTURE_2D, 0);
		glDrawArrays (mode, OFFSET, count);
		//glBindTexture(GL_TEXTURE_2D, 0);
		DEBUG_RENDER GL::GetError (4);
	}

}


namespace MESH::VI {

	void CreateVAO (
		/*OUT*/	GLuint& vao,
		/*OUT*/ GLuint* buffers,
		/*IN */	const u64& verticesSize,
		/*IN */	const GLfloat* vertices,
		/*IN */	const u64& indicesSize,
		/*IN */	const GLuint* indices
	) {

		auto& vbo = buffers[0];
		auto& ebo = buffers[1];

		glGenVertexArrays (1, &vao);
		glGenBuffers (2, buffers);

		/* i */ glBindVertexArray (vao);
		/* i */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* i */ glBufferData (GL_ARRAY_BUFFER, verticesSize * VERTEX * UNIT_SIZE, vertices, GL_STATIC_DRAW);
		/* j */ glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo); // We do not unbind it!
		/* j */ glBufferData (GL_ELEMENT_ARRAY_BUFFER, indicesSize * UNIT_SIZE, indices, GL_STATIC_DRAW);

		// Tell GL how to treat vertices pointer.
		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		// In this line VBO gets connected to VAO based on previous glBindVertexArray call.
		/* i */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 3 * UNIT_SIZE, (void*)0);
		/* i */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);

		// Not needed -> Unbind! 
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count) {
		const void* USING_VBO = nullptr;
		glDrawElements(mode, count, GL_UNSIGNED_INT, USING_VBO);
		DEBUG_RENDER GL::GetError (1000);
	}

}


namespace MESH::VIT {

	void CreateVAO (
		/*OUT*/	GLuint& vao,
		/*OUT*/ GLuint* buffers,
		/*IN */	const u64& verticesSize,
		/*IN */	const GLfloat* vertices,
		/*IN */	const u64& indicesSize,
		/*IN */	const GLuint* indices
	) {
		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		const u64 SAMPLER_ATTRIBUTE_LOCATION_1 = 1;

		auto& vbo = buffers[0];
		auto& ebo = buffers[1];

		glGenVertexArrays (1, &vao);
		glGenBuffers (2, buffers);

		/* i */ glBindVertexArray (vao);
		/* i */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* i */ glBufferData (GL_ARRAY_BUFFER, verticesSize * (VERTEX + UV_SIZE) * UNIT_SIZE, vertices, GL_STATIC_DRAW);
		/* i */ DEBUG_RENDER GL::GetError (10);

		/* j */ glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo); // We do not unbind it!
		/* j */ glBufferData (GL_ELEMENT_ARRAY_BUFFER, indicesSize * UNIT_SIZE, indices, GL_STATIC_DRAW);
		/* j */ DEBUG_RENDER GL::GetError (11);

		// In this line VBO gets connected to VAO based on previous glBindVertexArray call.
		/* i */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 5 * UNIT_SIZE, (void*)0);
		/* i */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/* i */ DEBUG_RENDER GL::GetError (12);

		/* a */ glVertexAttribPointer (SAMPLER_ATTRIBUTE_LOCATION_1, /* f2 */ 2, GL_FLOAT, GL_FALSE, 5 * UNIT_SIZE, (void*)(3 * sizeof (float)));
		/* a */ glEnableVertexAttribArray (SAMPLER_ATTRIBUTE_LOCATION_1);
		/* a */ DEBUG_RENDER GL::GetError (13);

		// Not needed -> Unbind! 
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count) {
		const void* USING_VBO = nullptr;
		//glBindTexture(GL_TEXTURE_2D, texture1);
		glDrawElements(mode, count, GL_UNSIGNED_INT, USING_VBO);
		glBindTexture(GL_TEXTURE_2D, 0);
		DEBUG_RENDER GL::GetError (14);
	}

}


namespace MESH {

	void DestroyVAO (
		/*IN */ const GLuint& vao,
		/*IN */ const u64& buffersCount,
		/*IN */ const GLuint*& buffers
	) {
		glDeleteVertexArrays (1, &vao);
		glDeleteBuffers (buffersCount, buffers);
	}

}