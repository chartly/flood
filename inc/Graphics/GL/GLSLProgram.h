/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "Graphics/API.h" 
#include "Resources/Resource.h"

namespace fld
{
    struct ResourceEvent;
}

namespace dit {

    class GLSLUniformMetadata
    {
    public:
        int32 size;
        uint32 type;
        String name;
    };

    class GLSLUniform
    {
    public:
        template<typename T>
        void set(const T& val);

        uint32 location;
    };

    class GLSLShaderMetadata
    {
    public:
        ResourceHandle resource;
        uint32 type;
        String code;
    };

    class GLSLShader
    {
    public:
        bool create(GLenum shaderType);
        bool compile(const String& code);

        uint32 id;
    };

    class GLSLProgramMetadata
    {
    public:
        void attach(const ResourceHandle& resourceId, GLenum shaderType, const String& code);

        Array<GLSLShaderMetadata> shaders;
        Array<GLSLUniformMetadata> uniforms;
    };

    class GLSLProgram : public Object
    {
    public:
        bool create(GLSLProgramMetadata metadata);
        void destroy();
        void compile();
        bool link();

        uint32 id;
        uint64 key;
        Array<GLSLShader> shaders;
        Array<GLSLUniform> uniforms;
        GLSLProgramMetadata metadata;
    };

    typedef RefPtr<GLSLProgram> GLSLProgramPtr;

    class GLSLProgramManager
    {
    public:
        GLSLProgramManager();
        ~GLSLProgramManager();

        void init();

        GLSLProgramPtr load(const String & vsFilename, const String & fsFilename);
        void onResourceReload(const ResourceEvent& e);

        HashMap<GLSLProgramPtr> programs;

    private:
        HashMap<Array<uint64>*> dependencies;
    };
}