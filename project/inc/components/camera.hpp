#pragma once

#include <iostream>
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace CAMERA {

    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    enum CameraType {
        FREE,
        THIRD_PERSON
    };


    // Default camera values
    const float YAW         = -90.0f;
    const float PITCH       =  0.0f;

    const float SPEED       =  2.5f;
    const float SENSITIVITY =  0.1f;
    const float ZOOM        =  60.0f;
    const float DIST_FROM_TARGET = 5.0f;

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
    using MovementSpeed = float;
    using Distance = float;

    struct Local {
        Position position;
        Position targetPos;
        Front front;
        Up up;
        Right right;
        WorldUp worldUp;

        Yaw yaw;
        Pitch pitch;

        Distance distance;

        Zoom zoom;
        MouseSensitivity mouseSensitivity;
        MovementSpeed moveSpeed;
    };

    struct Camera {
        Local local;
        CameraType type;
    };

    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors(Camera& camera)
    {
        if (camera.local.pitch > 55.0f)
            camera.local.pitch = 55.0f;
        if (camera.local.pitch < -75.0f)
            camera.local.pitch = -75.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(camera.local.yaw)) * cos(glm::radians(camera.local.pitch));
        front.y = sin(glm::radians(camera.local.pitch));
        front.z = sin(glm::radians(camera.local.yaw)) * cos(glm::radians(camera.local.pitch));

        camera.local.front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        camera.local.right = glm::normalize(glm::cross(camera.local.front, camera.local.worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        camera.local.up = glm::normalize(glm::cross(camera.local.right, camera.local.front));
    }

    void UpdateCamPos(Camera& camera, glm::vec3& target)
    {
        if(camera.type == CameraType::THIRD_PERSON)
        {
            //auto distance = DIST_FROM_TARGET;
            auto distance = DIST_FROM_TARGET;
            distance -= glm::abs(camera.local.pitch)/85.f * DIST_FROM_TARGET;

            if(distance < 1.f) distance = 1.f;
            camera.local.targetPos = target - camera.local.front * distance;

        }
    }

    glm::mat4 GetViewMatrix(Camera& camera, glm::vec3& target)
    {
        if(camera.type == CameraType::THIRD_PERSON)
        {
            return glm::lookAt(camera.local.position, target, camera.local.up);
        }
        return glm::lookAt(camera.local.position, camera.local.position + camera.local.front, camera.local.up);
    }

    void ProcessZoom(Camera& camera, float zoomValue)
    {
        camera.local.zoom += zoomValue;
        if (camera.local.zoom < 1.0f)
            camera.local.zoom = 1.0f;
        if (camera.local.zoom > 60.0f)
            camera.local.zoom = 60.0f;
    }

    void ProcessKeyboard(Camera& camera, CameraMovement direction, float deltaTime )
    {
        float velocity = camera.local.moveSpeed * deltaTime;
        if (direction == FORWARD)
            camera.local.position += camera.local.front * velocity;
        if (direction == BACKWARD)
            camera.local.position -= camera.local.front * velocity;
        if (direction == LEFT)
            camera.local.position -= camera.local.right * velocity;
        if (direction == RIGHT)
            camera.local.position += camera.local.right * velocity;
        if (direction == UP)
            camera.local.position += camera.local.up * velocity;
        if (direction == DOWN)
            camera.local.position -= camera.local.up * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovementY(Camera& camera, float yoffset)
    {
        yoffset *= camera.local.mouseSensitivity;
        camera.local.pitch -= yoffset;

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors(camera);
    }

    void ProcessMouseMovementX(Camera& camera, float xoffset)
    {
        xoffset *= camera.local.mouseSensitivity;

        camera.local.yaw += xoffset;

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors(camera);
    }

}
