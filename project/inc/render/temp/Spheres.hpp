// Modified code from https://www.songho.ca/opengl/gl_sphere.html
//
#pragma once
#include "../mesh.hpp"

namespace MESH::DDD::DSPHERE {

	const r32 PI = acos (-1.0f);

	void CreateVerticesOld (
		std::vector<GLfloat>& vertices,
		std::vector<GLuint>& indices,
		std::vector<GLfloat>& normals,
		std::vector<GLfloat>& texCoords,
		const u16& stackCount, 
		const u16& sectorCount, 
		const r32& radius
	) {
		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
		float s, t;                                     // vertex texCoord

		float sectorStep = 2 * PI / sectorCount;
		float stackStep = PI / stackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i) {
			stackAngle = PI / 2 - i * stackStep;

			xy = radius * cosf (stackAngle);
			z = radius * sinf (stackAngle);

			for (int j = 0; j <= sectorCount; ++j) {
				sectorAngle = j * sectorStep;

				x = xy * cosf (sectorAngle);
				y = xy * sinf (sectorAngle);

				vertices.push_back (x);
				vertices.push_back (y);
				vertices.push_back (z);

				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;

				normals.push_back (nx);
				normals.push_back (ny);
				normals.push_back (nz);

				s = ((float)j) / sectorCount;
				t = ((float)i) / stackCount;

				texCoords.push_back (s);
			}
		}
		
		for (int i = 0; i < stackCount; ++i) {

			u32 k1 = i * (sectorCount + 1);
			u32 k2 = k1 + sectorCount + 1;

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {

				if (i != 0) {
					indices.push_back (k1);
					indices.push_back (k2);
					indices.push_back (k1 + 1);
				}

				if (i != (stackCount-1)) {
					indices.push_back (k1 + 1);
					indices.push_back (k2);
					indices.push_back (k2 + 1);
				}
			}
		}
	}

}