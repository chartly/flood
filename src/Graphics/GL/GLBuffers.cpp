/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/GL/GLBuffers.h"
#include "Graphics/GL/GLApp.h"

namespace dit {

    void GLBufferObject::create(const GLBufferObjectMetadata& metadata)
    {
        id = 0;

        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, metadata.sizeBytes, metadata.data, metadata.usage);
    }

    //////////////////////////////////////////////////////////////////////////

    void GLVertexBuffer::create()
    {
        id = 0;

        for(const auto& md : metadata.bufferObjects)
        {
            GLBufferObject bo;
            bo.create(md);
            bufferObjects.pushBack(bo);
        }

        glGenVertexArrays(1, &id);
        glBindVertexArray(id);
        
        assert(bufferObjects.size() == metadata.attributes.size());
        for (size_t i = 0; i < bufferObjects.size(); ++i)
        {
            const auto& bo = bufferObjects[i];
            const auto& attr = metadata.attributes[i];

            glBindBuffer(GL_ARRAY_BUFFER, bo.id);
            glVertexAttribPointer(attr.index, attr.componentCount, attr.type, GL_FALSE, attr.stride, attr.offset);
        }

        for(size_t i = 0; i < metadata.attributes.size(); ++i)
            glEnableVertexAttribArray(i);
    }

    void GLVertexBuffer::destroy()
    {
        glBindVertexArray(id);
        
        for(size_t i = 0; i < metadata.attributes.size(); ++i)
            glDisableVertexAttribArray(i);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        for(auto bo : bufferObjects)
            glDeleteBuffers(1, &bo.id);

        bufferObjects.clear();

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &id);
        id = 0;
    }

    //////////////////////////////////////////////////////////////////////////

    void GLIndexBuffer::create()
    {
        bufferObject.create(metadata);
    }

    void GLIndexBuffer::destroy()
    {
        glDeleteBuffers(1, &bufferObject.id);
        bufferObject.id = 0;
        metadata = {};
    }
}