/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "Graphics/API.h"
#include "Graphics/GL/GLBuffers.h"

namespace dit{

    class Quad
    {
    public:
        Quad() = default;
        Quad(const Quad& rhs)
        {
            buffer.metadata.bufferObjects[0].data = vtx.data();
            buffer.metadata.bufferObjects[1].data = tex.data();
        }

        // vertex element data
        std::array<vec3, 6> vtx = { {
            vec3{ -1.f, 0.f, -1.f }, vec3{ -1.f, 0.f, 1.f }, vec3{ 1.f, 0.f, -1.f },
            vec3{ 1.f, 0.f, -1.f }, vec3{ -1.f, 0.f, 1.f }, vec3{ 1.f, 0.f, 1.f } } };
        std::array<vec2, 6> tex = { {
            vec2{ 0.f, 0.f }, vec2{ 1.f, 0.f }, vec2{ 0.f, 1.f },
            vec2{ 0.f, 1.f }, vec2{ 1.f, 0.f }, vec2{ 1.f, 1.f } } };

        // vertex buffer data
        GLVertexBuffer buffer = { {
            { { GL_ARRAY_BUFFER, vtx.data(), sizeof(vec3) * vtx.size(), GL_STATIC_DRAW }
            , { GL_ARRAY_BUFFER, tex.data(), sizeof(vec2) * tex.size(), GL_STATIC_DRAW } },
            { { 0, 3, GL_FLOAT, 0, nullptr }
            , { 1, 2, GL_FLOAT, 0, nullptr } } } };
    };

}