/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "CS350/API.h"
#include "Core/Object.h"

namespace dit {

    class GLBufferObjectMetadata
    {
    public:
        GLBufferObjectMetadata() = default;
        GLBufferObjectMetadata(uint32 _type, void * _data, uint32 _sizeBytes, uint32 _usage)
            : type(_type), data(_data), sizeBytes(_sizeBytes), usage(_usage)
        {}

        uint32 type = 0;
        void * data = nullptr;
        uint32 sizeBytes = 0;
        uint32 usage = 0;
    };

    class GLBufferObject
    {
    public:
        void create(const GLBufferObjectMetadata& metadata);

        uint32 id = 0;
    };

    class GLVertexLayoutMetadata
    {
    public:
        GLVertexLayoutMetadata() = default;
        GLVertexLayoutMetadata(uint32 _index, uint32 _componentCount, uint32 _type, uint32 _stride, void * _offset)
            : index(_index), componentCount(_componentCount), type(_type), stride(_stride), offset(_offset)
        {}

        uint32 index = 0;
        uint32 componentCount = 0;
        uint32 type = 0;
        uint32 stride = 0;
        void * offset = nullptr;
    };

    class GLVertexBufferMetadata
    {
    public:
        GLVertexBufferMetadata() = default;
        GLVertexBufferMetadata(std::initializer_list<GLBufferObjectMetadata> _bufferOjects, std::initializer_list<GLVertexLayoutMetadata> _attributes)
            : bufferObjects(_bufferOjects)
            , attributes(_attributes)
        {}
        GLVertexBufferMetadata(GLVertexBufferMetadata&& rhs)
            : bufferObjects(std::move(rhs.bufferObjects))
            , attributes(std::move(rhs.attributes))
        {}

        Array<GLBufferObjectMetadata> bufferObjects;
        Array<GLVertexLayoutMetadata> attributes;
    };

    class GLVertexBuffer : public Object
    {
    public:
        GLVertexBuffer() = default;
        GLVertexBuffer(GLVertexBufferMetadata&& _metadata)
            : metadata(std::move(_metadata))
        {}

        void create();
        void destroy();

        uint32 id = 0;
        Array<GLBufferObject> bufferObjects;
        GLVertexBufferMetadata metadata;
    };

    typedef RefPtr<GLVertexBuffer> GLVertexBufferPtr;

    class GLIndexBuffer : public Object
    {
    public:
        GLIndexBuffer() = default;
        GLIndexBuffer(const GLBufferObjectMetadata& _metadata)
            : metadata(_metadata)
        {}

        void create();
        void destroy();

        GLBufferObject bufferObject;
        GLBufferObjectMetadata metadata;
    };

    typedef RefPtr<GLIndexBuffer> GLIndexBufferPtr;
}