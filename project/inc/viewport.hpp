#pragma once

#include "render/gl.hpp"
#include "components/camera.hpp"
#include "util/boundingFrustum.hpp"

namespace VIEWPORT {

    struct Viewport {
        CAMERA::Camera camera;
        BOUNDINGFRUSTUM::Frustum cameraFrustum;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 previousView = glm::mat4(1.f);
        glm::mat4 previousProjection = glm::mat4(1.f);
    };

}