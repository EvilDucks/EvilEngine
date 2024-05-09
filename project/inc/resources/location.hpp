#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

namespace RESOURCES::SCENE {

		const char* NAME = "name";
		const char* CHILDREN = "children";
		const char* MATERIAL = "material";
		const char* MESH = "mesh";
		const char* TRANSFORM = "transform";
		const char* POSITION = "position";
		const char* ROTATION = "rotation";
		const char* SCALE = "scale";

		const char* D_MATERIAL = "d_material";
		const char* D_MESH = "d_mesh";

		void NodeCreate (
			/* IN  */ Json& parent,
			//
			/* OUT */ u16& parenthoodsCount,
			/* OUT */ u16& transformsCount
		) {
			
			if ( parent.contains (NAME) ) {
				auto& nodeName = parent[NAME];
			}

			if ( parent.contains (MATERIAL) ) {
				auto& nodeMaterial = parent[MATERIAL];
			}

			if ( parent.contains (MESH) ) {
				auto& nodeMesh = parent[MESH];
			}

			if ( parent.contains (D_MATERIAL) ) {
				auto& nodeDebugMaterial = parent[D_MATERIAL];
			}

			if ( parent.contains (D_MESH) ) {
				auto& nodeDebugMesh = parent[D_MESH];
			}

			if ( parent.contains (TRANSFORM) ) {
				auto& nodeTransform = parent[TRANSFORM];
				++transformsCount;
			}
            
			if ( parent.contains (CHILDREN) ) {
				auto& nodeChildren = parent[CHILDREN];
				auto childrenCount = nodeChildren.size ();

				// Ensure we add a parenthood only when there are defined elements in children node.
				parenthoodsCount += (childrenCount > 0);

				for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
					auto& nodeChild = nodeChildren[iChild];
					NodeCreate (nodeChild, parenthoodsCount, transformsCount);
				}
			}

		}

		void Create (
			Json& json,
			//
			/* IN  */ const u8& materials,
			/* IN  */ const u8& meshes,
			//
			/* OUT */ u8*& meshTable,
			//
			/* OUT */ u16& parenthoodsCount,
			/* OUT */ u16& transformsCount
		) {
			
			ZoneScopedN("RESOURCES::SCENE: Create");
			DEBUG { spdlog::info ("JSON Scene Initialization"); }

			std::ifstream file;
			file.open ( "res/data/scene.json" );
			file >> json; // Parse the file.	

			auto& nodeRoot = json;
			NodeCreate (nodeRoot, parenthoodsCount, transformsCount);

		}

		void Load() {

		}

}