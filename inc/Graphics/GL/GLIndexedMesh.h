/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "Graphics/API.h"
#include "Graphics/GL/GLBuffers.h"

namespace dit {

    class GLIndexedMesh : public Object
    {
    public:
        GLIndexedMesh() = default;
        GLIndexedMesh(GLVertexBufferMetadata&& vtxMetadata, std::initializer_list<GLBufferObjectMetadata> idxMetadata);

        void create();
        void drawIndexed(uint32 idx);
        void destroy();

        GLVertexBuffer vtxBuffer;
        Array<GLIndexBuffer> idxBuffers;
    };

}