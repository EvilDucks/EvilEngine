#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

// GL Transmission Format
// Read GLTF page in documentation for better understanding.


namespace RESOURCES::GLTF {

}


namespace RESOURCES::GLTF::SUBSYSTEM {

	const char* NODE_MESH { "mesh" };

	void GetNodes (
		/* OUT */ Json& json
	) {
		u8 nodesCount = json.size();												// Max 256 nodes.


		spdlog::info ("Nodes in a gltf scene: {0}.", nodesCount);
	}

}

namespace RESOURCES::GLTF {

	const char* NODE_SCENE { "scene" };
	const char* NODE_SCENES { "scenes" };
	const char* NODE_NODES { "nodes" };

	void Create (
		/* OUT */ Json& json
	) {
		u8 scenesCount;																// Max 256 scenes.
		u8 defaultScene = 0;

		if (json.contains (NODE_SCENE)) {											// Get info on which is the default scene.
			auto& nodeScene = json[NODE_SCENE];
			defaultScene = nodeScene.get<int> ();
		} else DEBUG { ErrorExit (ERROR_CONTAIN, "gltf", NODE_SCENE); }

		if (json.contains (NODE_SCENES)) {											// Access all the scenes in the file.
			auto& nodeScenes = json[NODE_SCENES];

			scenesCount = nodeScenes.size();										// Get information on how many scenes there are in the file.

			for (u8 iScene = 0; iScene < scenesCount; ++iScene) {
				auto& nodeScene = nodeScenes[iScene];
				if (nodeScene.contains (NODE_NODES)) {
					auto& nodeNodes = nodeScene[NODE_NODES];
					SUBSYSTEM::GetNodes (nodeNodes);								// RabbitHole !
				} else DEBUG { ErrorExit (ERROR_CONTAIN, "gltf", NODE_NODES); }
			}
			
		} else DEBUG { ErrorExit (ERROR_CONTAIN, "gltf", NODE_SCENES ); }
		
	}

	void Load (
		/* OUT */ Json& json
	) {

	}

}