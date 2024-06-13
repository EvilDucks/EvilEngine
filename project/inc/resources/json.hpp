#pragma once

// NLOHMANN_JSON
#include <nlohmann/json.hpp>

// To simplify the process. Atleast for now.
#include <fstream>
#include <sstream>
#include <string>

namespace RESOURCES {
	using Json = nlohmann::json;

	constexpr char ERROR_CONTAIN[] = "Invalid {0} file. Does not contain: {1} key.";
	//constexpr char ERROR_CONTAIN[] = "{0} does not contains a '{1}' key!";

	void Parse (
		/* OUT */ Json& json,
		/* IN  */ const char* filepath
	) {		
		std::ifstream fileHandler;			// Define a structure to temporary hold file information.
		fileHandler.open ( filepath );		// Load file data into the structure.

		DEBUG if (fileHandler.fail()) {		// ONLY debug mode - validate file path!
			ErrorExit ("JSON - Invalid filepath: {0}!", filepath);
		}

		fileHandler >> json;				// Parse the file to nlohmann/json data format.
		fileHandler.close();				// Be aware that nlohmann/json copies the data inside the `fileHandler` structure.
											//  therefore we free and close the fileHandler resources right after.
	}


	void SortAssign (
		const u8& elementsCount,
		const Json& elements,
		u8*& copyArray
	) {
		u8 start = 0;

		for (u8 i = 1; i < elementsCount; ++i) {
			const Json& element = elements[i];
			u8 value = element.get<int> ();
				
			// Get Value position in array.
			u8 at = start; for (; value < copyArray[at]; --at);

			// Arithmetic condition if smaller, if bigger or same (it skips both prev and next for loop) 
			u8 count = at - (at != start); 

			// Move elements by one to the right.
			for (u8 elementBack = start; elementBack > count; --elementBack) {
				copyArray[elementBack + 1] = copyArray[elementBack];
			}

			// Finally emplace that value at right position.
			++start;
			copyArray[at + 1] = value;
		}
	}
}