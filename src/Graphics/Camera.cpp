/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "CS350/API.h"
#include "CS350/Camera.h"

namespace dit {

    Camera::Camera()
        : _position(vec3(0,0,0))
        , _orientation(one<mat4>())
        , _fov(quarter_pi<float>())
        , _nearPlane(0.01f)
        , _farPlane(1000.0f)
        , _viewAspectRatio(1280.0f / 720.0f)
    {}

    const vec3& Camera::position() const
    {
        return _position;
    }

    vec3& Camera::position()
    {
        return _position;
    }

    void Camera::translate(const vec3& v)
    {
        _position += v;
    }

    const float& Camera::fov() const
    {
        return _fov;
    }

    float& Camera::fov()
    {
        return _fov;
    }

    const float& Camera::nearPlane() const
    {
        return _nearPlane;
    }

    float& Camera::nearPlane()
    {
        return _nearPlane;
    }

    const float& Camera::farPlane() const
    {
        return _farPlane;
    }

    float& Camera::farPlane()
    {
        return _farPlane;
    }

    const mat4& Camera::orientation() const
    {
        return _orientation;
    }

    mat4& Camera::orientation()
    {
        return _orientation;
    }

    void Camera::rotate(const vec3& axis, float angleRadians)
    {
        _orientation = glm::rotate(_orientation, angleRadians, axis);
    }

    void Camera::lookAt(const vec3& pos, const vec3& up)
    {
        _orientation = glm::lookAt(_position, pos, up);
    }

    const float& Camera::viewAspectRatio() const
    {
        return _viewAspectRatio;
    }

    float& Camera::viewAspectRatio()
    {
        return _viewAspectRatio;
    }

    vec3 Camera::forward() const
    {
        vec4 f = inverse(_orientation) * vec4{ 0.f, 0.f, -1.f, 0.f };
        return vec3{ f.x, f.y, f.z };
    }

    vec3 Camera::right() const
    {
        return cross(forward(), vec3{ 0.f, 1.f, 0.f });
    }

    vec3 Camera::up() const
    {
        return vec3{ 0.f, 1.f, 0.f };
    }

    mat4 Camera::viewProjection() const
    {
        return projection() * view();
    }

    mat4 Camera::projection() const
    {
        return perspective(_fov, _viewAspectRatio, _nearPlane, _farPlane);
    }

    mat4 Camera::view() const
    {
        return _orientation * glm::translate(one<mat4>(), -_position);
    }
}