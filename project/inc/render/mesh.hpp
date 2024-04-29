#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

#include "temp/Sphere.h"


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
		glm::vec3 boundsMin;
		glm::vec3 boundsMax;
		float boundsRadius;
	};


	struct Mesh {
		GameObjectID id = 0;
		Base base { 0 };
	};


	GLuint texture1 = 0; //GLuint texture1UniformLocation = 0;
	GLuint texture2 = 0; //GLuint texture2UniformLocation = 0;
	GLuint textureAtlas1 = 0; //GLuint textureAtlas1UniformLocation = 0;

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


namespace MESH::DDD::SKYBOX {

	const u8 VERTICES_COUNT = 36;

	const GLfloat VERTICES[] {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

}


namespace MESH::DDD::DSPHERE { // Dynamic - which means it factors are being calculated.

	//u8 longitude = 0;
	//u8 latitude = 0;

	void DestroyVertices (
		GLfloat*& vertices
	) {
		delete[] vertices;
	}

	void CreateVertices (
		u16& vertexesCount, 
		GLfloat*& vertices, 
		const u16& sectorCount, 
		const u16& stackCount, 
		const r32& radius
	) {
		vertexesCount = 3 * sectorCount;
		const u16 verticesCount = vertexesCount * 3;
		vertices = (GLfloat*) malloc (verticesCount * sizeof (GLfloat));

		// TODO
		// 1. Gen a triangle
		// 2. Gen a circle
		// 3. Gen a cone
		// 4. Gen a cylinder
		// 5. Gen a Sphere (longitude & latidute)
		// 6. Gen a Sphere (IcoSphere)
		// 7. Gen a Sphere (CubeSphere)

		// BACK
		//{ // To generate a simple triangle now.
		//	// Left
		//	vertices[0] = -1.0f;
		//	vertices[1] =  0.0f;
		//	vertices[2] = -1.0f;
		//	// Right
		//	vertices[3] =  1.0f;
		//	vertices[4] =  0.0f;
		//	vertices[5] = -1.0f;
		//	// UP
		//	vertices[6] =  0.0f;
		//	vertices[7] =  0.0f;
		//	vertices[8] =  1.0f;
		//}
		const float PI = 3.1415926f;
		const float angleJump = 2 * PI / sectorCount;

		//float a1 = 0;
		//float x1 = 0 + radius * cosf(a1);
		//float y1 = 0 + radius * sinf(a1);
		//float a2 = 0 + angleJump;
		//float x2 = 0 + radius * cosf(a2);
		//float y2 = 0 + radius * sinf(a2);
		//spdlog::info ("x: {0}, y: {1}", x1, y1);
		//spdlog::info ("x: {0}, y: {1}", x2, y2);

		for (u16 i = 0; i < sectorCount; ++i) {
			const u64 offset = 9 * i;

			float a1 = angleJump * i;
			float x1 = 0 + radius * cosf(a1);
			float y1 = 0 + radius * sinf(a1);

			// Left
			vertices[0 + offset] =  x1;
			vertices[1 + offset] =  0.0f;
			vertices[2 + offset] =  y1;

			a1 = a1 + angleJump;
			x1 = 0 + radius * cosf(a1);
			y1 = 0 + radius * sinf(a1);

			// Right
			vertices[3 + offset] =  x1;
			vertices[4 + offset] =  0.0f;
			vertices[5 + offset] =  y1;
			
			// MID
			vertices[6 + offset] =  0.0f;
			vertices[7 + offset] =  0.0f;
			vertices[8 + offset] =  0.0f;
		}
		
	}
	
	//void CreateVertices (
	//	std::vector<GLfloat>& vertices,
	//	std::vector<GLuint>& indices,
	//	std::vector<GLfloat>& normals,
	//	std::vector<GLfloat>& texCoords,
	//	const u16& stackCount, 
	//	const u16& sectorCount, 
	//	const r32& radius
	//) {
	//	const r32 PI = acos(-1.0f);
	//	//
	//	float x, y, z, xy;                              // vertex position
	//	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	//	float s, t;                                     // vertex texCoord
	//	//
	//	float sectorStep = 2 * PI / sectorCount;
	//	float stackStep = PI / stackCount;
	//	float sectorAngle, stackAngle;
	//	//
	//	for (int i = 0; i <= stackCount; ++i) {
	//		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
	//		xy = radius * cosf(stackAngle);             // r * cos(u)
	//		z = radius * sinf(stackAngle);              // r * sin(u)
	//		//
	//		// add (sectorCount+1) vertices per stack
	//		// first and last vertices have same position and normal, but different tex coords
	//		for (int j = 0; j <= sectorCount; ++j) {
	//			sectorAngle = j * sectorStep;           // starting from 0 to 2pi
	//			//
	//			// vertex position (x, y, z)
	//			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
	//			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
	//			vertices.push_back(x);
	//			vertices.push_back(y);
	//			vertices.push_back(z);
	//			//
	//			// normalized vertex normal (nx, ny, nz)
	//			nx = x * lengthInv;
	//			ny = y * lengthInv;
	//			nz = z * lengthInv;
	//			normals.push_back(nx);
	//			normals.push_back(ny);
	//			normals.push_back(nz);
	//			//
	//			// vertex tex coord (s, t) range between [0, 1]
	//			s = (float)j / sectorCount;
	//			t = (float)i / stackCount;
	//			texCoords.push_back(s);
	//		}
	//	}
	//	//
	//	// generate CCW index list of sphere triangles
	//	// k1--k1+1
	//	// |  / |
	//	// | /  |
	//	// k2--k2+1
	//	
	//	//std::vector<int> lineIndices;
	//	unsigned int k1, k2;
	//	for (int i = 0; i < stackCount; ++i) {
	//	    k1 = i * (sectorCount + 1);     // beginning of current stack
	//	    k2 = k1 + sectorCount + 1;      // beginning of next stack
	//		//
	//	    for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
	//	    {
	//	        // 2 triangles per sector excluding first and last stacks
	//	        // k1 => k2 => k1+1
	//	        if (i != 0)
	//	        {
	//	            indices.push_back(k1);
	//	            indices.push_back(k2);
	//	            indices.push_back(k1 + 1);
	//	        }
	//			//
	//	        // k1+1 => k2 => k2+1
	//	        if (i != (stackCount-1))
	//	        {
	//	            indices.push_back(k1 + 1);
	//	            indices.push_back(k2);
	//	            indices.push_back(k2 + 1);
	//	        }
	//			//
	//	        // store indices for lines
	//	        // vertical lines for all stacks, k1 => k2
	//	        //lineIndices.push_back(k1);
	//	        //lineIndices.push_back(k2);
	//	        //if(i != 0)  // horizontal lines except 1st stack, k1 => k+1
	//	        //{
	//	        //    lineIndices.push_back(k1);
	//	        //    lineIndices.push_back(k1 + 1);
	//	        //}
	//	    }
	//	}
	//}
}


namespace MESH::V {

	void CreateVAO (
		/*OUT*/	GLuint& vao,
		/*OUT*/ GLuint* buffers,
		/*IN */	const u64& verticesSize,
		/*IN */	const GLfloat* vertices
	) {
		ZoneScopedN("Mesh: MESH::V: CreateVAO");

		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		auto& vbo = buffers[0];

		glGenVertexArrays (1, &vao);
		glGenBuffers (1, &vbo);

		/* v */ glBindVertexArray (vao);
		/* v */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* v */ glBufferData (GL_ARRAY_BUFFER, verticesSize * VERTEX * UNIT_SIZE, vertices, GL_STATIC_DRAW);
		/* v */ DEBUG_RENDER GL::GetError (2);

		/* v */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 3 * UNIT_SIZE, (void*)0);
		/* v */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/* v */ DEBUG_RENDER GL::GetError (3);

		// Not needed -> Unbind!
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count) {
		ZoneScopedN("Mesh: MESH::V: Draw");

		const u8 OFFSET = 0;
		glDrawArrays (mode, OFFSET, count);
		DEBUG_RENDER GL::GetError (1000 + 0);
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
		ZoneScopedN("Mesh: MESH::VI: CreateVAO");

		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;

		auto& vbo = buffers[0];
		auto& ebo = buffers[1];

		glGenVertexArrays (1, &vao);
		glGenBuffers (2, buffers);

		/* v */ glBindVertexArray (vao);
		/* v */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* v */ glBufferData (GL_ARRAY_BUFFER, verticesSize * VERTEX * UNIT_SIZE, vertices, GL_STATIC_DRAW);
		/* v */ DEBUG_RENDER GL::GetError (7);

		/* i */ glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo); // We do not unbind it!
		/* i */ glBufferData (GL_ELEMENT_ARRAY_BUFFER, indicesSize * UNIT_SIZE, indices, GL_STATIC_DRAW);
		/* i */ DEBUG_RENDER GL::GetError (8);

		
		/* v */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 3 * UNIT_SIZE, (void*)0);
		/* v */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/* v */ DEBUG_RENDER GL::GetError (9);

		// Not needed -> Unbind! 
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count) {
		ZoneScopedN("Mesh: MESH::VI: Draw");

		const void* USING_VBO = nullptr;
		glDrawElements(mode, count, GL_UNSIGNED_INT, USING_VBO);
		DEBUG_RENDER GL::GetError (1000 + 1);
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
		ZoneScopedN("Mesh: MESH::VIT: CreateVAO");

		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		const u64 SAMPLER_ATTRIBUTE_LOCATION_1 = 1;

		auto& vbo = buffers[0];
		auto& ebo = buffers[1];

		glGenVertexArrays (1, &vao);
		glGenBuffers (2, buffers);

		/* v */ glBindVertexArray (vao);
		/* v */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* v */ glBufferData (GL_ARRAY_BUFFER, verticesSize * (VERTEX + UV_SIZE) * UNIT_SIZE, vertices, GL_STATIC_DRAW);
		/* v */ DEBUG_RENDER GL::GetError (10);

		/* i */ glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo); // We do not unbind it!
		/* i */ glBufferData (GL_ELEMENT_ARRAY_BUFFER, indicesSize * UNIT_SIZE, indices, GL_STATIC_DRAW);
		/* i */ DEBUG_RENDER GL::GetError (11);

		/* v */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 5 * UNIT_SIZE, (void*)0);
		/* v */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/* v */ DEBUG_RENDER GL::GetError (12);

		/* t */ glVertexAttribPointer (SAMPLER_ATTRIBUTE_LOCATION_1, /* f2 */ 2, GL_FLOAT, GL_FALSE, 5 * UNIT_SIZE, (void*)(3 * sizeof (float)));
		/* t */ glEnableVertexAttribArray (SAMPLER_ATTRIBUTE_LOCATION_1);
		/* t */ DEBUG_RENDER GL::GetError (13);

		// Not needed -> Unbind! 
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count) {
		ZoneScopedN("Mesh: MESH::VIT: Draw");

		const void* USING_VBO = nullptr;
		glDrawElements(mode, count, GL_UNSIGNED_INT, USING_VBO);
		glBindTexture(GL_TEXTURE_2D, 0);
		DEBUG_RENDER GL::GetError (1000 + 2);
	}

}


namespace MESH {

	void DestroyVAO (
		/*IN */ const GLuint& vao,
		/*IN */ const u64& buffersCount,
		/*IN */ const GLuint*& buffers
	) {
		ZoneScopedN("Mesh: DestroyVAO");

		glDeleteVertexArrays (1, &vao);
		glDeleteBuffers (buffersCount, buffers);
	}

}