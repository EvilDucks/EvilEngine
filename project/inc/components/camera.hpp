#pragma once

#include <iostream>
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace CAMERA {

    // Default camera values
    const float YAW         = -90.0f;
    const float PITCH       =  0.0f;

    const float SPEED       =  2.5f;
    const float SENSITIVITY =  0.1f;
    const float ZOOM        =  60.0f;

    // camera attributes
    using Position = glm::vec3;
    using Front = glm::vec3;
    using Up = glm::vec3;
    using Right = glm::vec3;
    using WorldUp = glm::vec3;

    // euler angles
    using Yaw = float;
    using Pitch = float;

    // camera settings
    using Zoom = float;
    using MouseSensitivity = float;

    struct Base {
        Position position;
        Front front;
        Up up;
        Right right;
        WorldUp worldUp;

        Yaw yaw;
        Pitch pitch;

        Zoom zoom;
        MouseSensitivity mouseSensitivity;
    };

    struct Camera {
        GameObjectID id = 0;
        Base local;
    };

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors(Camera& camera)
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(camera.local.yaw)) * cos(glm::radians(camera.local.pitch));
        front.y = sin(glm::radians(camera.local.pitch));
        front.z = sin(glm::radians(camera.local.yaw)) * cos(glm::radians(camera.local.pitch));
        camera.local.front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        camera.local.right = glm::normalize(glm::cross(camera.local.front, camera.local.worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        camera.local.up = glm::normalize(glm::cross(camera.local.right, camera.local.front));
    }

    glm::mat4 GetViewMatrix(Camera& camera)
    {
        return glm::lookAt(camera.local.position, camera.local.position + camera.local.front, camera.local.up);
    }


    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovementY(Camera& camera, float yoffset)
    {
        yoffset *= camera.local.mouseSensitivity;

        camera.local.pitch -= yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        //if (constrainPitch)
         {
             if (camera.local.pitch > 89.0f)
                 camera.local.pitch = 89.0f;
             if (camera.local.pitch < -89.0f)
                 camera.local.pitch = -89.0f;
         }
        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors(camera);
    }

    void ProcessMouseMovementX(Camera& camera, float xoffset)
    {
        xoffset *= camera.local.mouseSensitivity;

        camera.local.yaw += xoffset;

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors(camera);
    }
}