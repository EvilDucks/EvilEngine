#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

#include "util/mmrelation.hpp"

#include <ios>
#include <iostream>
#include <sstream>

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


namespace RESOURCES::GLTF::FILE {

	const char* TYPE_SCALAR = "SCALAR";
	const char* TYPE_VEC3 = "VEC3";
	const char* TYPE_VEC2 = "VEC2";

	const u16 UNSIGNED_SHORT = 5123;
	const u16 FLOAT = 5126;

	char fullString[256] = D_GLTFS;	

	// conversion u16->u32
	const u64 indicesExCount = 8164;
	u32* indicesEx = nullptr;

	void Open (
		/* IN  */ const char* filepath,
		/* OUT */ std::ifstream& fileHandler
	) {		
		fileHandler.open (filepath, std::ios_base::binary);		// Load file data into the structure.

		DEBUG if (fileHandler.fail()) {		// ONLY debug mode - validate file path!
			ErrorExit ("GLTF - Invalid filepath: {0}!", filepath);
		}
	}

	void ReadBytes (
		/* OUT */ any buffor,
		/* IN  */ std::ifstream& fileHandler,
		/* IN  */ const u32& byteOffset, 
		/* IN  */ const u32& byteLength
	) {
		auto&& bufforByte = (s8*)buffor;
		fileHandler.seekg (byteOffset); 
		fileHandler.read (bufforByte, byteLength);
	}

	void Close (
		/* IN  */ std::ifstream& fileHandler
	) {
		fileHandler.close();
	}


	void LoadMesh (
		/* IN  */ const u8& meshId,
		/* OUT */ MESH::Mesh*& meshes,
		/* IN  */ const u16& instancesCount,
		/* IN  */ const r32& verticesCount,
		/* IN  */ r32* const& vertices,
		/* IN  */ const u16& indicesCount,
		/* IN  */ u16* const& indices,
		/* IN  */ const r32& normalsCount,
		/* IN  */ r32* const& normals,
		/* IN  */ const r32& uvsCount,
		/* IN  */ r32* const& uvs
	) {

		DEBUG {

			spdlog::info (
				"LOADMESH: vc: {0}, ic: {1}, nc: {2}, uvc: {3}", 
				verticesCount, indicesCount, normalsCount, uvsCount
			);

			if (vertices == nullptr)	ErrorExit ("Vertices is nullptr");
			if (indices == nullptr)		ErrorExit ("Indices is nullptr");
			if (normals == nullptr)		ErrorExit ("Normals is nullptr");
			if (uvs == nullptr)			ErrorExit ("UVS is nullptr");

		}
		
		DEBUG if (indicesCount > indicesExCount) 
			ErrorExit ("GLTF: LoadMesh: indicesEx size: {1} is smaller then indicies size: {0} !",
				indicesCount, indicesExCount
			);

		// conversion u16->u32
		for (u16 i = 0; i < indicesCount; ++i) {
			indicesEx[i] = indices[i];
		}

		//auto& averticesCount = MESH::DDD::CUBE::IVERTICES_COUNT;
		//auto& avertices = MESH::DDD::CUBE::IVERTICES;
		//auto& aindicesCount = MESH::DDD::CUBE::INDICES_COUNT;
		//auto& aindices = MESH::DDD::CUBE::INDICES;

		auto& componentMesh = meshes[meshId];
		auto& mesh = componentMesh.base;

		MESH::INSTANCED::XVITN::CreateVAO(
			mesh.vao, mesh.buffers,
			verticesCount, vertices,
			indicesCount, indicesEx,
            uvsCount, uvs,
            normalsCount, normals,
			instancesCount
		);

		mesh.verticiesCount = indicesCount;
		mesh.drawFunc = MESH::INSTANCED::XVITN::Draw;

		MESH::CalculateBounds (componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::SQUARE::VERTICES);

		//{ // CUBE MESH. (vertex + indicies + uvs + normals)
		//	auto& verticesCount = MESH::DDD::SQUARE::VERTICES_COUNT;
		//	auto& vertices = MESH::DDD::SQUARE::VERTICES;
		//	auto& indicesCount = MESH::DDD::SQUARE::INDICES_COUNT;
		//	auto& indices = MESH::DDD::SQUARE::INDICES;
		//	auto& uvsCount = MESH::DDD::SQUARE::VERTICES_COUNT;
		//	auto& uvs = MESH::DDD::SQUARE::UVS;
		//	auto& normalsCount = MESH::DDD::SQUARE::NORMALS_COUNT;
		//	auto& normals = MESH::DDD::SQUARE::NORMALS;
		//		
		//	auto meshId = 1;
		//	auto& componentMesh = wMeshes[meshId];
		//	auto& mesh = componentMesh.base;
		//
		//	MESH::INSTANCED::XVITN::CreateVAO (
		//		mesh.vao, mesh.buffers,
		//		verticesCount, vertices,
		//		indicesCount, indices,
		//		uvsCount, uvs,
		//		normalsCount, normals,
		//		wInstancesCounts[meshId]
		//	);
		//
		//	mesh.verticiesCount = indicesCount;
		//	mesh.drawFunc = MESH::INSTANCED::XVITN::Draw;
		//
		//	MESH::CalculateBounds (componentMesh, MESH::DDD::SQUARE::VERTICES_COUNT, MESH::DDD::SQUARE::VERTICES);
		//}
	}

}


namespace RESOURCES::GLTF {

	struct LoadHelper {						// Helper struct for scene loading procedure.
		u16* mmrlut;						// Helper array for sorting TRANSFROM's.
		u8*  duplicateObjects;				// Helper array for nodes that appear multiple times inside a graph.
		u8*  nodeMeshTable;					// Helper array for hidden nodes eg. Primitive -> Mesh convertion.
	};

	u8 sceneGraphLookUpTableSize = 0;
	u8 sceneGraphLookUpTable[MMRELATION::MAX_NODES];									// Max 256 nodes.

	// GLTF assumes that a node can be drawn using multiple draw calls
	//  engine does not support that. Therefore if a node says it uses a mesh
	//  that has multiple primitives defined then we need to convert that information into
	//  3 nodes. Where each has it's own primitive (mesh) and 
	//  transform althrough shared during initialization. 
	
}


namespace RESOURCES::GLTF::MATERIALS {

	const char* NODE_MATERIALS		{ "materials"	};

	void GetMaterialsCount (
		/* OUT */ Json json,
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
		/* OUT */ Json json,
		/* OUT */ u8& meshesCount,
		/* OUT */ u8*& nodeMeshTable
	) {
		auto& nodeMeshes = json[NODE_MESHES];
		u8 nodesCount = nodeMeshes.size ();

		for (u8 iMesh = 0; iMesh < nodesCount; ++iMesh) {
			auto& nodePrimitives = nodeMeshes[iMesh][NODE_PRIMITIVES];
			u8 primitivesCount = nodePrimitives.size ();

			meshesCount += primitivesCount;
			nodeMeshTable[iMesh] = primitivesCount;
		}
	}
	
}


namespace RESOURCES::GLTF::PARENTHOOD {

	// First we call 'GetSceneNodes' to get root's nodes eg. scenes->nodes
	//  Then with each node found we call 'GetNodes' recursively to get node children children.
	//  After this process we will have a complete array descibing scenegraph.

	const char* NODE_CHILDREN		{ "children"	};

	void GetNodes (
		/* OUT */ Json nodes,
		///* IN  */ Json& nodeNode, // change it to hold id
		u8 cNodeId,
		/* OUT */ u8*& duplicateObjects,
		/* OUT */ u16& parenthoodsCounter,
		/* OUT */ u16& childrenCount,
		/* IN  */ u8*& nodeMeshTable
	) {
		Json nodeNode = nodes[cNodeId];											// Get node from nodes
		Json nodeChildren = nodeNode[NODE_CHILDREN];							// Get node field "children"
		u8 nodesCount = nodeChildren.size();									// How many of them.

		++parenthoodsCounter;													
		childrenCount += nodesCount;

		//spdlog::info ("");
		//spdlog::info ("GetNodes: NodesCount: {0}", nodesCount);

		for (u8 iNode = 0; iNode < nodesCount; ++iNode) {
			u8 nodeId = nodeChildren[iNode].get<int> ();

			//spdlog::info ("GetNodes: Node: {0}", nodeId);

			sceneGraphLookUpTable[sceneGraphLookUpTableSize] = nodeId;
			++sceneGraphLookUpTableSize;
			++duplicateObjects[nodeId];											// Increment duplicate counter.

			Json nextNode = nodes[nodeId];										// Get that Node now in 'nodes' table.
			u8 isChildren = nextNode.contains (NODE_CHILDREN);
			u8 isMesh = nextNode.contains ("mesh");

			if (isMesh) {														// Nodes Extension
				Json mesh = nextNode["mesh"];
				u8 meshId = mesh.get<int> ();
				auto& primitivesCount = nodeMeshTable[meshId];
				childrenCount += (primitivesCount - 1);
			}

			if (isChildren) {
				//spdlog::info ("Children!");
				GetNodes (nodes, nodeId, duplicateObjects, parenthoodsCounter, childrenCount, nodeMeshTable);	// Follow the tree to further recreate sceneGrapth
			}
		}
	}
	

	void GetSceneNodes (
		/* OUT */ Json nodes,
		/* OUT */ Json rootNodes,
		/* OUT */ u8*& duplicateObjects,
		/* OUT */ u16& parenthoodsCount,
		/* OUT */ u16& childrenCount,
		/* IN  */ u8*& nodeMeshTable
	) {
		// Top level parenthood children.
		u8 rootNodesCount = rootNodes.size();
		childrenCount += rootNodesCount;

		for (u8 iRootNode = 0; iRootNode < rootNodesCount; ++iRootNode) {
			Json rootNode = rootNodes[iRootNode];
			u8 nodeId = rootNode.get<int> ();

			sceneGraphLookUpTable[sceneGraphLookUpTableSize] = nodeId;
			++sceneGraphLookUpTableSize;
			++duplicateObjects[nodeId];												// Increment duplicate counter.

			Json nextNode = nodes[nodeId];											// Get that Node now in 'nodes' table.
			u8 isChildren = nextNode.contains (NODE_CHILDREN);
			u8 isMesh = nextNode.contains ("mesh");

			if (isMesh) {															// Nodes Extension
				Json mesh = nextNode["mesh"];
				u8 meshId = mesh.get<int> ();
				auto& primitivesCount = nodeMeshTable[meshId];
				childrenCount += (primitivesCount - 1);
			}

			if (isChildren) {
				GetNodes (nodes, nodeId, duplicateObjects, parenthoodsCount, childrenCount, nodeMeshTable);			// Follow the tree to further recreate sceneGrapth
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


	void IsTransform (
		/* OUT */ Json object,
		/* OUT */ u8& isTransform
	) {
		isTransform	+= object.contains (NODE_MATRIX);
		isTransform	+= object.contains (NODE_TRANSLATION);
		isTransform	+= object.contains (NODE_ROTATION);
		isTransform	+= object.contains (NODE_SCALE);
	}

	void ReadTransform (
		/* OUT */ Json object,
		/* IN  */ const u8& isMatrix,
		/* IN  */ const u8& isTranslation,
		/* IN  */ const u8& isRotation,
		/* IN  */ const u8& isScale,
		/* OUT */ TRANSFORM::LTransform& transformComponent
	) {
		auto& transform = transformComponent.base;
		transform.scale = { 1, 1, 1 };

		glm::mat4 model = glm::mat4(1.0);

		if (isMatrix) {
			auto& matrix = object[COMPONENTS::NODE_MATRIX];

			ErrorExit ("GLTF Matrix not yet supported!");
						
		} else {
			
			if (isTranslation) {
				auto& translation = object[COMPONENTS::NODE_TRANSLATION];

				for (u8 iAxis = 0; iAxis < 3; ++iAxis) { // READ
					transform.position[iAxis] = translation[iAxis].get<float>();
				}
			}

			if (isRotation) {
				auto& rotation = object[COMPONENTS::NODE_ROTATION];
				glm::quat quaternion, temp;

				for (u8 iAxis = 0; iAxis < 4; ++iAxis) { // READ
					quaternion[iAxis] = rotation[iAxis].get<double>();
				}


				{ // Conversion

					// Holy cow ( It was actually easy? )
					// https://math.stackexchange.com/questions/4479544/how-to-convert-a-quaternion-from-one-coordinate-system-to-another

					// Dunno rly whats the right '-' and right swap but it seems that this works.
					//  or at least works for blender models.

					temp.w =  quaternion.w;
					temp.x =  quaternion.x;
					temp.y = -quaternion.z;
					temp.z =  quaternion.y;

					TRANSFORM::TRANSFORMATION::QuaternionToAxis (temp, transform.rotation);
				}
				
			}

			if (isScale) {
				auto& scale = object[COMPONENTS::NODE_SCALE];

				for (u8 iAxis = 0; iAxis < 3; ++iAxis) { // READ
					transform.scale[iAxis] = scale[iAxis].get<float>();
				}
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


	void Validate (
		/* OUT */ Json json
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
		/* OUT */ Json json,
		/* OUT */ LoadHelper& loadContext,
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
		// HELPERS
		///* OUT */ u8*& duplicateObjects,
		///* OUT */ u8* nodeMeshTable
	) {
		auto& mmrlut = loadContext.mmrlut;											// Material-Mesh Relation Look Up Table
		auto& duplicateObjects = loadContext.duplicateObjects;						//
		auto& nodeMeshTable = loadContext.nodeMeshTable;							//

		// For simplicity we allocate it with size of 'MMRELATION::MAX_NODES'. 
		//  Otherwise we would have to loop few more times.
		u16 mmrlutu = 0; 															// Uniques, ...
		u16 mmrlutc = 0; 															// Counter, ...

		// Initialize components size.
		parenthoodsCount = 1;														// Always add-up Root parenthood
		transformsCount = 1;														// Always add-up Root transfrom even tho it's always 0.
		materialsCount = 0;
		meshesCount = 0;
		childrenCount = 0;

		// HACK [ We always use the default scene. ]
		u8 scenesCount;																// Max 256 scenes.
		u8 defaultScene = 0;

		sceneGraphLookUpTableSize = 0;

		DEBUG Validate (json);

		{ // SHARED CONTENT READ eg. Materials, MeshTable
			if (json.contains (MATERIALS::NODE_MATERIALS))							// GLTF might not define any materials...
				MATERIALS::GetMaterialsCount (json, materialsCount);				// Get MaterialCount
			MESHES::GetMeshes (json, meshesCount, nodeMeshTable);					// Get Meshes
		}

		// Create a reference to 'nodes' array, and a duplicate counter array for objects (to simplyfy and optimize the algorithm).
		Json nodes = json[NODE_NODES];
		u8 nodesCount = nodes.size();
		duplicateObjects = (u8*) calloc (nodesCount, sizeof (u8));					// ALLOCATION with 0-initialization.


		{ // SceneGraph READ 
			if (json.contains (NODE_SCENE)) {										// What scene is default information.
				Json nodeScene = json[NODE_SCENE];
				defaultScene = nodeScene.get<int> ();
			} else DEBUG ErrorExit (ERROR_CONTAIN, "gltf", NODE_SCENE);

			if (json.contains (NODE_SCENES)) {										// Access all the scenes in the file.
				Json nodeScenes = json[NODE_SCENES];
				scenesCount = nodeScenes.size();									// Get information on how many scenes there are in the file.

				for (u8 iScene = 0; iScene < scenesCount; ++iScene) {
					Json nodeScene = nodeScenes[iScene];
					if (nodeScene.contains (NODE_NODES)) {
						Json nodeNodes = nodeScene[NODE_NODES];
						PARENTHOOD::GetSceneNodes (nodes, nodeNodes, duplicateObjects, parenthoodsCount, childrenCount, nodeMeshTable);	// RabbitHole !
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

			// To get all empty (transform only) relations before. So it's nice and sorted :v
			for (u8 iNode = 0; iNode < nodesCount; ++iNode) {
				Json object = nodes[iNode];
				u8 isNodeTransform = 0;

				u8 isNodeMesh = object.contains (COMPONENTS::NODE_MESH);
				COMPONENTS::IsTransform (object, isNodeTransform);

				// A Node might not have a mesh and only function as a parent transform
				//  for it's children nodes. Because of that we have to use MMRELATIONS 
				//  for assiging GameObjectID and Transform Index.
				if (isNodeMesh == false && isNodeTransform == true) {

					MMRELATION::AddEmptyRelation (
						mmrlutc, mmrlut,
						RESOURCES::MMRELATION::MATERIAL_INVALID, 
						RESOURCES::MMRELATION::MESH_INVALID
					);

					++transformsCount;
					++transformsOffset;
				}
			}

			// To adjust json-primitives (meshes) with json-meshes counts.
			u8 meshCounter = 0;

			for (u8 iNode = 0; iNode < nodesCount; ++iNode) {
				auto& objectDuplicatesCounter = duplicateObjects[iNode];
				Json object = nodes[iNode];

				if (object.contains (COMPONENTS::NODE_MESH)) {
					Json mesh = object[COMPONENTS::NODE_MESH];

					u8 meshId = mesh.get<int> ();
					u8 materialId;

					auto& primitivesCount = nodeMeshTable[meshId];

					DEBUG_GLTF spdlog::info ("pc: {0}", primitivesCount);

					// Naturally we hope gltf is valid and it has a transform component.
					// We create additional NODES when a node has more then one primitive (mesh).
					transformsCount += (primitivesCount * objectDuplicatesCounter);

					Json meshes = json[MESHES::NODE_MESHES];
					Json primitives = meshes[meshId][MESHES::NODE_PRIMITIVES];

					for (u8 iPrimitive = 0; iPrimitive < primitivesCount; ++iPrimitive) {
						Json primitive = primitives[iPrimitive];

						if (primitive.contains (MESHES::NODE_MATERIAL)) {
							Json material = primitive[MESHES::NODE_MATERIAL];
							materialId = material.get<int> ();

							MMRELATION::CheckAddRelation (
								mmrlutu, mmrlutc, mmrlut,
								materialId, meshCounter + iPrimitive
							);

						} else {
							// If it will be necessery i might create a dummy material for such cases in future.
							DEBUG_GLTF spdlog::error ("{0} mesh primitive does not define a material", "gltf");
						}
						
					}

					meshCounter += primitivesCount;
				}
			}

			DEBUG_GLTF MMRELATION::Log (mmrlutu, mmrlutc, mmrlut);
		
			// We initialize it with 1 because theres 1 byte representing materials count.
			// And theres a byte for each material to represent how many different meshes to render it has.
			u8 meshTableBytes = 1 + materialsCount;
			// Now theres two bytes for each mesh using specifc material.
			meshTableBytes += (mmrlutu) * 2;
			meshTable = (u8*) calloc (meshTableBytes, sizeof (u8));					// Allocation !
		}

		DEBUG_GLTF {
			spdlog::info ("n: {0}, r: {1}", nodesCount, sceneGraphLookUpTableSize);
			spdlog::info ("t: {0}, p: {1}, ma: {2}, me: {3}, c: {4}", 
				transformsCount, parenthoodsCount, materialsCount, meshesCount, childrenCount
			);
		}

		// Reset for another function call.
		sceneGraphLookUpTableSize = 0;
		//nodeTableSize = 0;

		//DEBUG spdlog::info ();
		MMRELATION::SortRelations (transformsCount, mmrlut); // jednak musi...
		DEBUG_GLTF MMRELATION::Log (mmrlutu, mmrlutc, mmrlut);
	}


	void Allocate (
		/* IN  */ const u16& childrenCount,
		/* OUT */ u16*& parenthoodsChildrenTable,
		/* IN  */ const u16& parenthoodsCount,
		/* OUT */ ::PARENTHOOD::Parenthood*& parenthoods,
		//
		/* IN  */ const u16& transformsCount,
		/* OUT */ ::TRANSFORM::LTransform*& lTransforms,
		/* OUT */ ::TRANSFORM::GTransform*& gTransforms,
		//
		/* IN  */ const u8& materialsCount,
		/* OUT */ ::MATERIAL::Material*& materials,
		//
		/* IN  */ const u8& meshesCount,
		/* OUT */ ::MESH::Mesh*& meshes
	) {
		parenthoodsChildrenTable = (u16*) malloc (childrenCount * sizeof (u16));
		if (parenthoodsCount)	parenthoods	= new ::PARENTHOOD::Parenthood	[parenthoodsCount];
		if (transformsCount)	lTransforms	= new ::TRANSFORM::LTransform	[transformsCount] { 0 };
		if (transformsCount)	gTransforms	= new ::TRANSFORM::GTransform	[transformsCount];
		if (materialsCount)		materials	= new ::MATERIAL::Material		[materialsCount];
		if (meshesCount)		meshes		= new ::MESH::Mesh				[meshesCount];
	}


	// Every parent node (including root) have to see if its children have extra nodes.
	//  This is neccesery to form a proper parenthood component.
	void GetExtendedChildrenCount (
		/* IN  */ Json& parentNode,
		/* IN  */ Json& nodes,
		/* IN  */ const u8& childrenCount,
		/* IN  */ u8* nodeMeshTable,
		/* OUT */ u8& extendedChildrenCount
	) {
		for (u8 iNode = 0; iNode < childrenCount; ++iNode) {
			u16 nodeId = parentNode[iNode].get<int> ();

			//spdlog::info ("nid: {0}", nodeId);

			auto& nodeNode = nodes[nodeId];
			u8 isMesh = nodeNode.contains(COMPONENTS::NODE_MESH);

			if (isMesh) {
				u16 meshId = nodeNode[COMPONENTS::NODE_MESH].get<int> ();
				//spdlog::info ("mid: {0}", meshId);
				extendedChildrenCount += nodeMeshTable[meshId] - 1;
			} 
		}
	}


	// Now i need to make this method run for next parenthood
	void LoadNode (
		/* IN  */ Json& nodes,								// REF
		/* IN  */ const u8& nodeId,							// REF
		/* IN  */ Json& meshes,								// REF
		// Transform & Sorting
		/* IN  */ ::TRANSFORM::LTransform*& transforms,		// REF
		/* IN  */ const u16& transformsOffset,				// REF
		/* IN  */ u16* const& mmrlut,						// REF
		// Parenthood & Cascading
		/* IN  */ ::PARENTHOOD::Parenthood* parent,			// CPY
		/* IN  */ ::PARENTHOOD::Parenthood*& parenthoods,	// REF
		/* IN  */ u16*& parenthoodsChildrenTable,			// REF
		/* IN  */ u8& childrenCount,						// REF
		/* IN  */ u8& childrenCounter,						// REF
		// Primitive -> Mesh Conversion
		/* IN  */ u8* const& nodeMeshTable,					// REF
		/* OUT */ u8*& meshTable							// REF
	) {
		TRANSFORM::LTransform transformComponent {};
		u16 validKeyPos = 0;

		auto& object = nodes[nodeId];

		u8 isMatrix			= object.contains (COMPONENTS::NODE_MATRIX);
		u8 isTranslation	= object.contains (COMPONENTS::NODE_TRANSLATION);
		u8 isRotation		= object.contains (COMPONENTS::NODE_ROTATION);
		u8 isScale			= object.contains (COMPONENTS::NODE_SCALE);
		u8 isMesh			= object.contains (COMPONENTS::NODE_MESH);
		u8 isChildren		= object.contains (PARENTHOOD::NODE_CHILDREN);

		COMPONENTS::ReadTransform (
			object, isMatrix, isTranslation, isRotation, isScale,
			transformComponent
		);

		if (isMesh) {
			u8 meshId = object[COMPONENTS::NODE_MESH].get<int> ();
			u8 meshCounter = 0;

			auto& primitives = meshes[meshId][MESHES::NODE_PRIMITIVES];
			auto& primitivesCount = nodeMeshTable[meshId];

			// Due to the creation of extra nodes. Now every next mesh has 
			//  to be offseted by the extra nodes size.
			for (u8 i = 0; i < meshId; ++i) meshCounter += nodeMeshTable[i];

			for (u8 iPrimitive = 0; iPrimitive < primitivesCount; ++iPrimitive) {
				auto& primitive = primitives[iPrimitive];

				const u8 materialId = primitive[MESHES::NODE_MATERIAL].get<int> ();
				const u8 extendedMeshId = meshCounter + iPrimitive;
				const u16 relation = (materialId << 8) + extendedMeshId;

				MMRELATION::Find (validKeyPos, transforms, mmrlut, relation);

				// TRANSFORM SETUP
				transformComponent.id = validKeyPos; // !!!!!!!!! ERROR ???? TRANSFORMS_COUNTER
				transforms[validKeyPos] = transformComponent; 

				// PARENTHOOD->CHILD SETUP
				parent->base.children[childrenCounter] = validKeyPos;
				++childrenCounter;

				MMRELATION::SetMeshTableValue (meshTable, mmrlut, transformsOffset, materialId, extendedMeshId);
			}

		} else {
			const u16 relation = MMRELATION::NOT_REPRESENTIVE;
			MMRELATION::Find (validKeyPos, transforms, mmrlut, relation);

			// TRANSFORM SETUP
			transformComponent.id = validKeyPos;
			transforms[validKeyPos] = transformComponent;

			// PARENTHOOD->CHILD SETUP
			parent->base.children[childrenCounter] = validKeyPos;
			++childrenCounter;
		}

		if (isChildren) {
			auto& nodeChildren = object[PARENTHOOD::NODE_CHILDREN];

			u8 nextChildrenCounter = 0;										// Create child counter for the next parenthood.
			auto&& nextParenthood = parenthoods + 1;						// It does deep-search like read we need to store
																			//  hold onto node's parent information.

			u8 nextChildrenCount = nodeChildren.size();						// Get childrenCount for child node.
			u8 nextExtendedChildrenCount = nextChildrenCount; 

			GetExtendedChildrenCount (
				nodeChildren, nodes, nextChildrenCount, nodeMeshTable, 
				nextExtendedChildrenCount
			);

			parenthoodsChildrenTable += childrenCount; 						// Cascade childTable. Pointer here is a copy so that works fine.
			++parenthoods; 													// Cascade parenthoods. Pointer here is a copy so that works fine.

			parenthoods->id = validKeyPos;									// Create new parenthood params.
			parenthoods->base.children = parenthoodsChildrenTable;		
			parenthoods->base.childrenCount = nextExtendedChildrenCount;

			for (u8 iChild = 0; iChild < nextChildrenCount; ++iChild) {

				auto nextNodeId = nodeChildren[iChild].get<int> ();
			
				LoadNode (
					nodes, nextNodeId, meshes, 								// json SECTIONS
					transforms, transformsOffset, mmrlut, 					// Transform & Sorting
					nextParenthood, parenthoods, parenthoodsChildrenTable,	// Parenthood & Cascading
					nextExtendedChildrenCount, nextChildrenCounter, 		//
					nodeMeshTable,											// Extension
					meshTable
				);
			}

		}
	}
}


namespace RESOURCES::GLTF::MESH {

	void GetIndices (
		/* IN  */ std::ifstream*& fileHandlers,
		/* IN  */ Json& bufferViews,
		/* IN  */ Json& accesor,
		/* OUT */ u16& indiciesCount,
		/* OUT */ u16*& indicies
	) {
		const u8 TYPE_SIZE = 1;

		auto& bufferViewNode	= accesor["bufferView"];
		u8 bufferViewId			= bufferViewNode.get<int> ();
		auto& bufferView		= bufferViews[bufferViewId];
		auto& bufferNode		= bufferView["buffer"];
		u8 bufferId				= bufferNode.get<int> ();

		u16 commponentType 		= accesor["componentType"]	.get<int> ();
		u16 count 				= accesor["count"]			.get<int> ();
		std::string type 		= accesor["type"]			.get<std::string> ();
		u32 byteLength 			= bufferView["byteLength"]	.get<int> ();
		u32 byteOffset 			= bufferView["byteOffset"]	.get<int> ();
		u32 target 				= bufferView["target"]		.get<int> ();

		auto& file = fileHandlers[bufferId];
		indiciesCount = count * TYPE_SIZE;
		indicies = new u16[indiciesCount];

		FILE::ReadBytes (indicies, file, byteOffset, byteLength);
	}

	void GetVertices (
		/* IN  */ std::ifstream*& fileHandlers,
		/* IN  */ Json& bufferViews,
		/* IN  */ Json& accesor,
		/* OUT */ r32& verticiesCount,
		/* OUT */ r32*& verticies
	) {
		const u8 TYPE_SIZE = 3;

		auto& bufferViewNode	= accesor["bufferView"];
		u8 bufferViewId			= bufferViewNode.get<int> ();
		auto& bufferView		= bufferViews[bufferViewId];
		auto& bufferNode		= bufferView["buffer"];
		u8 bufferId				= bufferNode.get<int> ();

		u16 commponentType 		= accesor["componentType"]	.get<int> ();			// dataSize
		u16 count 				= accesor["count"]			.get<int> ();			// dataSize
		std::string type 		= accesor["type"]			.get<std::string> ();	// dataSize
		u32 byteLength 			= bufferView["byteLength"]	.get<int> ();			// 
		u32 byteOffset 			= bufferView["byteOffset"]	.get<int> ();			//
		u32 target 				= bufferView["target"]		.get<int> ();			// (34962, standing for ARRAY_BUFFER), or that the data is used for vertex indices (34963, standing for ELEMENT_ARRAY_BUFFER).

		auto& file = fileHandlers[bufferId];
		verticiesCount = count; // We don't multiply by 3 (TYPE_SIZE) because that is arleady happaning later in mesh creation.
		verticies = new r32[verticiesCount * TYPE_SIZE];

		FILE::ReadBytes (verticies, file, byteOffset, byteLength);
	}

	void GetNormals (
		/* IN  */ std::ifstream*& fileHandlers,
		/* IN  */ Json& bufferViews,
		/* IN  */ Json& accesor,
		/* OUT */ r32& normalsCount,
		/* OUT */ r32*& normals
	) {
		const u8 TYPE_SIZE = 3;

		auto& bufferViewNode	= accesor["bufferView"];
		u8 bufferViewId			= bufferViewNode.get<int> ();
		auto& bufferView		= bufferViews[bufferViewId];
		auto& bufferNode		= bufferView["buffer"];
		u8 bufferId				= bufferNode.get<int> ();

		u16 commponentType 		= accesor["componentType"]	.get<int> ();
		u16 count 				= accesor["count"]			.get<int> ();
		std::string type 		= accesor["type"]			.get<std::string> ();
		u32 byteLength 			= bufferView["byteLength"]	.get<int> ();
		u32 byteOffset 			= bufferView["byteOffset"]	.get<int> ();
		u32 target 				= bufferView["target"]		.get<int> ();

		auto& file = fileHandlers[bufferId];
		normalsCount = count;
		normals = new r32[normalsCount * TYPE_SIZE];

		//spdlog::info ("GN: {0}, {1}, {2}", byteLength, byteOffset, target);

		FILE::ReadBytes (normals, file, byteOffset, byteLength);
	}

	void GetUVs (
		/* IN  */ std::ifstream*& fileHandlers,
		/* IN  */ Json& bufferViews,
		/* IN  */ Json& accesor,
		/* OUT */ r32& uvsCount,
		/* OUT */ r32*& uvs
	) {
		const u8 TYPE_SIZE = 2;

		auto& bufferViewNode	= accesor["bufferView"];
		u8 bufferViewId			= bufferViewNode.get<int> ();
		auto& bufferView		= bufferViews[bufferViewId];
		auto& bufferNode		= bufferView["buffer"];
		u8 bufferId				= bufferNode.get<int> ();

		u16 commponentType 		= accesor["componentType"]	.get<int> ();
		u16 count 				= accesor["count"]			.get<int> ();
		std::string type 		= accesor["type"]			.get<std::string> ();
		u32 byteLength 			= bufferView["byteLength"]	.get<int> ();
		u32 byteOffset 			= bufferView["byteOffset"]	.get<int> ();
		u32 target 				= bufferView["target"]		.get<int> ();

		auto& file = fileHandlers[bufferId];
		uvsCount = count;
		uvs = new r32[uvsCount * TYPE_SIZE];

		FILE::ReadBytes (uvs, file, byteOffset, byteLength);
	}

}


namespace RESOURCES::GLTF::MATERIAL {

}


namespace RESOURCES::GLTF {

	void Load (
		/* OUT */ Json json,
		/* IN  */ const LoadHelper& loadContext,
		//
		/* IN  */ const u16& parenthoodsCount,
		/* OUT */ u16* parenthoodsChildrenTable,
		/* OUT */ ::PARENTHOOD::Parenthood* parenthoods,
		//
		/* IN  */ const u16& transformsCount,	
		/* IN  */ const u16& transformsOffset,	
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
		auto& mmrlut = loadContext.mmrlut;											// Material-Mesh Relation Look Up Table
		auto& duplicateObjects = loadContext.duplicateObjects;						//
		auto& nodeMeshTable = loadContext.nodeMeshTable;							//

		// meshTable materialsCount byte.
		meshTable[0] = materialsCount;

		//spdlog::info ("HERE!");

		// 5. Scene sorting and connecting.

		{ // Read MESHES & MATEIRALS
			auto& materialsNode = json["materials"];
			auto& bufferViews = json["bufferViews"];
			auto& accessors = json["accessors"];
			auto& meshesNode = json["meshes"];
			auto& buffers = json["buffers"];

			const auto& buffersCount = buffers.size();

			// Define a structure to temporary hold file information.
			std::ifstream* fileHandlers = new std::ifstream[buffersCount];
			
			for (u16 iBuffer = 0; iBuffer < buffersCount; ++iBuffer) {
				auto& buffer = buffers[iBuffer];
		
				auto byteLength = buffer["byteLength"].get<int> ();
				auto uri = buffer["uri"].get<std::string> ();
				auto str = uri.c_str();
		
				// ADD STRING
				u8 i = 0; for (; str[i] != 0; ++i) {
					FILE::fullString[D_GLTFS_LENGTH + i] = str[i];
				} FILE::fullString[D_GLTFS_LENGTH + i] = 0;
		
				DEBUG_GLTF spdlog::info ("bl: {0}, uri: {1}", byteLength, FILE::fullString);
				FILE::Open (FILE::fullString, fileHandlers[iBuffer]);
			}

			u8 meshCounter = 0;
			for (u8 iMesh = 0; iMesh < meshesNode.size(); ++iMesh) {
				auto& primitivesNode = meshesNode[iMesh]["primitives"];
				//DEBUG spdlog::info ("j");

				for (u8 iPrimitive = 0; iPrimitive < primitivesNode.size(); ++iPrimitive) {
					//DEBUG spdlog::info ("iMesh {0}, iPrimitive {1}", iMesh, iPrimitive);

					auto& attribNode = primitivesNode[iPrimitive]["attributes"];
					auto& indicesNode = primitivesNode[iPrimitive]["indices"];
					auto& vertexNode = attribNode["POSITION"];
					auto& normalNode = attribNode["NORMAL"];
					auto& uvNode = attribNode["TEXCOORD_0"];

					u8 indicesId = indicesNode.get<int> ();
					u8 vertexsId = vertexNode.get<int> ();
					u8 normalsId = normalNode.get<int> ();
					u8 uvsId = uvNode.get<int> ();

					DEBUG spdlog::info ("{0}, {1}, {2}, {3}", indicesId, vertexsId, normalsId, uvsId);

					r32 verticiesCount = 0;
					r32* verticies = nullptr;
					u16 indiciesCount = 0;
					u16* indicies = nullptr;
					r32 normalsCount = 0;
					r32* normals = nullptr;
					r32 uvsCount = 0;
					r32* uvs = nullptr;

					MESH::GetVertices	(fileHandlers, bufferViews, accessors[vertexsId],	verticiesCount,	verticies	);
					MESH::GetIndices	(fileHandlers, bufferViews, accessors[indicesId],	indiciesCount,	indicies	);
					MESH::GetNormals	(fileHandlers, bufferViews, accessors[normalsId],	normalsCount,	normals		);
					MESH::GetUVs		(fileHandlers, bufferViews, accessors[uvsId],		uvsCount,		uvs			);

					DEBUG spdlog::info ("v: {0}, i: {1}, n: {2}, u: {3}", verticiesCount, indiciesCount, normalsCount, uvsCount);

					FILE::LoadMesh (
						meshCounter, meshes, 1, // 1 INSTANCE ! // We need to tell how many possible instances there are to prep buffor size.
						verticiesCount, verticies,
						indiciesCount, indicies,
						normalsCount, normals,
						uvsCount, uvs
					); 

					DEBUG spdlog::info ("a");
					delete[] verticies;
					DEBUG spdlog::info ("b");
					delete[] indicies;
					DEBUG spdlog::info ("c");
					delete[] normals;
					DEBUG spdlog::info ("d");
					delete[] uvs;
					DEBUG spdlog::info ("e");

					{ // Read Materials
						auto materialId 	= primitivesNode[iPrimitive]["material"].get<int> ();
						auto& materialNode 	= materialsNode	[materialId];
						auto& pbr 			= materialNode	["pbrMetallicRoughness"];
						auto& color 		= pbr			["baseColorFactor"];

						materials[materialId].type				= ::MATERIAL::TYPE::COLOR_ONLY;
						materials[materialId].packed.color.r 	= color[0].get<float> ();
						materials[materialId].packed.color.g 	= color[1].get<float> ();
						materials[materialId].packed.color.b 	= color[2].get<float> ();
					}

					++meshCounter;
				}

			}

			DEBUG spdlog::info ("h");
			for (u16 iBuffer = 0; iBuffer < buffers.size(); ++iBuffer) {
				FILE::Close (fileHandlers[iBuffer]);
			}

			//DEBUG spdlog::info ("l");
			delete[] fileHandlers;
			//DEBUG spdlog::info ("k");
		}

		//spdlog::info ("LATER!");

		{ // Transforms & Parenthoods
		
			auto& nodes = json[NODE_NODES];
			auto& meshes = json[MESHES::NODE_MESHES];
		
			{ // ROOT
				TRANSFORM::LTransform transformComponent {}; // 0-initialzie
				auto& transform = transformComponent.base;
		
				transformComponent.id = 0;
				transform.scale = { 1, 1, 1 };
		
				lTransforms[0] = transformComponent;
		
				{ // Root Parenthood
					auto& nodeScene = json[NODE_SCENE];
					u8 defaultScene = nodeScene.get<int> ();
		
					auto& nodeScenes = json[NODE_SCENES];
					auto& nodeNodes = nodeScenes[defaultScene][NODE_NODES];
					auto& root = parenthoods[0];
		
					u8 childrenCount = nodeNodes.size();
					u8 extendedChildrenCount = childrenCount; // Primitive -> Mesh EXTENSION ( creating additional nodes. )
					GetExtendedChildrenCount (nodeNodes, nodes, childrenCount, nodeMeshTable, extendedChildrenCount);

					//spdlog::info ("cc, {0}, ecc: {1}", childrenCount, extendedChildrenCount);
		
					root.id = 0;
					root.base.children = parenthoodsChildrenTable;
					root.base.childrenCount = extendedChildrenCount;
		
					{ // Other Nodes ( including Extended Nodes )
						u8 childrenCounter = 0;
		
						for (u8 iNode = 0; iNode < childrenCount; ++iNode) {
							u8 nodeId = nodeNodes[iNode].get<int> ();
		
							LoadNode (
								nodes, nodeId, meshes, 
								lTransforms, transformsOffset, mmrlut, 					// Transform & Sorting
								parenthoods, parenthoods, parenthoodsChildrenTable, 	// Parenthood & Cascading
								extendedChildrenCount, childrenCounter, 				//
								nodeMeshTable,											// Extension
								meshTable
							);
						}
					}
				}
			}
		}

		//spdlog::info ("AFTER!");

		// Free allocated memory.
		delete[] duplicateObjects;
		delete[] nodeMeshTable;
		delete[] mmrlut;

	}

}
