#pragma once

#include "render/gl.hpp"
#include "components/camera.hpp"
#include "util/boundingFrustum.hpp"

namespace VIEWPORT {

    struct data {
        u8 index;
        WIN::WindowTransform viewPortWindowTransform;
        CAMERA::Camera* camera;
        glm::mat4* view;
        glm::mat4* projection;
        BOUNDINGFRUSTUM::Frustum* camFrustum;
    };

}