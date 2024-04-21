#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

namespace RESOURCES::SCENE {

		void Create (
			Json& json
		) {
			
			ZoneScopedN("RESOURCES::SCENE: Create");
			DEBUG { spdlog::info ("JSON Scene Initialization"); }

			std::ifstream file;
			file.open ( "res/data/scene.json" );
			file >> json; // Parse the file.

			{ // root

				if ( json.contains ("name") ) {
					auto& name 		= json["name"];
				}

				if ( json.contains ("transform") ) {
					auto& transform	= json["transform"];
				}
            	
				if ( json.contains ("children") ) {
					auto& children	= json["children"];
					auto childrenCount	= children.size ();

					DEBUG spdlog::info ("ChildCount: {0}", childrenCount);
					
				}
            	
			}

			

		}

		void Load() {

		}

}