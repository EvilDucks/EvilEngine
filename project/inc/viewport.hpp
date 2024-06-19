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
    };


    void CreateLoad (VIEWPORT::Viewport*& viewports, const s32& viewportsCount) {
		if (viewportsCount) viewports = new VIEWPORT::Viewport[viewportsCount]; // allocation

		{ // Viewport 1
			auto& viewport = viewports[0];
			auto& camera = viewport.camera;

			camera.local.position 			= glm::vec3 (2.0f, 0.0f, 8.0f);
            camera.local.targetPos          = camera.local.position;
			camera.local.worldUp			= glm::vec3 (0.0f, 1.0f, 0.0f);
			camera.local.front				= glm::vec3 (0.0f, 0.0f, -1.0f);
			camera.type						= CAMERA::CameraType::THIRD_PERSON;
			camera.local.yaw				= CAMERA::YAW;
			camera.local.pitch				= CAMERA::PITCH;
			camera.local.zoom				= CAMERA::ZOOM;
			camera.local.distance			= CAMERA::DIST_FROM_TARGET;
			camera.local.mouseSensitivity	= CAMERA::SENSITIVITY;
			camera.local.moveSpeed			= CAMERA::SPEED;

			CAMERA::UpdateCameraVectors (camera);
		}

		{ // Viewport 2
			auto& viewport = viewports[1];
			auto& camera = viewport.camera;

			camera.local.position			= glm::vec3 (2.0f, 0.0f, 8.0f);
            camera.local.targetPos          = camera.local.position;
			camera.local.worldUp			= glm::vec3 (0.0f, 1.0f, 0.0f);
			camera.local.front				= glm::vec3 (0.0f, 0.0f, -1.0f);
			camera.type						= CAMERA::CameraType::THIRD_PERSON;
			camera.local.yaw				= CAMERA::YAW;
			camera.local.pitch				= CAMERA::PITCH;
			camera.local.zoom				= CAMERA::ZOOM;
			camera.local.mouseSensitivity	= CAMERA::SENSITIVITY;
			camera.local.moveSpeed			= CAMERA::SPEED;

			CAMERA::UpdateCameraVectors (camera);
		}
	}

	void Destroy(VIEWPORT::Viewport*& viewports) {
		delete[] viewports;
	}

}