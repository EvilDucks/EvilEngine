//
// Created by Bartek on 10.05.2024.
//

#ifndef EVILENGINE_MODEL_HPP
#define EVILENGINE_MODEL_HPP

#include "../render/mesh.hpp"
#include "../render/material.hpp"

namespace MODEL {

    struct Model {
        u8 materialsCount;
        MATERIAL::Material* materials;
        u8 meshesCount;
        MESH::Mesh* meshes;
    };

}

#endif //EVILENGINE_MODEL_HPP
