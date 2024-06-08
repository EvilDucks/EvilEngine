#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

#include "util/mmrelation.hpp"

// GL Transmission Format
// Read GLTF page in documentation for better understanding.


// When using location.hpp reading a location .json file we arleady knew about possible materials and meshes
//  Here instead we have to read information about them from here too.
//  So we need 


// TODO
// 1. I cannot create a mmrelation table for transform placement lookup without 
//  information about materials and mesh. For simplicity I might need only their sizeses for now.
//  Tho later i would have to offset mesh indexes in mesh table after sorting.
//  - Create a materialMeshRelationsLookUpTable to get uniques to dermine bytes of meshTable.
// 2. Problem. Information about what mesh is using what material is inside mesh node.
//  Therefore when saving a relation i would need to read it along with mesh id.
//	- Create meshTable
// 3. Problem. Primitive not Mesh nodes! If node 0 has a mesh that has 3 primitives
//   then node 1 mesh/es should start from index 3 not 1! Where do i store and access that 
//   information for further processing?

// 0->0,1,2
// 1->3
// 2->4,5


// To read through we read first(next) intieger and then 
//  move by that value n times to get n index set.

// nodeTable
// n -> (size) (id, id, ...)
// 0 -> 1,1
// 1 -> 2,2,3
// 2 -> 3,4,5,6
// 3 -> 4,7,8,9,10

// ninja.gltf example
// 0 -> 3,0,1,2
// 1 -> 1,3
// 2 -> 1,4


namespace RESOURCES::GLTF {

	u8 sceneGraphLookUpTableSize = 0;
	u8 sceneGraphLookUpTable[MMRELATION::MAX_NODES];									// Max 256 nodes.

	// GLTF assumes that a node can be drawn using multiple draw calls
	//  engine does not support that. Therefore if a node says it uses a mesh
	//  that has multiple primitives defined then we need to convert that information into
	//  3 nodes. Where each has it's own primitive (mesh) and 
	//  transform althrough shared during initialization. 
	u8 nodeTableSize = 0;
	u8 nodeTable[MESH::MAX_MESHES];
}


namespace RESOURCES::GLTF::MATERIALS {

	const char* NODE_MATERIALS		{ "materials"	};

	void GetMaterialsCount (
		/* OUT */ Json& json,
		/* OUT */ u8& materialsCount
	) {
		auto& nodeMaterials = json[NODE_MATERIALS];
		materialsCount = nodeMaterials.size ();
	}

}


namespace RESOURCES::GLTF::MESHES {

	const char* NODE_MESHES			{ "meshes"		};
	const char* NODE_PRIMITIVES		{ "primitives"	};
	const char* NODE_MATERIAL		{ "material"	};

	void GetMeshes (
		/* OUT */ Json& json,
		/* OUT */ u8& meshesCount
	) {
		auto& nodeMeshes = json[NODE_MESHES];

		for (u8 iMesh = 0; iMesh < nodeMeshes.size (); ++iMesh) {
			auto& nodePrimitives = nodeMeshes[iMesh][NODE_PRIMITIVES];
			meshesCount += nodePrimitives.size ();

			nodeTable[nodeTableSize] = nodePrimitives.size ();
			++nodeTableSize;

			//for (u8 iPrimitive = 0; iPrimitive < nodePrimitives.size (); ++iPrimitive) {
			//	nodeTable[nodeTableSize] = nodeTableSize - 1;
			//	++nodeTableSize;
			//}
		}
	}
	
}


namespace RESOURCES::GLTF::PARENTHOOD {

	// First we call 'GetSceneNodes' to get root's nodes eg. scenes->nodes
	//  Then with each node found we call 'GetNodes' recursively to get node children children.
	//  After this process we will have a complete array descibing scenegraph.

	const char* NODE_CHILDREN		{ "children"	};

	void GetNodes (
		/* OUT */ Json& nodes,
		/* IN  */ Json& nodeNode,
		/* OUT */ u8*& duplicateObjects,
		/* OUT */ u16& parenthoodsCounter
	) {
		auto& nodeChildren = nodeNode[NODE_CHILDREN];
		++parenthoodsCounter;													// Increment duplicate counter.

		for (u8 iNode; iNode < nodeChildren.size(); ++iNode) {
			u8 nodeId = nodeChildren[iNode].get<int> ();

			sceneGraphLookUpTable[sceneGraphLookUpTableSize] = nodeId;
			++sceneGraphLookUpTableSize;
			++duplicateObjects[nodeId];											// Increment duplicate counter.

			auto& nodeNode = nodes[nodeId];										// Get that Node now in 'nodes' table.
			if (nodeNode.contains(NODE_CHILDREN)) {
				GetNodes (nodes, nodeNode, duplicateObjects, parenthoodsCounter);	// Follow the tree to further recreate sceneGrapth
			}
		}
	}
	

	void GetSceneNodes (
		/* OUT */ Json& nodes,
		/* OUT */ Json& rootNodes,
		/* OUT */ u8*& duplicateObjects,
		/* OUT */ u16& parenthoodsCount
	) {
		u8 nodesCount = rootNodes.size();											// Max 256 nodes.

		for (u8 iRootNode = 0; iRootNode < nodesCount; ++iRootNode) {
			auto& rootNode = rootNodes[iRootNode];
			u8 nodeId = rootNode.get<int> ();

			sceneGraphLookUpTable[sceneGraphLookUpTableSize] = nodeId;
			++sceneGraphLookUpTableSize;
			++duplicateObjects[nodeId];												// Increment duplicate counter.

			auto& nodeNode = nodes[nodeId];											// Get that Node now in 'nodes' table.
			if (nodeNode.contains(NODE_CHILDREN)) {
				GetNodes (nodes, nodeNode, duplicateObjects, parenthoodsCount);			// Follow the tree to further recreate sceneGrapth
			}
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

	//void GetObjectComponents (
	//	/* OUT */ Json& gltf,
	//	/* OUT */ Json& object
	//) {
	//
	//	if (object.contains (NODE_MESH)) {
	//		auto& mesh = object[NODE_MESH];
	//
	//		u8 meshId = mesh.get<int> (); // THIS IS NOT VALID !!!!! Primitive is what our Mesh component is !!!!!
	//		u8 materialId = 0;
	//
	//		// Information about materialId is stored inside mesh node. So we need to dig in.
	//		//  or create a lookuptable ... maybe not.
	//
	//		auto& meshes = gltf[MESHES::NODE_MESHES];
	//		auto& primitives = meshes[meshId][MESHES::NODE_PRIMITIVES];
	//
	//		for (u8 iPrimitive = 0; iPrimitive < primitives.size(); ++iPrimitive) {
	//			auto& primitive = primitives[iPrimitive];
	//
	//			if (primitive.contains (MESHES::NODE_MATERIAL)) {
	//				auto& material = primitive[MESHES::NODE_MATERIAL];
	//				materialId = material.get<int> ();
	//			}
	//
	//			//DEBUG spdlog::info ("ma: {0}, me: {1}", materialId, meshId);
	//
	//		}
	//
	//	}
	//
	//	if (object.contains (NODE_MATRIX)) {
	//		auto& matrix = object[NODE_MATRIX];
	//	} else {
	//		if (object.contains (NODE_TRANSLATION)) {
	//			auto& translation = object[NODE_TRANSLATION];
	//		}
	//
	//		if (object.contains (NODE_ROTATION)) {
	//			auto& rotation = object[NODE_ROTATION];
	//		}
	//
	//		if (object.contains (NODE_SCALE)) {
	//			auto& scale = object[NODE_SCALE];
	//		}
	//	}
	//
	//}

	void GetObject (
		/* OUT */ Json& gltf,
		/* OUT */ Json& object
	) {

	}
	
}


namespace RESOURCES::GLTF {

	const char* NODE_SCENE		{ "scene"	};
	const char* NODE_SCENES		{ "scenes"	};
	const char* NODE_NODES		{ "nodes"	};
	const char* NODE_ASSET		{ "asset"	};
	const char* NODE_VERSION	{ "version"	};

	const char* VERSION_2_0		{ "2.0"		};


	void Validate (
		/* OUT */ Json& json
	) {
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

		if (json.contains (MESHES::NODE_MESHES)) {
			//
		} else ErrorExit (ERROR_CONTAIN, "gltf", MESHES::NODE_MESHES );
	}


	// 1. Construct scenegraph reference table fully.
	// 2. Get materials Count and Meshes Count.
	// 3. Construct Material-Mesh Relations -> 'mmr'
	// 4. Create the mesh table.

	void Create (
		/* OUT */ Json& json,
		/* OUT */ ::SCENE::SceneLoadContext& loadContext,
		// [ Components ]
		/* OUT */ u16& parenthoodsCount,
		/* OUT */ u16& childrenCount,
		/* OUT */ u16& transformsCount,		
		// [ Shared ]									
		/* OUT */ u8& materialsCount,
		/* OUT */ u8& meshesCount,
		// [ Unique - Per each World ]
		/* OUT */ u8*& meshTable,
		/* OUT */ u16& transformsOffset
	) {
		auto& mmrlut = loadContext.relationsLookUpTable;							// Material-Mesh Relation Look Up Table

		// For simplicity we allocate it with size of 'MMRELATION::MAX_NODES'. 
		//  Otherwise we would have to loop few more times. ALLOCATION!
		//
		mmrlut = (u16*) malloc (MMRELATION::MAX_NODES * sizeof (u16));				// With it during load phase we're able to 'sort' in a way our transform components. 
		u16 mmrlutu = 0; 															// Uniques, ...
		u16 mmrlutc = 0; 															// Counter, ...

		// Initialize components size.
		parenthoodsCount = 1;														// Always add-up Root parenthood
		transformsCount = 1;														// Always add-up Root transfrom even tho it's always 0.
		materialsCount = 0;
		meshesCount = 0;

		// HACK [ We always use the default scene. ]
		u8 scenesCount;																// Max 256 scenes.
		u8 defaultScene = 0;

		DEBUG Validate (json);

		{ // SHARED CONTENT READ eg. Materials, MeshTable
			if (json.contains (MATERIALS::NODE_MATERIALS))							// GLTF might not define any materials...
				MATERIALS::GetMaterialsCount (json, materialsCount);				// Get MaterialCount
			MESHES::GetMeshes (json, meshesCount);									// Get Meshes
		}


		// Create a reference to 'nodes' array, and a duplicate counter array for objects (to simplyfy and optimize the algorithm).
		auto& nodes = json[NODE_NODES];
		auto duplicateObjects = (u8*) calloc (nodes.size(), sizeof (u8));			// ALLOCATION with 0-initialization.


		{ // SceneGraph READ 
			if (json.contains (NODE_SCENE)) {										// What scene is default information.
				auto& nodeScene = json[NODE_SCENE];
				defaultScene = nodeScene.get<int> ();
			} else DEBUG ErrorExit (ERROR_CONTAIN, "gltf", NODE_SCENE);

			if (json.contains (NODE_SCENES)) {										// Access all the scenes in the file.
				auto& nodeScenes = json[NODE_SCENES];
				scenesCount = nodeScenes.size();									// Get information on how many scenes there are in the file.

				for (u8 iScene = 0; iScene < scenesCount; ++iScene) {
					auto& nodeScene = nodeScenes[iScene];
					if (nodeScene.contains (NODE_NODES)) {
						auto& nodeNodes = nodeScene[NODE_NODES];
						PARENTHOOD::GetSceneNodes (nodes, nodeNodes, duplicateObjects, parenthoodsCount);	// RabbitHole !
					} else DEBUG ErrorExit (ERROR_CONTAIN, "gltf", NODE_NODES);
				}
			
			} else DEBUG ErrorExit (ERROR_CONTAIN, "gltf", NODE_SCENES );
		}


		// Now that we have information about the actuall amount of nodes and meshes
		//  we can finally create the mesh table just as we do when we load a scene.
		{ // But first we need MMRELATIONS to do so...

			{ // ROOT TRANSFORM
				MMRELATION::AddEmptyRelation (
					mmrlutc, mmrlut,
					RESOURCES::MMRELATION::MATERIAL_INVALID, 
					RESOURCES::MMRELATION::MESH_INVALID
				);

				++transformsOffset;
			}

			// To adjust json-primitives (meshes) with json-meshes counts.
			u8 meshCounter = 0;

			for (u8 iNode = 0; iNode < nodes.size(); ++iNode) {
				auto& objectDuplicatesCounter = duplicateObjects[iNode];
				auto& object = nodes[iNode];

				if (object.contains (COMPONENTS::NODE_MESH)) {
					auto& mesh = object[COMPONENTS::NODE_MESH];

					u8 meshId = mesh.get<int> ();
					u8 materialId;

					auto& primitivesCount = nodeTable[meshId];

					// We create additional NODES when a node has more then one primitive (mesh).
					transformsCount += (primitivesCount * objectDuplicatesCounter);

					auto& meshes = json[MESHES::NODE_MESHES];
					auto& primitives = meshes[meshId][MESHES::NODE_PRIMITIVES];

					for (u8 iPrimitive = 0; iPrimitive < primitivesCount; ++iPrimitive) {
						auto& primitive = primitives[iPrimitive];

						if (primitive.contains (MESHES::NODE_MATERIAL)) {
							auto& material = primitive[MESHES::NODE_MATERIAL];
							materialId = material.get<int> ();

							MMRELATION::CheckAddRelation (
								mmrlutu, mmrlutc, mmrlut,
								materialId, meshCounter + iPrimitive
							);

						} else {
							// If it will be necessery i might create a dummy material for such cases in future.
							DEBUG spdlog::error ("{0} mesh primitive does not define a material", "gltf");
						}
						
					}

					meshCounter += primitivesCount;
				}
			}

			DEBUG MMRELATION::Log (mmrlutu, mmrlutc, mmrlut);
		
			// We initialize it with 1 because theres 1 byte representing materials count.
			// And theres a byte for each material to represent how many different meshes to render it has.
			u8 meshTableBytes = 1 + materialsCount;
			// Now theres two bytes for each mesh using specifc material.
			meshTableBytes += (mmrlutu) * 2;
			meshTable = (u8*) calloc (meshTableBytes, sizeof (u8));					// Allocation !
		}

		DEBUG {
			spdlog::info ("n: {0}, r: {1}", nodes.size(), sceneGraphLookUpTableSize);
			spdlog::info ("t: {0}, p: {1}, ma: {2}, me: {3}", transformsCount, parenthoodsCount, materialsCount, meshesCount);
		}
		
		// Free allocated memory.
		delete[] duplicateObjects;

		// Reset for another function call.
		sceneGraphLookUpTableSize = 0;
		nodeTableSize = 0;
	}


	void Load (
		/* OUT */ Json& json,
		/* IN  */ const ::SCENE::SceneLoadContext& loadContext,
		//
		/* IN  */ const u16& parenthoodsCount,
		/* OUT */ u16*& parenthoodsChildrenTable,
		/* OUT */ ::PARENTHOOD::Parenthood* parenthoods,
		//
		/* IN  */ const u16& transformsCount,	
		/* OUT */ ::TRANSFORM::LTransform* lTransforms,
		/* OUT */ ::TRANSFORM::GTransform* gTransforms,
		//
		/* IN  */ const u8& materialsCount,
		/* OUT */ ::MATERIAL::Material* materials,
		//
		/* IN  */ const u8& meshesCount,
		/* OUT */ ::MESH::Mesh* meshes,
		//
		/* OUT */ u8*& meshTable
	) {

		// 1.
		// meshesCount, meshes, meshTable

		// 1. Check if MMRelation table has to be sorted or not.
		// 2. Read the meshes. We'll try displaying them with a simple setup material.
		// 3. Read the whole object. Making it apper without materials.
		// 4. Load the materials partially or fully
		// 5. Scene sorting and connecting.

		// Set roots transfrom!
		//"position":	[0.0, 0.0, 0.0],
		//"rotation":	[0.0, 0.0, 0.0],
		//"scale":	[1.0, 1.0, 1.0]
	}

}