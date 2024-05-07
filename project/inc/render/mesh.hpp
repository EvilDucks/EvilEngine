#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

#include "temp/Sphere.h"


// VBO - Vertex Buffer Objects.
// EBO - Element Buffer Objects.
// VAO - Vertex Array Object.
// IMO - Instanced Model (as global transfrom).

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

	using DrawFunc = void (*)(GLenum mode, GLsizei count, u16 instances);

	struct Base {
		GLuint vao = 0;
		GLsizei verticiesCount = 0;
		GLsizei buffersCount = 0;
		GLuint buffers[6] = { 0 };
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


namespace MESH::INSTANCED {

	void AddTransfrom (
		const GLuint& inm,
		const u16& instancesCount,
		const u8& location
	) {

		/*  m  */ glBindBuffer (GL_ARRAY_BUFFER, inm);
		/*  m  */ glBufferData (GL_ARRAY_BUFFER, instancesCount * sizeof (glm::mat4), nullptr, GL_DYNAMIC_DRAW);
		// glBufferStorage(GL_ARRAY_BUFFER, instancesCount * sizeof (glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
		/*  m  */ DEBUG_RENDER GL::GetError (14);

		// Because max is 4 we need to call it 4 times to store 16 as in glm::mat4 !

		/*  m  */ glVertexAttribPointer (location + 0, 4, GL_FLOAT, GL_FALSE, sizeof (glm::mat4), (void*)(0 * sizeof (glm::vec4)));
		/*  m  */ glEnableVertexAttribArray (location + 0);
		/*  m  */ glVertexAttribDivisor (location + 0, 1);

		/*  m  */ glVertexAttribPointer (location + 1, 4, GL_FLOAT, GL_FALSE, sizeof (glm::mat4), (void*)(1 * sizeof (glm::vec4)));
		/*  m  */ glEnableVertexAttribArray (location + 1);
		/*  m  */ glVertexAttribDivisor (location + 1, 1);

		/*  m  */ glVertexAttribPointer (location + 2, 4, GL_FLOAT, GL_FALSE, sizeof (glm::mat4), (void*)(2 * sizeof (glm::vec4)));
		/*  m  */ glEnableVertexAttribArray (location + 2);
		/*  m  */ glVertexAttribDivisor (location + 2, 1);

		/*  m  */ glVertexAttribPointer (location + 3, 4, GL_FLOAT, GL_FALSE, sizeof (glm::mat4), (void*)(3 * sizeof (glm::vec4)));
		/*  m  */ glEnableVertexAttribArray (location + 3);
		/*  m  */ glVertexAttribDivisor (location + 3, 1);
		/*  m  */ DEBUG_RENDER GL::GetError (15);

	}

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

//
// D - Dynamic - which means it factors are being calculated.
//

namespace MESH::DD::DCIRCLE {

	void CreateVertices (
		u16& vertexesCount, 
		GLfloat*& vertices, 
		const u16& sectorCount,
		const r32& radius
	) {
		vertexesCount = 3 * sectorCount;
		const u16 verticesCount = vertexesCount * 3;
		vertices = (GLfloat*) malloc (verticesCount * sizeof (GLfloat));

		const float PI = 3.1415926f;
		const float angleJump = 2 * PI / sectorCount;
		const float cx = 0;
		const float cy = 0;

		for (u16 i = 0; i < sectorCount; ++i) {
			const u64 offset = 9 * i;
			float a1, x1, y1;

			// MID
			vertices[0 + offset] =  0.0f;
			vertices[1 + offset] =  0.0f;
			vertices[2 + offset] =  0.0f;

			a1 = angleJump * i;
			x1 = cx + radius * cosf(a1);
			y1 = cy + radius * sinf(a1);

			// Left
			vertices[3 + offset] =  x1;
			vertices[4 + offset] =  0.0f;
			vertices[5 + offset] =  y1;

			a1 = a1 + angleJump;
			x1 = cx + radius * cosf(a1);
			y1 = cy + radius * sinf(a1);

			// Right
			vertices[6 + offset] =  x1;
			vertices[7 + offset] =  0.0f;
			vertices[8 + offset] =  y1;
			
		}

	}


	void CreateVertices (
		u16& vertexesCount,
		GLfloat*& vertices,
		u16& indicesCount,
		GLuint*& indices,
		const u16& sectorCount,
		const r32& radius
	) {
		if (sectorCount < 3) exit (1);

		// (mid-point, first-point), (n-point, ...) 
		vertexesCount = 1 + sectorCount; 			 // 3 floats
		const u16 verticesCount = vertexesCount * 3; // 3 Vertexes
		vertices = (GLfloat*) malloc (verticesCount * sizeof (GLfloat));

		indicesCount = sectorCount * 3;			 // 3 Vertexes
		indices = (GLuint*) malloc (indicesCount * sizeof (GLuint));

		const float PI = 3.1415926f;
		const float angleJump = 2 * PI / sectorCount;
		const float cx = 0;
		const float cy = 0;

		// FIRST-POINT calculations.
		float a0 = 0;
		float x0 = cx + radius * cosf(a0);
		float y0 = cy + radius * sinf(a0);

		// MID-POINT
		vertices[0 + VERTEX * 0] = 0.0f;
		vertices[1 + VERTEX * 0] = 0.0f;
		vertices[2 + VERTEX * 0] = 0.0f;
		indices [0]				 = 0;

		// FIRST-POINT
		vertices[0 + VERTEX * 1] = x0;
		vertices[1 + VERTEX * 1] = 0.0f;
		vertices[2 + VERTEX * 1] = y0;
		indices [1]				 = 1;

		u16 i = 1;
		for (; i < sectorCount; ++i) {
			// Current Offset by 2 prev set Vertexes + by current VERTEX
			const u64 co = (VERTEX * 1) + (VERTEX * i);
			const u64 currentVertex = 1 + i;

			a0 = angleJump * i;
			x0 = cx + radius * cosf(a0);
			y0 = cy + radius * sinf(a0);

			// SECOND-POINT
			vertices[0 + co] =  x0;
			vertices[1 + co] =  0.0f;
			vertices[2 + co] =  y0;

			// (0,1,[2), (0,2],3), 0,3,4
			// -> i2, i5, i8
			indices [(VERTEX * i) - 1] = currentVertex;
			indices [(VERTEX * i) + 0] = 0;
			indices [(VERTEX * i) + 1] = currentVertex;
		}

		indices [(VERTEX * i) - 1] = 1;
	}

}


namespace MESH::DDD::DCONE {

	void CreateVertices (
		u16& vertexesCount,
		GLfloat*& vertices,
		u16& indicesCount,
		GLuint*& indices,
		const u16& sectorCount,
		const r32& length, 
		const r32& radius
	) {

		if (sectorCount < 3 || length == 0) 
			exit (1);

		// (mid-point, first-point), (n-point, ...) 
		vertexesCount = 2 + sectorCount; 			 // 3 floats
		const u16 verticesCount = vertexesCount * 3; // 3 Vertexes
		vertices = (GLfloat*) malloc (verticesCount * sizeof (GLfloat));

		indicesCount = sectorCount * 2 * 3;			 // 3 Vertexes '*2' because it's a cone shape.
		indices = (GLuint*) malloc (indicesCount * sizeof (GLuint));
		
		const float PI = 3.1415926f;
		const float angleJump = 2 * PI / sectorCount;
		const float cx = 0;
		const float cy = 0;

		// FIRST-POINT calculations.
		float a0 = 0;
		float x0 = cx + radius * cosf(a0);
		float y0 = cy + radius * sinf(a0);

		// MID-POINT
		vertices[0 + VERTEX * 0] = 0.0f;
		vertices[1 + VERTEX * 0] = 0.0f;
		vertices[2 + VERTEX * 0] = 0.0f;
		indices [0]				 = 0;

		// CONE-TOP-POINT
		vertices[0 + VERTEX * 1] = 0.0f;
		vertices[1 + VERTEX * 1] = length;
		vertices[2 + VERTEX * 1] = 0.0f;
		indices [1]				 = 1;

		// FIRST-POINT
		vertices[0 + VERTEX * 2] = x0;
		vertices[1 + VERTEX * 2] = 0.0f;
		vertices[2 + VERTEX * 2] = y0;
		indices [2]				 = 2;

		u16 i = 1;
		for (; i < sectorCount; ++i) {
			// Current Offset by 2 prev set Vertexes + by current VERTEX
			const u64 co = (VERTEX * 2) + (VERTEX * i);
			const u64 currentVertex = 2 + i;

			a0 = angleJump * i;
			x0 = cx + radius * cosf(a0);
			y0 = cy + radius * sinf(a0);

			// SECOND-POINT
			vertices[0 + co] =  x0;
			vertices[1 + co] =  0.0f;
			vertices[2 + co] =  y0;

			// (0,1,[2), (0,2],3), 0,3,4
			// -> i2, i5, i8
			indices [(VERTEX * i) - 1] = currentVertex;
			indices [(VERTEX * i) + 0] = 0;
			indices [(VERTEX * i) + 1] = currentVertex;
		}

		indices [(VERTEX * i) - 1] = 2;
		//
		indices [(VERTEX * i) + 0] = 1;
		indices [(VERTEX * i) + 1] = 2;

		u16 j = 1;
		for (; j < sectorCount; ++j) {
			const u64 currentVertex = 2 + j;

			indices [(VERTEX * (i + j)) - 1] = currentVertex;
			indices [(VERTEX * (i + j)) + 0] = 1;
			indices [(VERTEX * (i + j)) + 1] = currentVertex;
		}

		indices [(VERTEX * (i + j)) - 1] = 2;
		
	}
}


namespace MESH::DDD::DCYLINDER {

	void CreateVertices (
		u16& vertexesCount,
		GLfloat*& vertices,
		u16& indicesCount,
		GLuint*& indices,
		const u16& sectorCount, 
		const r32& length, 
		const r32& radius
	) {
		if (sectorCount < 3 || length == 0) exit (1);
		// (mid-point, first-point), (n-point, ...) 
		vertexesCount = (1 + sectorCount) * 2; 		 // 3 floats
		const u16 verticesCount = vertexesCount * 3; // 3 Vertexes
		vertices = (GLfloat*) malloc (verticesCount * sizeof (GLfloat));

		indicesCount = sectorCount * 4 * 3;		// 3 Vertexes '*4' because it's a cylinder shape.
		indices = (GLuint*) malloc (indicesCount * sizeof (GLuint));
		
		const float PI = 3.1415926f;
		const float angleJump = 2 * PI / sectorCount;
		const float cx = 0;
		const float cy = 0;

		// FIRST-POINT calculations.
		float a0 = 0;
		float x0 = cx + radius * cosf(a0);
		float y0 = cy + radius * sinf(a0);

		// D-MID-POINT
		vertices[0 + VERTEX * 0] = 0.0f;
		vertices[1 + VERTEX * 0] = 0.0f;
		vertices[2 + VERTEX * 0] = 0.0f;
		indices [0]				 = 0;

		// D-FIRST-POINT
		vertices[0 + VERTEX * 1] = x0;
		vertices[1 + VERTEX * 1] = 0.0f;
		vertices[2 + VERTEX * 1] = y0;
		indices [1]				 = 1;

		// dla 4.

		// [00,01,02] - D MID
		// [03,04,05] - D FISRT
		// 06,07,08	  - D n1
		// 09,10,11   - D n2
		// 12,13,14   - D n3

		// 15,16,17   - T MID
		// 18,19,20   - T FIRST
		// 21,22,23	  - T n1
		// 24,25,26	  - T n2
		// 27,28,29   - T n3

		const u32 uo = (sectorCount + 1); // UP OFFSET

		//spdlog::info ("n: {0}", 0 + (VERTEX * (uo + 0)));
		//spdlog::info ("a: {0}", indicesCount);
		//exit (1);

		// T-MID-POINT
		vertices[0 + (VERTEX * (uo + 0))] = 0.0f;
		vertices[1 + (VERTEX * (uo + 0))] = length;
		vertices[2 + (VERTEX * (uo + 0))] = 0.0f;
		indices [0 + (sectorCount * 3)]	  = uo + 0;

		// T-FIRST-POINT
		vertices[0 + (VERTEX * (uo + 1))] = x0;
		vertices[1 + (VERTEX * (uo + 1))] = length;
		vertices[2 + (VERTEX * (uo + 1))] = y0;
		indices [1 + (sectorCount * 3)]	  = uo + 1;

		u16 i = 1;
		for (; i < sectorCount; ++i) {
			// Current Down Offset by 2 prev set Vertexes + by current VERTEX
			const u64 cdo = VERTEX * (1 + i);
			const u64 currentDownVertex = 1 + i;
			const u64 cto = VERTEX * (uo + 1 + i);
			const u64 currentTopVertex = uo + 1 + i;

			a0 = angleJump * i;
			x0 = cx + radius * cosf(a0);
			y0 = cy + radius * sinf(a0);

			// D-SECOND-POINT
			vertices[0 + cdo] =  x0;
			vertices[1 + cdo] =  0.0f;
			vertices[2 + cdo] =  y0;

			// T-SECOND-POINT
			vertices[0 + cto] =  x0;
			vertices[1 + cto] =  length;
			vertices[2 + cto] =  y0;

			// (0,1,[2), (0,2],3), 0,3,4
			// -> i2, i5, i8
			indices [(VERTEX * i) - 1] = currentDownVertex;
			indices [(VERTEX * i) + 0] = 0;
			indices [(VERTEX * i) + 1] = currentDownVertex;

			indices [(VERTEX * (sectorCount + i)) - 1] = currentTopVertex;
			indices [(VERTEX * (sectorCount + i)) + 0] = uo + 0;
			indices [(VERTEX * (sectorCount + i)) + 1] = currentTopVertex;

			// SIDES
			indices [(VERTEX * ((sectorCount * 2) + (i * 2) + 0)) + 0] = currentDownVertex - 1;	// prev down
			indices [(VERTEX * ((sectorCount * 2) + (i * 2) + 0)) + 1] = currentDownVertex;		// cur down
			indices [(VERTEX * ((sectorCount * 2) + (i * 2) + 0)) + 2] = currentTopVertex - 1;	// prev top

			indices [(VERTEX * ((sectorCount * 2) + (i * 2) + 1)) + 0] = currentTopVertex - 1;	// prev top
			indices [(VERTEX * ((sectorCount * 2) + (i * 2) + 1)) + 1] = currentTopVertex;		// cur top
			indices [(VERTEX * ((sectorCount * 2) + (i * 2) + 1)) + 2] = currentDownVertex;		// cur down
		}

		indices [(VERTEX * i) - 1] = 1;
		indices [(VERTEX * (sectorCount + i)) - 1] = uo + 1;

		// First SIDES
		indices [(VERTEX * ((sectorCount * 2) + 0)) + 0] = sectorCount + 0; // last-down
		indices [(VERTEX * ((sectorCount * 2) + 0)) + 1] = 1;				// first-down
		indices [(VERTEX * ((sectorCount * 2) + 0)) + 2] = uo + sectorCount;// first-up

		// First SIDES
		indices [(VERTEX * ((sectorCount * 2) + 1)) + 0] = uo + sectorCount;// last-top
		indices [(VERTEX * ((sectorCount * 2) + 1)) + 1] = uo + 1;			// first-top
		indices [(VERTEX * ((sectorCount * 2) + 1)) + 2] = 1;				// first-up
	}

}


namespace MESH::DDD::DSPHERE {

	// TODO
	// 1. Gen a triangle						// DONE
	// 2. Gen a circle							// DONE
	// 3. Gen a cone							// DONE
	// 4. Gen a cylinder						// DONE
	// 5. Gen a Sphere  (longitude & latidute)	// DONE
	// 6. Write it better						//
	// 7. Gen a Capsule (longitude & length)	//
	// 8. Gen a Sphere  (IcoSphere)				//
	// 9. Write it better						//
	//10. Gen a Sphere  (CubeSphere)			//
	//11. Write it better						//

	//u8 longitude = 0;
	//u8 latitude = 0;

	//void CreateVertices (
	//	u16& vertexesCount,
	//	GLfloat*& vertices,
	//	u16& indicesCount,
	//	GLuint*& indices,
	//	const u16& sectorCount, 
	//	const u16& stackCount,
	//	const r32& radius
	//) {
	//	if (sectorCount < 3 || stackCount < 3) exit (1);
	//}

}


namespace MESH::DDD::DCAPSULE {

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

	void Draw (GLenum mode, GLsizei count, u16 reserved) {
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

	void Draw (GLenum mode, GLsizei count, u16 reserved) {
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

	void Draw (GLenum mode, GLsizei count, u16 reserved) {
		ZoneScopedN("Mesh: MESH::VIT: Draw");

		const void* USING_VBO = nullptr;
		glDrawElements(mode, count, GL_UNSIGNED_INT, USING_VBO);
		glBindTexture(GL_TEXTURE_2D, 0);
		DEBUG_RENDER GL::GetError (1000 + 2);
	}

}





namespace MESH::INSTANCED::V {

	void CreateVAO (
		/* OUT */	GLuint& vao,
		/* OUT */	GLuint* buffers,
		/* IN  */	const u64& verticesSize,
		/* IN  */	const GLfloat* vertices,
		//
		/* IN  */ const u16& instancesCount
	) {
		ZoneScopedN("Mesh: MESH::V: CreateVAO");

		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		const u64 INSTANCE_MODEL_ATTRIBUTE_LOCATION_1 = 1;

		auto& vbo = buffers[0];
		auto& inm = buffers[1];

		glGenVertexArrays (1, &vao);
		glGenBuffers (2, buffers);

		/* v */ glBindVertexArray (vao);
		/* v */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* v */ glBufferData (GL_ARRAY_BUFFER, verticesSize * VERTEX * UNIT_SIZE, vertices, GL_STATIC_DRAW);
		/* v */ DEBUG_RENDER GL::GetError (2);

		/* v */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 3 * UNIT_SIZE, (void*)0);
		/* v */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/* v */ DEBUG_RENDER GL::GetError (3);

		/* m */ AddTransfrom (inm, instancesCount, INSTANCE_MODEL_ATTRIBUTE_LOCATION_1);

		// Not needed -> Unbind!
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count, u16 instances) {
		ZoneScopedN("Mesh: MESH::INSTANCED::V: Draw");

		const u8 OFFSET = 0;
		glDrawArraysInstanced (mode, OFFSET, count, instances);
		DEBUG_RENDER GL::GetError (1000 + 0);
	}

}

namespace MESH::INSTANCED::VI {

	void CreateVAO (
		/* OUT */	GLuint& vao,
		/* OUT */	GLuint* buffers,
		/* IN  */	const u64& verticesSize,
		/* IN  */	const GLfloat* vertices,
		/* IN  */	const u64& indicesSize,
		/* IN  */	const GLuint* indices,
		//
		/* IN  */ const u16& instancesCount
	) {
		ZoneScopedN("Mesh: MESH::VI: CreateVAO");

		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		const u64 INSTANCE_MODEL_ATTRIBUTE_LOCATION_1 = 1;

		auto& vbo = buffers[0];
		auto& inm = buffers[1];
		auto& ebo = buffers[2];

		glGenVertexArrays (1, &vao);
		glGenBuffers (3, buffers);

		/* v */ glBindVertexArray (vao);
		/* v */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* v */ glBufferData (GL_ARRAY_BUFFER, verticesSize * VERTEX * UNIT_SIZE, vertices, GL_STATIC_DRAW);
		/* v */ DEBUG_RENDER GL::GetError (7);
		
		/* v */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 3 * UNIT_SIZE, (void*)0);
		/* v */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/* v */ DEBUG_RENDER GL::GetError (9);

		/* m */ AddTransfrom (inm, instancesCount, INSTANCE_MODEL_ATTRIBUTE_LOCATION_1);

		/* i */ glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo); // We do not unbind it!
		/* i */ glBufferData (GL_ELEMENT_ARRAY_BUFFER, indicesSize * UNIT_SIZE, indices, GL_STATIC_DRAW);
		/* i */ DEBUG_RENDER GL::GetError (8);

		// Not needed -> Unbind! 
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count, u16 instances) {
		ZoneScopedN("Mesh: MESH::VI: Draw");

		const void* USING_VBO = nullptr;
		glDrawElementsInstanced (mode, count, GL_UNSIGNED_INT, USING_VBO, instances);
		DEBUG_RENDER GL::GetError (1000 + 1);
	}

}

namespace MESH::INSTANCED::VIT {

	void CreateVAO (
		/* OUT */ GLuint& vao,
		/* OUT */ GLuint* buffers,
		/* IN  */ const u64& verticesSize,
		/* IN  */ const GLfloat* vertices,
		/* IN  */ const u64& indicesSize,
		/* IN  */ const GLuint* indices,
		//
		/* IN  */ const u16& instancesCount
	) {
		ZoneScopedN("Mesh: MESH::VIT: CreateVAO");

		const u8 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		const u8 SAMPLER_ATTRIBUTE_LOCATION_1 = 1;
		const u8 INSTANCE_MODEL_ATTRIBUTE_LOCATION_2 = 2;
		

		auto& vbo = buffers[0];
		auto& inm = buffers[1];
		auto& ebo = buffers[2];

		glGenVertexArrays (1, &vao);
		glGenBuffers (3, buffers);
		glBindVertexArray (vao);

		/* v+t */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
		/* v+t */ glBufferData (GL_ARRAY_BUFFER, verticesSize * (VERTEX + UV_SIZE) * UNIT_SIZE + 5, vertices, GL_STATIC_DRAW);
		/* v+t */ DEBUG_RENDER GL::GetError (10);

		/*  v  */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 5 * UNIT_SIZE, (void*)0);
		/*  v  */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
		/*  v  */ DEBUG_RENDER GL::GetError (11);

		/*  t  */ glVertexAttribPointer (SAMPLER_ATTRIBUTE_LOCATION_1, /* f2 */ 2, GL_FLOAT, GL_FALSE, 5 * UNIT_SIZE, (void*)(3 * sizeof (float)));
		/*  t  */ glEnableVertexAttribArray (SAMPLER_ATTRIBUTE_LOCATION_1);
		/*  t  */ DEBUG_RENDER GL::GetError (12);

		AddTransfrom (inm, instancesCount, INSTANCE_MODEL_ATTRIBUTE_LOCATION_2);

		/*  i  */ glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo); // We do not unbind it!
		/*  i  */ glBufferData (GL_ELEMENT_ARRAY_BUFFER, indicesSize * UNIT_SIZE, indices, GL_STATIC_DRAW);
		/*  i  */ DEBUG_RENDER GL::GetError (13);

		// Not needed -> Unbind! 
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

	}

	void Draw (GLenum mode, GLsizei count, u16 instances) {
		ZoneScopedN("Mesh: MESH::INSTANCED::VIT: Draw");

		const void* USING_VBO = nullptr;
		glDrawElementsInstanced (mode, count, GL_UNSIGNED_INT, USING_VBO, instances);
		glBindTexture (GL_TEXTURE_2D, 0);
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
