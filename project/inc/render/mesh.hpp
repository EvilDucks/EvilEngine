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

	enum class TYPE : u8 {
		V 		= 1, /* VERTEX */
		VC 		= 2, /* VERTEX_COLOR */
		VI		= 2, /* VERTEX_INDEX */
		VCI 	= 3, /* VERTEX_INDEX_COLOR */
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

}


namespace MESH::DD::TRIANGLE {

	const u8 VERTICES_COUNT = 3;

	const GLfloat VERTICES[] {
		-0.5f, -0.5f, 0.0f,
	 	 0.5f, -0.5f, 0.0f,
	 	 0.0f,  0.5f, 0.0f,
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


namespace MESH::DD::V {

	void CreateVAO (
		/*OUT*/	GLuint& vao,
		/*OUT*/ GLuint* buffers,
		/*IN */	const u64& verticesSize,
		/*IN */	const GLfloat* vertices
	) {
		auto& vbo = buffers[0];

		glGenVertexArrays (1, &vao); 
		glGenBuffers (1, &vbo);

		/* i */ glBindVertexArray (vao);
		/* i */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* i */ glBufferData (GL_ARRAY_BUFFER, verticesSize * VERTEX * UNIT_SIZE, vertices, GL_STATIC_DRAW);

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
		const u8 OFFSET = 0;
		glDrawArrays(mode, OFFSET, count);
	}

}


namespace MESH::DD::VI {

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