/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "Graphics/API.h"

namespace dit {

    class Camera
    {
    public:
        Camera();

        const vec3& position() const;
        vec3& position();
        void translate(const vec3& v);

        const float& fov() const;
        float& fov();

        const float& nearPlane() const;
        float& nearPlane();
        const float& farPlane() const;
        float& farPlane();

        const mat4& orientation() const;
        mat4& orientation();

        void rotate(const vec3& axis, float angleRadians);
        void lookAt(const vec3& pos, const vec3& up);

        const float& viewAspectRatio() const;
        float& viewAspectRatio();

        vec3 forward() const;
        vec3 right() const;
        vec3 up() const;

        mat4 viewProjection() const;
        mat4 projection() const;
        mat4 view() const;

    private:
        vec3 _position;
        mat4 _orientation;
        float _fov;
        float _nearPlane;
        float _farPlane;
        float _viewAspectRatio;
    };

}