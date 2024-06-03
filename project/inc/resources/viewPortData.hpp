#pragma once

#include "render/gl.hpp"
#include "components/camera.hpp"
#include "util/boundingFrustum.hpp"

namespace VIEWPORT {

    struct data {
        CAMERA::Camera camera;
        BOUNDINGFRUSTUM::Frustum camFrustum;
        glm::mat4 view{};
        glm::mat4 projection{};

        u64 colliderIndex;
    };

}