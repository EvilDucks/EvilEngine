#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

// GL Transmission Format
// Read GLTF page in documentation for better understanding.


namespace RESOURCES::GLTF {

	u8 sceneGraphLookUpTableSize = 0;
	u8 sceneGraphLookUpTable[256];													// Max 256 nodes.

}

namespace RESOURCES::GLTF::PARENTHOOD {

	const char* NODE_CHILDREN		{ "children"	};

	void GetNodes (
		/* OUT */ Json& nodes,
		/* IN  */ Json& nodeNode,
		/* OUT */ u8*& duplicateObjects
	) {
		if (nodeNode.contains(NODE_CHILDREN)) {
			auto& nodeChildren = nodeNode[NODE_CHILDREN];

			for (u8 iNode; iNode < nodeChildren.size(); ++iNode) {
				u8 nodeId = nodeChildren[iNode].get<int> ();

				sceneGraphLookUpTable[sceneGraphLookUpTableSize] = nodeId;
				++sceneGraphLookUpTableSize;

				++duplicateObjects[nodeId];											// Increment duplicate counter.

				auto& nodeNode = nodes[nodeId];										// Get that Node now in 'nodes' table.
				GetNodes (nodes, nodeNode, duplicateObjects);						// Follow the tree to further recreate sceneGrapth
			}
			
		}
	}
	

	void GetSceneNodes (
		/* OUT */ Json& nodes,
		/* OUT */ Json& rootNodes,
		/* OUT */ u8*& duplicateObjects
	) {
		u8 nodesCount = rootNodes.size();											// Max 256 nodes.

		DEBUG spdlog::info ("Nodes in a gltf scene: {0}.", nodesCount);

		for (u8 iRootNode = 0; iRootNode < nodesCount; ++iRootNode) {
			auto& rootNode = rootNodes[iRootNode];
			u8 nodeId = rootNode.get<int> ();

			sceneGraphLookUpTable[sceneGraphLookUpTableSize] = nodeId;
			++sceneGraphLookUpTableSize;

			++duplicateObjects[nodeId];												// Increment duplicate counter.

			auto& nodeNode = nodes[nodeId];											// Get that Node now in 'nodes' table.
			GetNodes (nodes, nodeNode, duplicateObjects);							// Follow the tree to further recreate sceneGrapth
		}

	}

}

namespace RESOURCES::GLTF::COMPONENTS {

	const char* NODE_MESH			{ "mesh"		};
	// Matrix can be represented in 2 different ways.
	const char* NODE_MATRIX			{ "matrix"		};
	const char* NODE_TRANSLATION	{ "translation"	};
	const char* NODE_ROTATION		{ "rotation"	};
	const char* NODE_SCALE			{ "scale"		};

	void GetObjectComponents (
		/* OUT */ Json& object,
		/* OUT */ u16& meshesCounter
	) {

		if (object.contains (NODE_MESH)) {
			auto& mesh = object[NODE_MESH];
			++meshesCounter;
		}

		if (object.contains (NODE_MATRIX)) {
			auto& matrix = object[NODE_MATRIX];
		} else {
			if (object.contains (NODE_TRANSLATION)) {
				auto& translation = object[NODE_TRANSLATION];
			}

			if (object.contains (NODE_ROTATION)) {
				auto& rotation = object[NODE_ROTATION];
			}

			if (object.contains (NODE_SCALE)) {
				auto& scale = object[NODE_SCALE];
			}
		}

	}
	
}

namespace RESOURCES::GLTF {

	const char* NODE_SCENE		{ "scene"	};
	const char* NODE_SCENES		{ "scenes"	};
	const char* NODE_NODES		{ "nodes"	};
	const char* NODE_ASSET		{ "asset"	};
	const char* NODE_VERSION	{ "version"	};

	const char* VERSION_2_0		{ "2.0"		};

	void Create (
		/* OUT */ Json& json
	) {
		u16 transformsCount = 1;													// Always add-up Root transfrom even tho it's always 0.
		u16 materialsCount = 0;
		u16 meshesCount = 0;

		u8 scenesCount;																// Max 256 scenes.
		u8 defaultScene = 0;

		DEBUG {																		// VALIDATION information only.
			if (json.contains (NODE_ASSET)) {										
				auto& nodeAsset = json[NODE_ASSET];
				if (nodeAsset.contains (NODE_VERSION)) {
					auto& nodeVersion = nodeAsset[NODE_VERSION];
					const std::string version = nodeVersion.get<std::string> ();

					if (version.compare (VERSION_2_0) != 0) 
						ErrorExit ("Invalid gltf version: {0}.", version);
				} else 	ErrorExit (ERROR_CONTAIN, "gltf", NODE_VERSION );
			} else 		ErrorExit (ERROR_CONTAIN, "gltf", NODE_ASSET );

			if (json.contains (NODE_NODES)) {
				//
			} else ErrorExit (ERROR_CONTAIN, "gltf", NODE_NODES );
		}

		// how did relations worked??

		// Create a reference to 'nodes' array, and a duplicate counter array for objects (to simplyfy and optimize the algorithm).
		auto& nodes = json[NODE_NODES];
		auto duplicateObjects = (u8*) calloc (nodes.size(), sizeof (u8));			// ALLOCATION with 0-initialization.

		if (json.contains (NODE_SCENE)) {											// Get info on which is the default scene.
			auto& nodeScene = json[NODE_SCENE];
			defaultScene = nodeScene.get<int> ();
		} else DEBUG ErrorExit (ERROR_CONTAIN, "gltf", NODE_SCENE);

		if (json.contains (NODE_SCENES)) {											// Access all the scenes in the file.
			auto& nodeScenes = json[NODE_SCENES];

			scenesCount = nodeScenes.size();										// Get information on how many scenes there are in the file.

			for (u8 iScene = 0; iScene < scenesCount; ++iScene) {
				auto& nodeScene = nodeScenes[iScene];
				if (nodeScene.contains (NODE_NODES)) {
					auto& nodeNodes = nodeScene[NODE_NODES];
					PARENTHOOD::GetSceneNodes (nodes, nodeNodes, duplicateObjects);	// RabbitHole !
				} else DEBUG ErrorExit (ERROR_CONTAIN, "gltf", NODE_NODES);
			}
			
		} else DEBUG ErrorExit (ERROR_CONTAIN, "gltf", NODE_SCENES );

		DEBUG for (u8 iNode = 0; iNode < sceneGraphLookUpTableSize; ++iNode) {
			spdlog::info("{0}:\t{1}", iNode, sceneGraphLookUpTable[iNode]);
		}

		// Get Objects
		for (u8 iNode = 0; iNode < nodes.size(); ++iNode) {
			auto& objectDuplicatesCounter = duplicateObjects[iNode];
			auto& object = nodes[iNode];

			transformsCount += objectDuplicatesCounter;								// Sum up all transforms.

			COMPONENTS::GetObjectComponents (
				object, meshesCount
			);
		}

		// Get Meshes
		//

		// Get Materials
		//

		DEBUG {
			spdlog::info ("t: {0}, ma: {1}, me: {2}", transformsCount, materialsCount, meshesCount);
		}
		
		// Free allocated memory.
		delete[] duplicateObjects;

		// Reset for another function call.
		sceneGraphLookUpTableSize = 0;
	}

	void Load (
		/* OUT */ Json& json
	) {

	}

}