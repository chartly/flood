/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "CS350/API.h"
#include "CS350/GL/GLBuffers.h"

namespace dit {

    class Crosshair
    {
    public:
        Crosshair() = default;
        Crosshair(const Crosshair& rhs)
            : buffer(rhs.buffer)
        {
            buffer.metadata.bufferObjects[0].data = vtx.data();
            buffer.metadata.bufferObjects[1].data = color.data();
        }

        // vertex element data
        std::array<vec3, 6> vtx = { {
            vec3{ 0.f, 0.f, 0.f }, vec3{ 1.f, 0.f, 0.f },
            vec3{ 0.f, 0.f, 0.f }, vec3{ 0.f, 1.f, 0.f },
            vec3{ 0.f, 0.f, 0.f }, vec3{ 0.f, 0.f, 1.f } } };
        std::array<vec3, 6> color = { {
            vec3{ 1.f, 0.f, 0.f }, vec3{ 1.f, 0.f, 0.f },
            vec3{ 0.f, 1.f, 0.f }, vec3{ 0.f, 1.f, 0.f },
            vec3{ 0.f, 0.f, 1.f }, vec3{ 0.f, 0.f, 1.f } } };

        // vertex buffer
        GLVertexBuffer buffer = { {
            { { GL_ARRAY_BUFFER, vtx.data(), sizeof(vec3) * vtx.size(), GL_STATIC_DRAW }
            , { GL_ARRAY_BUFFER, color.data(), sizeof(vec3) * color.size(), GL_STATIC_DRAW } },
            { { 0, 3, GL_FLOAT, 0, nullptr }
            , { 1, 3, GL_FLOAT, 0, nullptr } } } };
    };
}