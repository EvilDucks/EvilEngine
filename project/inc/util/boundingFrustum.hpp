#pragma once

#include "render/gl.hpp"
#include "components/camera.hpp"
#include "components/transform.hpp"

namespace BOUNDINGFRUSTUM
{
    struct Plane
    {
        // unit vector
        glm::vec3 normal = { 0.f, 0.1f, 0.f };
        // distance from origin to the nearest point in the plane
        float distance = 0.f;

        float getSignedDistanceToPlane(const glm::vec3& point) const
        {
            return glm::dot(normal, point) - distance;
        }
    };

    struct Sphere
    {
        glm::vec3 center{0.f, 0.f, 0.f};
        float radius{0.f};

        bool isOnForwardPlane(const Plane& plane)
        {
            return plane.getSignedDistanceToPlane(center) > -radius;
        }
    };

    struct Frustum
    {
        Plane topFace;
        Plane bottomFace;

        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;

        Frustum createFrustumFromCamera(
                CAMERA::Camera& cam,
                float aspect,
                float fovY,     // fovY - angle of adjacent side of the camera  tan(fovY) * zFar = vSide;
                float zNear, float zFar)
        {
            Frustum frustum;
            const float halfVSide = zFar * tanf(fovY * 0.5f);
            const float halfHside = halfVSide * aspect;
            const glm::vec3 frontMultFar = zFar * cam.local.front;

            frustum.nearFace.normal = glm::normalize(cam.local.front);
            frustum.nearFace.distance = glm::dot(frustum.nearFace.normal, cam.local.position + zNear * cam.local.front);

            frustum.farFace.normal = glm::normalize(-cam.local.front);
            frustum.farFace.distance = glm::dot(frustum.farFace.normal, cam.local.position + frontMultFar);

            frustum.rightFace.normal = glm::normalize(glm::cross(frontMultFar - cam.local.right * halfHside, cam.local.up));
            frustum.rightFace.distance = glm::dot(frustum.rightFace.normal, cam.local.position);

            frustum.leftFace.normal = glm::normalize(glm::cross(cam.local.up, frontMultFar + cam.local.right * halfHside));
            frustum.leftFace.distance = glm::dot(frustum.leftFace.normal, cam.local.position);

            frustum.topFace.normal = glm::normalize(glm::cross(cam.local.right, frontMultFar - cam.local.up * halfVSide));
            frustum.topFace.distance = glm::dot(frustum.topFace.normal, cam.local.position);

            frustum.bottomFace.normal = glm::normalize(glm::cross(frontMultFar + cam.local.up * halfVSide, cam.local.right));
            frustum.bottomFace.distance = glm::dot(frustum.bottomFace.normal, cam.local.position);

            return frustum;
        }

    };

    //bool IsOnFrustum(
    //        const Frustum& camFrustum,
    //        glm::mat4& positionGlobal,
    //        float radius
    //)
    //{
    //    Sphere sphere;
    //    sphere.center = glm::vec3(positionGlobal[3]);
    //    glm::vec3 scale = glm::vec3(glm::length(glm::vec3(positionGlobal[0])), glm::length(glm::vec3(positionGlobal[1])), glm::length(glm::vec3(positionGlobal[2])) );
    //
    //    float maxScale = glm::max(glm::max(scale.x, scale.y), scale.z );
    //
    //    sphere.radius = (radius * (maxScale * 0.5f)) * 2.5f; //first maxScale/2 bcs its radius, not diameter, then radius*2.5, to make sure objects close to edges don't dissapear, increase/decrease the value if needed,
    //
    //    return (sphere.isOnForwardPlane(camFrustum.leftFace) &&
    //            sphere.isOnForwardPlane(camFrustum.rightFace) &&
    //            sphere.isOnForwardPlane(camFrustum.farFace) &&
    //            sphere.isOnForwardPlane(camFrustum.nearFace) &&
    //            sphere.isOnForwardPlane(camFrustum.topFace) &&
    //            sphere.isOnForwardPlane(camFrustum.bottomFace)
    //    );
    //}

    bool IsOnFrustum (
        const Frustum& camFrustum,
        TRANSFORM::GTransform* transforms,
        u8& instances,
        float radius
    ) {

        // Znowu trzeba zobaczyć czy obiekt znajduje się wew. 6 planów.
        //  Tylko teraz trzeba dodatkowo:
        //  a) Sprawdzić na kilku obiektach -> transfroms[instances]
        //  b) Przygotować posortowane pozycje, które są widoczne do 
        //   umieszczenia w zawołaniu do funkcji `glBufferSubData()`.

        // Właściwie zamiast Is(bool) może to być Set(void)
        //  tą funkcją zmieniamy wyjściową ilość instancji możliwe i równą 0.
        //  , więc pytanie czy korzystać z IF'a czy też i nie przejmować się niepotrzebnym
        //  wywołaniem funkcji, które w takim efekcie i tak nic nie zrobią.

        return true;
    }
};