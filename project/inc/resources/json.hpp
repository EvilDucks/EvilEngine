#pragma once

// NLOHMANN_JSON
#include <nlohmann/json.hpp>

// To simplify the process. Atleast for now.
#include <fstream>
#include <sstream>
#include <string>

namespace RESOURCES {
    using Json = nlohmann::json;

    constexpr char ERROR_CONTAIN[] = "{0} does not contains a '{1}' key!";
}