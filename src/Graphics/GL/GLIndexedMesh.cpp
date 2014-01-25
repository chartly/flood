/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/GL/GLIndexedMesh.h"
#include "Graphics/GL/GLApp.h"

namespace dit {

    GLIndexedMesh::GLIndexedMesh(GLVertexBufferMetadata&& vtxMetadata, std::initializer_list<GLBufferObjectMetadata> idxMetadata)
        : vtxBuffer(std::move(vtxMetadata))
    {
        for (const auto& md : idxMetadata)
            idxBuffers.pushBack({ md });
    }
    
    void GLIndexedMesh::drawIndexed(uint32 idx)
    {
        // ...
    }

    void GLIndexedMesh::destroy()
    {
        for (auto& ib : idxBuffers)
            ib.destroy();

        vtxBuffer.destroy();
    }
}